/*
* @author   JYLiang
* @date     2024.5.15
*/

#ifndef _SQLCONNRAII_H_
#define _SQLCONNRAII_H_

#include"./sqlconnpool.h"
#include<assert.h>

class SqlConnRAII{
private:
    MYSQL *sql;
    SqlConnPool *connpool;

public:
    SqlConnRAII(MYSQL **sql, SqlConnPool *connpool){
        assert(connpool);
        *sql = connpool->GetConn();
        this->sql = *sql;
        this->connpool = connpool;
    }

    ~SqlConnRAII(){
        if(this->sql){
            this->connpool->FreeConn(this->sql);
        }
    }
};

#endif
