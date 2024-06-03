/*
* @author : JYLiang
* @date : 2024.5.31
*/

#include "httprequest.h"  // Include the header for HttpRequest
using namespace std;

// Define the static member for default HTML paths
const unordered_set<string> HttpRequest::DEFAULT_HTML{
    "/index", "/register", "/login",
    "/welcome", "/video", "/picture",
};

// Define the static member for default HTML tags
const unordered_map<string, int> HttpRequest::DEFAULT_HTML_TAG {
    {"/register.html", 0}, {"/login.html", 1},
};

// Initialize the HTTP request
void HttpRequest::Init() {
    method_ = path_ = version_ = body_ = "";  // Reset all request components
    state_ = REQUEST_LINE;                    // Set initial state to REQUEST_LINE
    header_.clear();                          // Clear the headers map
    post_.clear();                            // Clear the POST parameters map
}

// Check if the connection should be kept alive
bool HttpRequest::IsKeepAlive() const {
    if(header_.count("Connection") == 1) {
        // Check if the "Connection" header is "keep-alive" and HTTP version is 1.1
        return header_.find("Connection")->second == "keep-alive" && version_ == "1.1";
    }
    return false;
}

// Parse the HTTP request from the buffer
bool HttpRequest::parse(Buffer& buff) {
    const char CRLF[] = "\r\n";  // Define the CRLF sequence used in HTTP headers
    if(buff.readableBytes() <= 0) {  // If the buffer has no readable bytes, return false
        return false;
    }
    while(buff.readableBytes() && state_ != FINISH) {  // Loop until buffer is empty or parsing is finished
        const char* lineEnd = search(buff.peek(), buff.beginWriteConst(), CRLF, CRLF + 2);  // Find the end of the line
        std::string line(buff.peek(), lineEnd);  // Create a string from the beginning to the end of the line
        switch(state_)  // Handle based on the current parsing state
        {
            case REQUEST_LINE:
                if(!ParseRequestLine_(line)) {  // Parse the request line
                    return false;
                }
                ParsePath_();  // Parse the request path
                break;
            case HEADERS:
                ParseHeader_(line);  // Parse the headers
                if(buff.readableBytes() <= 2) {  // If only CRLF is left, finish parsing
                    state_ = FINISH;
                }
                break;
            case BODY:
                ParseBody_(line);  // Parse the body
                break;
            default:
                break;
        }
        if(lineEnd == buff.beginWrite()) { break; }  // If we have reached the end of the buffer, break the loop
        buff.retrieveUntil(lineEnd + 2);  // Retrieve the processed data from the buffer
    }
    LOG_DEBUG("[%s], [%s], [%s]", method_.c_str(), path_.c_str(), version_.c_str());  // Log the request details
    return true;
}

// Parse the request path and modify it if necessary
void HttpRequest::ParsePath_() {
    if(path_ == "/") {
        path_ = "/index.html";  // Default path is /index.html if the root is requested
    }
    else {
        for(auto &item: DEFAULT_HTML) {
            if(item == path_) {
                path_ += ".html";  // Append .html to the path if it matches a default HTML path
                break;
            }
        }
    }
}

// Parse the request line
bool HttpRequest::ParseRequestLine_(const string& line) {
    regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");  // Define a regex pattern for the request line
    smatch subMatch;  // Define a match object to hold the results
    if(regex_match(line, subMatch, patten)) {  // If the line matches the pattern
        method_ = subMatch[1];  // Extract the HTTP method
        path_ = subMatch[2];  // Extract the request path
        version_ = subMatch[3];  // Extract the HTTP version
        state_ = HEADERS;  // Move to parsing headers
        return true;
    }
    LOG_ERROR("RequestLine Error");  // Log an error if the request line is invalid
    return false;
}

