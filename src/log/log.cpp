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
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t tSec = now.tv_sec;
    struct tm *sysTime = localtime(&tSec);
    struct tm t = *sysTime;
    va_list vaList;

    /* Log date and log line count */
    if (toDay != t.tm_mday || (lineCount && (lineCount % MAX_LINES == 0))) {
        unique_lock<mutex> locker(mtx);
        locker.unlock();

        char newFile[LOG_NAME_LEN];
        char tail[36] = {0};
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

        if (toDay != t.tm_mday) {
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", path, tail, suffix);
            toDay = t.tm_mday;
            lineCount = 0;
        } else {
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s-%d%s", path, tail, (lineCount / MAX_LINES), suffix);
        }

        locker.lock();
        flush();
        fclose(fp);
        fp = fopen(newFile, "a");
        assert(fp != nullptr);
    }

    {
        unique_lock<mutex> locker(mtx);
        lineCount++;
        int n = snprintf(buff.beginWrite(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                        t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                        t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);
        buff.hasWritten(n);
        appendLogLevelTitle(level);

        va_start(vaList, format);
        int m = vsnprintf(buff.beginWrite(), buff.writeableBytes(), format, vaList);
        va_end(vaList);

        buff.hasWritten(m);
        buff.append("\n\0", 2);

        if (isAsync && deq && !deq->full()) {
            deq->push_back(buff.retrieveAlltoStr());
        } else {
            fputs(buff.peek(), fp);
        }
        buff.retrieveAll();
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