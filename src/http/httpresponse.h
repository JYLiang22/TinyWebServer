/*
* @author : JYLiang
* @date : 2024.6.3
*/

// Ensure the class is only defined once
#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

// Include necessary headers
#include <unordered_map>  // For using unordered_map
#include <fcntl.h>        // For file control options (e.g., open)
#include <unistd.h>       // For POSIX API (e.g., close)
#include <sys/stat.h>     // For file status (e.g., stat)
#include <sys/mman.h>     // For memory management (e.g., mmap, munmap)

#include "../buffer/buffer.h" // Custom buffer class for handling I/O operations
#include "../log/log.h"       // Custom logging class for logging messages

class HttpResponse {
public:
    // Constructor: Initializes the response object with default values
    HttpResponse();
    
    // Destructor: Ensures any memory-mapped files are unmapped
    ~HttpResponse();

    // Initializes the response object with parameters
    void Init(const std::string& srcDir, std::string& path, bool isKeepAlive = false, int code = -1);

    // Constructs the HTTP response and stores it in the buffer
    void MakeResponse(Buffer& buff);

    // Unmaps the memory-mapped file
    void UnmapFile();

    // Returns a pointer to the memory-mapped file
    char* File();

    // Returns the length of the memory-mapped file
    size_t FileLen() const;

    // Adds error content to the buffer
    void ErrorContent(Buffer& buff, std::string message);

    // Returns the HTTP response code
    int Code() const { return code_; }

private:
    // Adds the status line to the response buffer
    void AddStateLine_(Buffer &buff);

    // Adds headers to the response buffer
    void AddHeader_(Buffer &buff);

    // Adds content to the response buffer
    void AddContent_(Buffer &buff);

    // Generates error HTML content if needed
    void ErrorHtml_();

    // Determines the file type based on the file extension
    std::string GetFileType_();

    // HTTP response code
    int code_;

    // Keep-alive flag
    bool isKeepAlive_;

    // Path to the requested file
    std::string path_;

    // Directory containing the requested file
    std::string srcDir_;
    
    // Pointer to the memory-mapped file
    char* mmFile_; 

    // File status structure
    struct stat mmFileStat_;

    // Mappings for file extensions to content types
    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;

    // Mappings for HTTP response codes to status messages
    static const std::unordered_map<int, std::string> CODE_STATUS;

    // Mappings for HTTP response codes to error file paths
    static const std::unordered_map<int, std::string> CODE_PATH;
};

#endif //HTTP_RESPONSE_H