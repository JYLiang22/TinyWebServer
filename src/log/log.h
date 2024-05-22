/*
* @author JYLiang
* @date 2024.5.22
*/

#ifndef _LOG_H_
#define _LOG_H_

#include <mutex>
#include <string>
#include <thread>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <sys/stat.h>
#include "./blockqueue.cpp"
#include "../buffer/buffer.h"

// Log class for managing asynchronous logging
class Log {
private:
    Log(); // Constructor
    void appendLogLevelTitle(int level); // Appends log level prefix
    virtual ~Log(); // Destructor
    void asyncWrite(); // Handles asynchronous writing to log file

private:
    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;

    const char *path; // Log file path
    const char *suffix; // Log file suffix

    int MAX_LINES_; // Maximum lines per log file
    int lineCount; // Current line count
    int toDay; // Current day for log rotation
    bool isOpen_; // Indicates if logging is open

    Buffer buff; // Buffer for log entries
    int level; // Current log level
    bool isAsync; // Indicates if logging is asynchronous

    FILE *fp; // File pointer for the log file
    std::unique_ptr<BlockQueue<std::string>> deq; // Queue for asynchronous logging
    std::unique_ptr<std::thread> writeThread; // Thread for asynchronous logging
    std::mutex mtx; // Mutex for thread safety

public:
    void init(int level, const char *path = "./log", const char *suffix = ".log", int maxQueueCapacity = 1024);

    static Log *instance(); // Singleton instance
    static void flushLogThread(); // Thread function for flushing logs

    void write(int level, const char *format, ...);
    void flush();

    int getLevel();
    void setLevel(int level);
    bool isOpen();
};

#define LOG_BASE(level, format, ...) \
    do { \
        Log *log = Log::instance(); \
        if (log->isOpen() && log->getLevel() <= level) { \
            log->write(level, format, ##__VA_ARGS__); \
            log->flush(); \
        } \
    } while(0);

#define LOG_DEBUG(format, ...) do { LOG_BASE(0, format, ##__VA_ARGS__) } while(0);
#define LOG_INFO(format, ...) do { LOG_BASE(1, format, ##__VA_ARGS__) } while(0);
#define LOG_WARN(format, ...) do { LOG_BASE(2, format, ##__VA_ARGS__) } while(0);
#define LOG_ERROR(format, ...) do { LOG_BASE(3, format, ##__VA_ARGS__) } while(0);

#endif
