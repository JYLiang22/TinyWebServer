/*
* @author : JYLiang
* @date : 2024.6.3
*/

// Include the implementation of the HttpResponse class
#include "httpresponse.h"

using namespace std;

// Define the static mapping of file extensions to content types
const unordered_map<string, string> HttpResponse::SUFFIX_TYPE = {
    { ".html",  "text/html" },
    { ".xml",   "text/xml" },
    { ".xhtml", "application/xhtml+xml" },
    { ".txt",   "text/plain" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".css",   "text/css"},
    { ".js",    "text/javascript"},
};

// Define the static mapping of HTTP response codes to status messages
const unordered_map<int, string> HttpResponse::CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};

// Define the static mapping of HTTP response codes to error file paths
const unordered_map<int, string> HttpResponse::CODE_PATH = {
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};

// Constructor: Initialize default values
HttpResponse::HttpResponse() {
    code_ = -1;  // Default to -1 indicating no code set
    path_ = srcDir_ = "";  // Initialize paths to empty strings
    isKeepAlive_ = false;  // Default keep-alive to false
    mmFile_ = nullptr;  // Initialize memory-mapped file pointer to null
    mmFileStat_ = { 0 };  // Initialize file status structure to zero
}

// Destructor: Ensure proper cleanup by unmapping the file
HttpResponse::~HttpResponse() {
    UnmapFile();  // Unmap the file if it is mapped
}

// Initialize the response with source directory, path, keep-alive flag, and HTTP code
void HttpResponse::Init(const string& srcDir, string& path, bool isKeepAlive, int code) {
    assert(srcDir != "");  // Ensure source directory is not empty
    if(mmFile_) { UnmapFile(); }  // Unmap any previously mapped file
    code_ = code;  // Set the HTTP response code
    isKeepAlive_ = isKeepAlive;  // Set the keep-alive flag
    path_ = path;  // Set the requested file path
    srcDir_ = srcDir;  // Set the source directory
    mmFile_ = nullptr;  // Initialize memory-mapped file pointer to null
    mmFileStat_ = { 0 };  // Initialize file status structure to zero
}

// Build the HTTP response and store it in the provided buffer
void HttpResponse::MakeResponse(Buffer& buff) {
    // Check if the requested resource exists and is accessible
    if(stat((srcDir_ + path_).data(), &mmFileStat_) < 0 || S_ISDIR(mmFileStat_.st_mode)) {
        code_ = 404;  // Set response code to 404 if the file doesn't exist or is a directory
    }
    else if(!(mmFileStat_.st_mode & S_IROTH)) {
        code_ = 403;  // Set response code to 403 if the file is not readable by others
    }
    else if(code_ == -1) { 
        code_ = 200;  // Default to 200 OK if no code was set
    }
    ErrorHtml_();  // Generate error HTML if needed
    AddStateLine_(buff);  // Add the status line to the response buffer
    AddHeader_(buff);  // Add the headers to the response buffer
    AddContent_(buff);  // Add the content to the response buffer
}

// Return a pointer to the memory-mapped file
char* HttpResponse::File() {
    return mmFile_;
}

// Return the length of the memory-mapped file
size_t HttpResponse::FileLen() const {
    return mmFileStat_.st_size;
}

// Generate error HTML content if needed
void HttpResponse::ErrorHtml_() {
    // If the response code has a corresponding error path
    if(CODE_PATH.count(code_) == 1) {
        path_ = CODE_PATH.find(code_)->second;  // Set the path to the error file
        stat((srcDir_ + path_).data(), &mmFileStat_);  // Get the file status for the error file
    }
}

// Add the HTTP status line to the response buffer
void HttpResponse::AddStateLine_(Buffer& buff) {
    string status;
    // If the response code has a corresponding status message
    if(CODE_STATUS.count(code_) == 1) {
        status = CODE_STATUS.find(code_)->second;  // Get the status message
    }
    else {
        code_ = 400;  // Default to 400 Bad Request if no valid code
        status = CODE_STATUS.find(400)->second;  // Get the status message for 400
    }
    buff.append("HTTP/1.1 " + to_string(code_) + " " + status + "\r\n");  // Append the status line to the buffer
}

// Add the HTTP headers to the response buffer
void HttpResponse::AddHeader_(Buffer& buff) {
    buff.append("Connection: ");  // Append the Connection header
    if(isKeepAlive_) {
        buff.append("keep-alive\r\n");  // Append keep-alive if needed
        buff.append("keep-alive: max=6, timeout=120\r\n");  // Append keep-alive options
    } else {
        buff.append("close\r\n");  // Append close if not keep-alive
    }
    buff.append("Content-type: " + GetFileType_() + "\r\n");  // Append the Content-Type header
}

// Add the HTTP content to the response buffer
void HttpResponse::AddContent_(Buffer& buff) {
    int srcFd = open((srcDir_ + path_).data(), O_RDONLY);  // Open the requested file
    if(srcFd < 0) { 
        ErrorContent(buff, "File NotFound!");  // Add error content if file not found
        return; 
    }

    // Log the file path for debugging
    LOG_DEBUG("file path %s", (srcDir_ + path_).data());
    
    // Memory-map the file to improve access speed
    int* mmRet = (int*)mmap(0, mmFileStat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    if(*mmRet == -1) {
        ErrorContent(buff, "File NotFound!");  // Add error content if mmap failed
        return; 
    }
    mmFile_ = (char*)mmRet;  // Set the memory-mapped file pointer
    close(srcFd);  // Close the file descriptor
    buff.append("Content-length: " + to_string(mmFileStat_.st_size) + "\r\n\r\n");  // Append the Content-Length header
}

// Unmap the file if it has been memory-mapped
void HttpResponse::UnmapFile() {
    if(mmFile_) {
        munmap(mmFile_, mmFileStat_.st_size);  // Unmap the file from memory
        mmFile_ = nullptr;  // Reset the memory-mapped file pointer
    }
}

// Determine the file type based on the file extension
string HttpResponse::GetFileType_() {
    // Find the last occurrence of '.' in the path
    string::size_type idx = path_.find_last_of('.');
    if(idx == string::npos) {
        return "text/plain";  // Default to text/plain if no extension found
    }
    string suffix = path_.substr(idx);  // Extract the file extension
    if(SUFFIX_TYPE.count(suffix) == 1) {
        return SUFFIX_TYPE.find(suffix)->second;  // Return the corresponding content type
    }
    return "text/plain";  // Default to text/plain if no matching type found
}

// Add error content to the buffer
void HttpResponse::ErrorContent(Buffer& buff, string message) {
    string body;  // String to hold the body of the error message
    string status;  // String to hold the status message
    body += "<html><title>Error</title>";  // Add HTML title
    body += "<body bgcolor=\"ffffff\">";  // Add body with background color
    // Get the status message corresponding to the code
    if(CODE_STATUS.count(code_) == 1) {
        status = CODE_STATUS.find(code_)->second;
    } else {
        status = "Bad Request";  // Default to "Bad Request" if no matching code
    }
    body += to_string(code_) + " : " + status + "\n";  // Add the status line to the body
    body += "<p>" + message + "</p>";  // Add the error message to the body
    body += "<hr><em>TinyWebServer</em></body></html>";  // Add a footer to the body

    buff.append("Content-length: " + to_string(body.size()) + "\r\n\r\n");  // Append the Content-Length header
    buff.append(body);  // Append the error body to the buffer
}