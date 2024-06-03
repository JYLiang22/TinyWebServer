/*
* @author : JYLiang
* @date : 2024.5.30
*/

#include "httpconn.h"
using namespace std;

// Static member variable initialization
bool HttpConn::isET;  // Edge-triggered mode flag
const char* HttpConn::srcDir;  // Source directory for files
std::atomic<int> HttpConn::userCnt;  // User connection count

// Constructor
HttpConn::HttpConn() {
    fd = -1;  // Initialize file descriptor to -1 (invalid)
    addr = { 0 };  // Zero initialize the sockaddr_in structure
    isClose = true;  // Mark the connection as closed
}

// Destructor
HttpConn::~HttpConn() {
    Close();  // Ensure the connection is closed
}

// Initialize the connection with a file descriptor and address
void HttpConn::init(int fd, const sockaddr_in &addr) {
    assert(fd > 0);  // Ensure the file descriptor is valid
    ++userCnt;  // Increment the user count
    this->addr = addr;  // Copy the address
    this->fd = fd;  // Set the file descriptor
    writeBuff.retrieveAll();  // Clear the write buffer
    readBuff.retrieveAll();  // Clear the read buffer
    isClose = false;  // Mark the connection as open
    LOG_INFO("Client[%d](%s:%d) in, userCnt:%d", fd, getIP(), getPort(), (int)userCnt);  // Log the connection
}

// Read data from the socket into the read buffer
ssize_t HttpConn::read(int *saveErrno) {
    ssize_t len = -1;  // Length of data read
    do {
        len = readBuff.readFd(fd, saveErrno);  // Read data into the buffer
        if (len <= 0) {
            break;  // Break if no data was read or an error occurred
        }
    } while (isET);  // Continue reading if edge-triggered mode is enabled
    return len;
}

// Write data from the write buffer to the socket
ssize_t HttpConn::write(int *saveErrno) {
    ssize_t len = -1;  // Length of data written
    do {
        len = writev(fd, iov, iovCnt);  // Write data using scatter-gather I/O
        if (len <= 0) {
            *saveErrno = errno;  // Save the error number
            break;
        }
        if (iov[0].iov_len + iov[1].iov_len == 0) {
            break;  // Break if all data has been written
        } else if (static_cast<size_t>(len) > iov[0].iov_len) {
            // Adjust the second iovec structure if more data was written than in the first iovec
            iov[1].iov_base = (uint8_t*)iov[1].iov_base + (len - iov[0].iov_len);
            iov[1].iov_len -= (len - iov[0].iov_len);
            if (iov[0].iov_len) {
                writeBuff.retrieveAll();  // Clear the write buffer
                iov[0].iov_len = 0;
            }
        } else {
            // Adjust the first iovec structure if only part of the data was written
            iov[0].iov_base = (uint8_t*)iov[0].iov_base + len;
            iov[0].iov_len -= len;
            writeBuff.retrieve(len);  // Remove written data from the buffer
        }
    } while (isET || toWriteBytes() > 10240);  // Continue writing if edge-triggered mode or data to write exceeds 10KB
    return len;
}

// Close the connection
void HttpConn::Close() {
    response.UnmapFile();  // Unmap the file associated with the response
    if (!isClose) {
        isClose = true;  // Mark the connection as closed
        --userCnt;  // Decrement the user count
        close(fd);  // Close the socket
        LOG_INFO("Client[%d](%s:%d) quit, userCnt:%d", fd, getIP(), getPort(), (int)userCnt);  // Log the disconnection
    }
}

// Get the file descriptor
int HttpConn::getFd() const {
    return fd;
}

// Get the address structure
struct sockaddr_in HttpConn::getAddr() const {
    return addr;
}

// Get the IP address as a string
const char* HttpConn::getIP() const {
    return inet_ntoa(addr.sin_addr);
}

// Get the port number
int HttpConn::getPort() const {
    return addr.sin_port;
}

// Process the HTTP request and prepare the response
bool HttpConn::process() {
    request.Init();  // Initialize the request
    if (readBuff.readableBytes() <= 0) {
        return false;  // No data to process
    } else if (request.parse(readBuff)) {
        // If the request was successfully parsed
        LOG_DEBUG("%s", request.path().c_str());
        response.Init(srcDir, request.path(), request.IsKeepAlive(), 200);  // Initialize a successful response
    } else {
        response.Init(srcDir, request.path(), false, 400);  // Initialize an error response
    }

    response.MakeResponse(writeBuff);  // Create the response and write it to the buffer
    iov[0].iov_base = const_cast<char*>(writeBuff.peek());  // Set the first iovec to the write buffer data
    iov[0].iov_len = writeBuff.readableBytes();
    iovCnt = 1;

    if (response.FileLen() > 0 && response.File()) {
        // If there is a file to be sent as part of the response
        iov[1].iov_base = response.File();
        iov[1].iov_len = response.FileLen();
        iovCnt = 2;
    }
    LOG_DEBUG("filesize:%d, %d to %d", response.FileLen(), iovCnt, toWriteBytes());
    return true;
}

// Get the total number of bytes to write
int HttpConn::toWriteBytes() {
    return iov[0].iov_len + iov[1].iov_len;
}

// Check if the connection should be kept alive
bool HttpConn::isKeepAlive() const {
    return request.IsKeepAlive();
}
