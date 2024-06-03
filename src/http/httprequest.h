/*
* @author : JYLiang
* @date : 2024.5.31
*/

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <unordered_map>    // For storing headers and POST parameters
#include <unordered_set>    // For storing default HTML paths
#include <string>           // For using std::string
#include <regex>            // For parsing request lines and headers
#include <errno.h>          // For error handling
#include <mysql/mysql.h>    // For MySQL database operations

#include "../buffer/buffer.h"            // Custom buffer class
#include "../log/log.h"                  // Custom logging class
#include "../sqlconnectpool/sqlconnpool.h"  // MySQL connection pool
#include "../sqlconnectpool/sqlconnRAII.h"  // RAII wrapper for MySQL connections

// Define the HttpRequest class
class HttpRequest {
public:
    // Enumeration for the state of the request parsing
    enum PARSE_STATE {
        REQUEST_LINE,   // Parsing the request line
        HEADERS,        // Parsing the headers
        BODY,           // Parsing the body
        FINISH,         // Parsing complete
    };

    // Enumeration for the HTTP response codes
    enum HTTP_CODE {
        NO_REQUEST = 0,         // No request
        GET_REQUEST,            // GET request
        BAD_REQUEST,            // Bad request
        NO_RESOURSE,            // No resource found
        FORBIDDENT_REQUEST,     // Forbidden request
        FILE_REQUEST,           // Request for a file
        INTERNAL_ERROR,         // Internal server error
        CLOSED_CONNECTION,      // Connection closed
    };

    // Constructor initializing the request
    HttpRequest() { Init(); }

    // Destructor
    ~HttpRequest() = default;

    // Initialize the request state
    void Init();

    // Parse the HTTP request
    bool parse(Buffer& buff);

    // Getters for various request components
    std::string path() const;
    std::string& path();
    std::string method() const;
    std::string version() const;
    std::string GetPost(const std::string& key) const;
    std::string GetPost(const char* key) const;

    // Check if the connection should be kept alive
    bool IsKeepAlive() const;

    // Functions to be implemented later
    /*
    todo
    void HttpConn::ParseFormData() {}
    void HttpConn::ParseJson() {}
    */

private:
    // Parsing helper functions
    bool ParseRequestLine_(const std::string& line);
    void ParseHeader_(const std::string& line);
    void ParseBody_(const std::string& line);

    // Further parsing functions
    void ParsePath_();
    void ParsePost_();
    void ParseFromUrlencoded_();

    // User verification function
    static bool UserVerify(const std::string& name, const std::string& pwd, bool isLogin);

    // Private member variables
    PARSE_STATE state_;                             // Current parsing state
    std::string method_, path_, version_, body_;    // Request components
    std::unordered_map<std::string, std::string> header_;   // Headers map
    std::unordered_map<std::string, std::string> post_;     // POST parameters map

    // Constants for default HTML paths and tags
    static const std::unordered_set<std::string> DEFAULT_HTML;
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG;

    // Convert a hex character to its integer value
    static int ConverHex(char ch);
};

#endif // HTTP_REQUEST_H