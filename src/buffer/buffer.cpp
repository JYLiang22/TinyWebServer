/*
* @author   JYLiang
* @date     2024.5.20
*/

#include"./buffer.h"

char* Buffer::beginPtr(){
    return &(*buffer.begin());
}

const char* Buffer::beginPtr() const{
    return &(*buffer.begin());
}

void Buffer::makeSpace(size_t len){
    if(writeableBytes() + prependableBytes() < len){
        buffer.resize(writePos + len + 1);
    }
    else{
        size_t readable = readableBytes();
        std::copy(beginPtr() + readPos, beginPtr() + writePos, beginPtr());
        readPos = 0;
        writePos = readPos + readable;
        assert(readable == readableBytes());
    }
}

Buffer::Buffer(int initBufferSize) : buffer(initBufferSize), readPos(0), writePos(0) {}

size_t Buffer::readableBytes() const{
    return writePos - readPos;
}

size_t Buffer::writeableBytes() const{
    return buffer.size() - writePos;
}

size_t Buffer::prependableBytes() const{
    return readPos;
}

const char* Buffer::peek() const{
    return beginPtr() + readPos;
}

void Buffer::retrieve(size_t len){
    assert(len <= readableBytes());
    readPos += len;
}

void Buffer::retrieveUntil(const char *end){
    assert(peek() <= end);
    retrieve(end - peek());
}

void Buffer::retrieveAll(){
    bzero(&buffer[0], buffer.size());
    readPos = 0;
    writePos = 0;
}

std::string Buffer::retrieveAlltoStr(){
    std::string str(peek(), readableBytes());
    retrieveAll();
    return str;
}

const char* Buffer::beginWriteConst() const{
    return beginPtr() + writePos;
}

char* Buffer::beginWrite(){
    return beginPtr() + writePos;
}

void Buffer::hasWritten(size_t len){
    writePos += len;
}

void Buffer::append(const std::string &str){
    append(str.data(), str.length());
}

void Buffer::append(const void *data, size_t len){
    assert(data);
    append(static_cast<const char *>(data), len);
}

void Buffer::append(const char *str, size_t len){
    assert(str);
    ensureWriteable(len);
    std::copy(str, str + len, beginWrite());
    hasWritten(len);
}

void Buffer::append(const Buffer &buff){
    append(buff.peek(), buff.readableBytes());
}

void Buffer::ensureWriteable(size_t len){
    if(writeableBytes() < len){
        makeSpace(len);
    }
    assert(writeableBytes() >= len);
}

ssize_t Buffer::readFd(int fd, int *saveErrno){
    char buff[65536];
    struct iovec iov[2];
    const size_t writeable = writeableBytes();
    // 分散读，保证数据全部读完
    iov[0].iov_base = beginPtr() + writePos;
    iov[0].iov_len = writeable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if(len < 0){
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writeable){
        writePos += len;
    }
    else{
        writePos = buffer.size();
        append(buff, len - writeable);
    }
    return len;
}

ssize_t Buffer::writeFd(int fd, int *saveError){
    size_t readSize = readableBytes();
    ssize_t len = write(fd, peek(), readSize);
    if(len < 0){
        *saveError = errno;
        return len;
    }
    readPos += len;
    return len;
}