// Parse a header line
void HttpRequest::ParseHeader_(const string& line) {
    regex patten("^([^:]*): ?(.*)$");  // Define a regex pattern for the header line
    smatch subMatch;  // Define a match object to hold the results
    if(regex_match(line, subMatch, patten)) {  // If the line matches the pattern
        header_[subMatch[1]] = subMatch[2];  // Extract the header name and value
    }
    else {
        state_ = BODY;  // If the line does not match, move to parsing the body
    }
}

// Parse the request body
void HttpRequest::ParseBody_(const string& line) {
    body_ = line;  // Store the body content
    ParsePost_();  // Parse the POST parameters from the body
    state_ = FINISH;  // Mark parsing as complete
    LOG_DEBUG("Body:%s, len:%d", line.c_str(), line.size());  // Log the body content and length
}

// Convert a hexadecimal character to an integer
int HttpRequest::ConverHex(char ch) {
    if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;  // Convert uppercase hex character
    if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;  // Convert lowercase hex character
    return ch;  // Return the character itself if it's not a hex character
}

// Parse the POST parameters
void HttpRequest::ParsePost_() {
    if(method_ == "POST" && header_["Content-Type"] == "application/x-www-form-urlencoded") {
        ParseFromUrlencoded_();  // Parse URL-encoded form data
        if(DEFAULT_HTML_TAG.count(path_)) {
            int tag = DEFAULT_HTML_TAG.find(path_)->second;  // Get the tag for the requested path
            LOG_DEBUG("Tag:%d", tag);  // Log the tag value
            if(tag == 0 || tag == 1) {
                bool isLogin = (tag == 1);  // Determine if it's a login or registration
                if(UserVerify(post_["username"], post_["password"], isLogin)) {
                    path_ = "/welcome.html";  // Redirect to welcome page if verification is successful
                } 
                else {
                    path_ = "/error.html";  // Redirect to error page if verification fails
                }
            }
        }
    }   
}

// Parse URL-encoded form data
void HttpRequest::ParseFromUrlencoded_() {
    if(body_.size() == 0) { return; }  // Return if the body is empty

    string key, value;  // Variables to hold the key and value
    int num = 0;  // Variable to hold the converted hex value
    int n = body_.size();  // Get the size of the body
    int i = 0, j = 0;  // Initialize indices

    for(; i < n; i++) {  // Loop through each character in the body
        char ch = body_[i];
        switch (ch) {
        case '=':  // Equals sign indicates the end of a key
            key = body_.substr(j, i - j);  // Extract the key
            j = i + 1;  // Move to the start of the value
            break;
        case '+':  // Plus sign represents a space
            body_[i] = ' ';
            break;
        case '%':  // Percent sign indicates an encoded character
            num = ConverHex(body_[i + 1]) * 16 + ConverHex(body_[i + 2]);  // Convert the hex value
            body_[i + 2] = num % 10 + '0';  // Store the ones place of the converted value
            body_[i + 1] = num / 10 + '0';  // Store the tens place of the converted value
            i += 2;  // Skip the next two characters
            break;
        case '&':  // Ampersand indicates the end of a key-value pair
            value = body_.substr(j, i - j);  // Extract the value
            j = i + 1;  // Move to the start of the next key
            post_[key] = value;  // Store the key-value pair in the POST map
            LOG_DEBUG("%s = %s", key.c_str(), value.c_str());  // Log the key-value pair
            break;
        default:
            break;
        }
    }
    assert(j <= i);  // Ensure that j is not greater than i
    if(post_.count(key) == 0 && j < i) {  // If the last key-value pair has not been added yet
        value = body_.substr(j, i - j);  // Extract the value
        post_[key] = value;  // Store the key-value pair in the POST map
    }
}

