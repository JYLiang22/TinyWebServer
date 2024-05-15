/*
* @author   JYLiang
* @date     2024.5.15
*/

#ifndef _SQLCONNPOOL_H_
#define _SQLCONNPOOL_H_

#include<mysql/mysql.h>
#include<string>
#include<queue>
#include<mutex>
#include<semaphore.h>
#include<thread>
#include"../log/log.h"

class SqlConnPool{
private:
    SqlConnPool();
    ~SqlConnPool();

    int Max_Conn;       // 最大连接数
    int useCount;       // 在用连接数
    int freeCount;      // 可用连接数

    std::queue<MYSQL *> connQue;    // MySQL连接队列
    std::mutex mtx;
    sem_t sem_id;

public:
    static SqlConnPool *Instance();

    MYSQL *GetConn();
    void FreeConn(MYSQL *conn);
    int GetFreeConn();

    void Init(const char *host, int port, 
              const char *user, const char *pwd, 
              const char *dbName, int connSize);
    void ClosePool();
};

#endif
