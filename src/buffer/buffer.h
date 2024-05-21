/*
* @author JYLiang
* @date 2024.5.20
*/

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <cstring>      // for perror
#include <iostream>
#include <unistd.h>     // for write
#include <sys/uio.h>    // for readv
#include <vector>       // for dynamic buffer management
#include <atomic>       // for atomic operations on positions
#include <assert.h>     // for runtime assertions

class Buffer {
private:
    // Returns a non-const pointer to the beginning of the buffer
    char* beginPtr();
    // Returns a const pointer to the beginning of the buffer
    const char* beginPtr() const;
    // Ensures there is enough space to accommodate additional data
    void makeSpace(size_t len);

    // Underlying storage for the buffer
    std::vector<char> buffer;
    // Positions in the buffer managed atomically
    std::atomic<std::size_t> readPos;
    std::atomic<std::size_t> writePos;

public:
    // Constructor initializes the buffer with a given size
    Buffer(int initBufferSize = 1024);
    // Default destructor
    ~Buffer() = default;

    // Returns the number of writable bytes in the buffer
    size_t writeableBytes() const;
    // Returns the number of readable bytes in the buffer
    size_t readableBytes() const;
    // Returns the number of bytes that can be prepended
    size_t prependableBytes() const;

    // Returns a const pointer to the current read position
    const char* peek() const;
    // Ensures there is enough writable space for len bytes
    void ensureWriteable(size_t len);
    // Updates the write position after writing len bytes
    void hasWritten(size_t len);

    // Moves the read position forward by len bytes
    void retrieve(size_t len);
    // Moves the read position to a specific position
    void retrieveUntil(const char* end);

    // Clears the buffer
    void retrieveAll();
    // Retrieves all data as a string and clears the buffer
    std::string retrieveAlltoStr();

    // Returns a const pointer to the current write position
    const char* beginWriteConst() const;
    // Returns a non-const pointer to the current write position
    char* beginWrite();

    // Appends a string to the buffer
    void append(const std::string& str);
    // Appends len bytes from str to the buffer
    void append(const char* str, size_t len);
    // Appends len bytes from data to the buffer
    void append(const void* data, size_t len);
    // Appends another buffer's contents to this buffer
    void append(const Buffer& buff);

    // Reads data from a file descriptor into the buffer
    ssize_t readFd(int fd, int* saveErrno);
    // Writes data from the buffer to a file descriptor
    ssize_t writeFd(int fd, int* saveError);
};

#endif // _BUFFER_H_