// Verify user credentials
bool HttpRequest::UserVerify(const string &name, const string &pwd, bool isLogin) {
    if(name == "" || pwd == "") { return false; }  // Return false if either username or password is empty
    LOG_INFO("Verify name:%s pwd:%s", name.c_str(), pwd.c_str());  // Log the verification attempt
    MYSQL* sql;
    SqlConnRAII(&sql, SqlConnPool::Instance());  // Get a MySQL connection from the pool
    assert(sql);  // Ensure the connection is valid
    
    bool flag = false;  // Flag to indicate if the verification is successful
    unsigned int j = 0;  // Variable to hold the number of fields
    char order[256] = { 0 };  // Buffer to hold the SQL query
    MYSQL_FIELD *fields = nullptr;  // Variable to hold the field metadata
    MYSQL_RES *res = nullptr;  // Variable to hold the result set
    
    if(!isLogin) { flag = true; }  // If it's a registration, set the flag to true
    // Prepare the SQL query to select the user
    snprintf(order, 256, "SELECT username, password FROM user WHERE username='%s' LIMIT 1", name.c_str());
    LOG_DEBUG("%s", order);  // Log the SQL query

    if(mysql_query(sql, order)) {  // Execute the SQL query
        mysql_free_result(res);  // Free the result set
        return false;  // Return false if the query fails
    }
    res = mysql_store_result(sql);  // Store the result set
    j = mysql_num_fields(res);  // Get the number of fields in the result set
    fields = mysql_fetch_fields(res);  // Get the field metadata

    while(MYSQL_ROW row = mysql_fetch_row(res)) {  // Loop through the result set
        LOG_DEBUG("MYSQL ROW: %s %s", row[0], row[1]);  // Log the row data
        string password(row[1]);  // Get the password from the row
        if(isLogin) {  // If it's a login attempt
            if(pwd == password) { flag = true; }  // Check if the password matches
            else {
                flag = false;  // Set the flag to false if the password does not match
                LOG_DEBUG("pwd error!");  // Log the error
            }
        } 
        else { 
            flag = false;  // Set the flag to false if the user is already registered
            LOG_DEBUG("user used!");  // Log the error
        }
    }
    mysql_free_result(res);  // Free the result set

    if(!isLogin && flag == true) {  // If it's a registration and the username is not used
        LOG_DEBUG("register!");  // Log the registration
        bzero(order, 256);  // Clear the query buffer
        snprintf(order, 256, "INSERT INTO user(username, password) VALUES('%s','%s')", name.c_str(), pwd.c_str());  // Prepare the insert query
        LOG_DEBUG("%s", order);  // Log the insert query
        if(mysql_query(sql, order)) {  // Execute the insert query
            LOG_DEBUG("Insert error!");  // Log the error if the query fails
            flag = false;  // Set the flag to false
        }
        flag = true;  // Set the flag to true if the query succeeds
    }
    SqlConnPool::Instance()->FreeConn(sql);  // Free the MySQL connection
    LOG_DEBUG("UserVerify success!!");  // Log the success
    return flag;  // Return the flag indicating the verification result
}

// Getter for the request path (const version)
std::string HttpRequest::path() const {
    return path_;
}

// Getter for the request path (non-const version)
std::string& HttpRequest::path() {
    return path_;
}

// Getter for the request method
std::string HttpRequest::method() const {
    return method_;
}

// Getter for the request version
std::string HttpRequest::version() const {
    return version_;
}

// Getter for a POST parameter by key (std::string version)
std::string HttpRequest::GetPost(const std::string& key) const {
    assert(key != "");  // Ensure the key is not empty
    if(post_.count(key) == 1) {  // Check if the key exists in the POST map
        return post_.find(key)->second;  // Return the value associated with the key
    }
    return "";  // Return an empty string if the key does not exist
}

// Getter for a POST parameter by key (const char* version)
std::string HttpRequest::GetPost(const char* key) const {
    assert(key != nullptr);  // Ensure the key is not null
    if(post_.count(key) == 1) {  // Check if the key exists in the POST map
        return post_.find(key)->second;  // Return the value associated with the key
    }
    return "";  // Return an empty string if the key does not exist
}