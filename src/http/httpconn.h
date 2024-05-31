/*
* @author : JYLiang
* @date : 2024.5.30
*/

#ifndef _HTTPCONN_H_
#define _HTTPCONN_H_

#include <sys/types.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>

#include "../log/log.h"
#include "../sqlconnectpool/sqlconnRAII.h"
#include "../buffer/buffer.h"
#include "httprequest.h"
#include "httpresponse.h"

class HttpConn {
private:
    int fd;  // File descriptor for the socket connection
    struct sockaddr_in addr;  // Client address
    bool isClose;  // Connection status flag
    int iovCnt;  // Number of iovec structures
    struct iovec iov[2];  // Array for scatter-gather I/O
    Buffer readBuff;  // Buffer for reading data
    Buffer writeBuff;  // Buffer for writing data
    HttpRequest request;  // HTTP request object
    HttpResponse response;  // HTTP response object

public:
    HttpConn();  // Constructor
    ~HttpConn();  // Destructor
    void init(int sockFd, const sockaddr_in &addr);  // Initialize connection
    ssize_t read(int *saveErrno);  // Read data from socket
    ssize_t write(int *saveErrno);  // Write data to socket
    void Close();  // Close the connection
    int getFd() const;  // Get the file descriptor
    int getPort() const;  // Get the port number
    const char *getIP() const;  // Get the IP address
    sockaddr_in getAddr() const;  // Get the address structure
    bool process();  // Process the HTTP request
    int toWriteBytes();  // Get the number of bytes to write
    bool isKeepAlive() const;  // Check if the connection should be kept alive

    static bool isET;  // Edge-triggered mode flag
    static const char *srcDir;  // Source directory for files
    static std::atomic<int> userCnt;  // User connection count
};

#endif
