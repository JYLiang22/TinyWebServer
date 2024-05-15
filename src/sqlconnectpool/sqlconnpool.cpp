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
    �� C++ �У��ڴ������Χ��Ӵ����� {} ��һ�ֳ�������������ȷ����Χ���ر����ڴ�����Ҫ�����޷�Χ�ڻ�ȡ���ͷŵ�����������Դʱ��
    ������ {} ������ lock_guard �����������ķ�Χ��
    ��������ȷ�������ʱ��ȡ�����˳���ʱ�ͷ������Ӷ���ֹǱ�ڵ�������ȷ���ض���Χ�ڵ��̰߳�ȫ��
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