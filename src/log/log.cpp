#include "./log.h"
using namespace std;

// Constructor: initializes log variables
Log::Log() {
    lineCount = 0;
    isAsync = false;
    writeThread = nullptr;
    deq = nullptr;
    toDay = 0;
    fp = nullptr;
}

// Destructor: cleans up resources and ensures all logs are flushed
Log::~Log() {
    if (writeThread && writeThread->joinable()) {
        while (!deq->empty()) {
            deq->flush();
        }
        deq->close();
        writeThread->join();
    }
    if (fp) {
        lock_guard<mutex> locker(mtx);
        flush();
        fclose(fp);
    }
}

// Returns the current log level
int Log::getLevel() {
    lock_guard<mutex> locker(mtx);
    return level;
}

// Sets the log level
void Log::setLevel(int level) {
    lock_guard<mutex> locker(mtx);
    this->level = level;
}

// Checks if the logging system is open
bool Log::isOpen() {
    lock_guard<mutex> locker(mtx);
    return isOpen_;
}

// Initializes the logging system
void Log::init(int level, const char *path, const char *suffix, int maxQueueCapacity) {
    isOpen_ = true;
    this->level = level;
    if (maxQueueCapacity > 0) {
        isAsync = true;
        if (!deq) {
            unique_ptr<BlockQueue<std::string>> newDeq(new BlockQueue<std::string>);
            deq = move(newDeq);
            std::unique_ptr<std::thread> newThread(new thread(flushLogThread));
            writeThread = move(newThread);
        }
    } else {
        isAsync = false;
    }

    lineCount = 0;
    time_t timer = time(nullptr);
    struct tm *sysTime = localtime(&timer);
    struct tm t = *sysTime;
    this->path = path;
    this->suffix = suffix;
    char filename[LOG_NAME_LEN] = {0};
    snprintf(filename, LOG_NAME_LEN - 1, "%s/%04d_%02d_%02d%s", path, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix);
    toDay = t.tm_mday;

    {
        lock_guard<mutex> locker(mtx);
        buff.retrieveAll();
        if (fp) {
            flush();
            fclose(fp);
        }

        fp = fopen(filename, "a");
        if (fp == nullptr) {
            mkdir(path, 0777);
            fp = fopen(filename, "a");
        }
        assert(fp != nullptr);
    }
}

