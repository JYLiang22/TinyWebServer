/*
* @author   JYLiang
* @date     2024.5.15
*/

#include<assert.h>
#include"./sqlconnpool.h"
using namespace std;

SqlConnPool::SqlConnPool(){
    useCount = 0;
    freeCount = 0;
}

SqlConnPool* SqlConnPool::Instance(){
    static SqlConnPool connPool;
    return &connPool;
}

void SqlConnPool::Init(const char *host, int port, const char *user, const char *pwd, 
                       const char *dbName, int connSize = 10){
    assert(connSize > 0);
    for(int i = 0; i < connSize; ++i){
        MYSQL *sql = nullptr;
        sql = mysql_init(sql);
        if(!sql){
            LOG_ERROR("MySql init error!");
            assert(sql);
        }
        sql = mysql_real_connect(sql, host, user, pwd, dbName, port, nullptr, 0);
        if(!sql){
            LOG_ERROR("MySql connect error!");
        }
        connQue.push(sql);
    }
    Max_Conn = connSize;
    sem_init(&sem_id, 0, Max_Conn);
}

MYSQL* SqlConnPool::GetConn(){
    MYSQL *sql = nullptr;
    if(connQue.empty()){
        LOG_WARN("SqlConnPool busy!");
        return nullptr;
    }
    sem_wait(&sem_id);
    /*
    在 C++ 中，在代码块周围添加大括号 {} 是一种常见做法，用于确定范围，特别是在处理需要在有限范围内获取和释放的锁或其他资源时。
    大括号 {} 定义了 lock_guard 对象锁定器的范围。
    这样可以确保进入块时获取锁，退出块时释放锁，从而防止潜在的死锁并确保特定范围内的线程安全。
    */
    {
        lock_guard<mutex> locker(mtx);
        sql = connQue.front();
        connQue.pop();
    }
    return sql;
}

void SqlConnPool::FreeConn(MYSQL *sql){
    assert(sql);
    lock_guard<mutex> locker(mtx);
    connQue.push(sql);
    sem_post(&sem_id);
}

void SqlConnPool::ClosePool(){
    lock_guard<mutex> locker(mtx);
    while(!connQue.empty()){
        auto item = connQue.front();
        connQue.pop();
        mysql_close(item);
    }
    mysql_library_end();
}

int SqlConnPool::GetFreeConn(){
    lock_guard<mutex> locker(mtx);
    return connQue.size();
}

SqlConnPool::~SqlConnPool(){
    ClosePool();
}