// Writes a log entry
void Log::write(int level, const char *format, ...) {
    // Get the current time
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t tSec = now.tv_sec;
    struct tm *sysTime = localtime(&tSec);
    struct tm t = *sysTime;
    va_list vaList;

    // Check if a new day has started or if the log line count has reached the maximum allowed lines per file
    if (toDay != t.tm_mday || (lineCount && (lineCount % MAX_LINES == 0))) {
        unique_lock<mutex> locker(mtx);  // Lock the mutex to ensure thread safety
        locker.unlock();  // Unlock it for some reason, possibly an error in the code

        char newFile[LOG_NAME_LEN];
        char tail[36] = {0};

        // Create a date string for the log file name
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

        if (toDay != t.tm_mday) {
            // If the day has changed, create a new log file with the date
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", path, tail, suffix);
            toDay = t.tm_mday;
            lineCount = 0;
        } else {
            // If the log line count has reached the max lines per file, create a new log file with a counter
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s-%d%s", path, tail, (lineCount / MAX_LINES), suffix);
        }

        /*
        Understanding 36
        The 36 used in the line char tail[36] = {0}; 
        and in the call to snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday); 
        is a size value for the tail buffer, which holds the date string formatted as YYYY_MM_DD. 
        This value is calculated as follows:

        The date format string is "%04d_%02d_%02d".
        %04d represents the year, which takes up 4 characters.
        _ is a separator character, which takes up 1 character.
        %02d represents the month, which takes up 2 characters.
        _ is another separator character, which takes up 1 character.
        %02d represents the day, which takes up 2 characters.
        Therefore, the total number of characters required for the date string is 4 + 1 + 2 + 1 + 2 = 10 characters.
        However, allocating 36 characters ensures that there is ample space for any additional characters or 
        future modifications, as well as ensuring that the buffer is sufficiently large to avoid buffer overflow 
        in any circumstances.

        Understanding 72
        The 72 in the line snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", path, tail, suffix); 
        is used to calculate the maximum length of the formatted string and ensure it fits within the newFile buffer. 
        The value is derived based on the maximum size of LOG_NAME_LEN and the components of the file path and name.

        Here¡¯s how 72 might be determined:

        LOG_NAME_LEN is a predefined constant representing the maximum allowed length of the log file name.
        Subtracting 72 from LOG_NAME_LEN reserves space for the directory path (path), the date string (tail), 
        and the file suffix (suffix).
        Assuming LOG_NAME_LEN is set to a large value to accommodate the longest possible log file path and name, 
        72 is chosen to leave sufficient room for additional characters that might be added due to formatting, 
        directory separators, or other unexpected characters.

        For example:

        If LOG_NAME_LEN is 256:
        256 - 72 = 184 characters are available for the path, tail, and suffix combined.
        The 72 characters are reserved for extra safety and ensuring the filename does not exceed the buffer length.
        Ensuring Buffer Safety
        Both 36 and 72 are chosen to provide ample space for the strings being formatted and to prevent buffer overflow, 
        ensuring that the function operates safely within the defined buffer sizes.
        */

        locker.lock();  // Lock the mutex again
        flush();  // Flush the current log file
        fclose(fp);  // Close the current log file
        fp = fopen(newFile, "a");  // Open a new log file for appending
        assert(fp != nullptr);  // Ensure the file opened successfully
    }

    {
        unique_lock<mutex> locker(mtx);  // Lock the mutex for thread safety
        lineCount++;  // Increment the log line count
        // Write the timestamp to the log buffer
        int n = snprintf(buff.beginWrite(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                         t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                         t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);
        buff.hasWritten(n);  // Mark the buffer as having written the timestamp
        appendLogLevelTitle(level);  // Append the log level title to the buffer

        va_start(vaList, format);  // Initialize the variable argument list
        // Write the formatted log message to the buffer
        int m = vsnprintf(buff.beginWrite(), buff.writeableBytes(), format, vaList);
        va_end(vaList);  // Clean up the variable argument list

        buff.hasWritten(m);  // Mark the buffer as having written the log message
        buff.append("\n\0", 2);  // Append a newline and null terminator to the buffer

        if (isAsync && deq && !deq->full()) {
            // If asynchronous logging is enabled and the queue is not full
            deq->push_back(buff.retrieveAlltoStr());  // Add the log message to the queue
        } else {
            // Otherwise, write the log message directly to the file
            fputs(buff.peek(), fp);
        }
        buff.retrieveAll();  // Clear the buffer
    }
}

// Appends a log level prefix to the log entry
void Log::appendLogLevelTitle(int level) {
    switch (level) {
        case 0:
            buff.append("[debug]: ", 9);
            break;
        case 1:
            buff.append("[info] : ", 9);
            break;
        case 2:
            buff.append("[warn] : ", 9);
            break;
        case 3:
            buff.append("[error]: ", 9);
            break;
        default:
            buff.append("[info] : ", 9);
            break;
    }
}

// Flushes the log buffer to the file
void Log::flush() {
    if (isAsync) {
        deq->flush();
    }
    fflush(fp);
}

// Handles asynchronous writing to the log file
void Log::asyncWrite() {
    string str = "";
    while (deq->pop(str)) {
        lock_guard<mutex> locker(mtx);
        fputs(str.c_str(), fp);
    }
}

// Returns the singleton instance of the Log class
Log* Log::instance() {
    static Log inst;
    return &inst;
}

// Thread function for flushing logs asynchronously
void Log::flushLogThread() {
    Log::instance()->asyncWrite();
}