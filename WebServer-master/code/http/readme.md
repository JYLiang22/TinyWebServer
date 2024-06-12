# 功能概述


## HttpConn

HttpConn 类负责以下职责：<br>

1. 使用给定的文件描述符和地址初始化连接。
2. 将数据从套接字读入缓冲区。
3. 将数据从缓冲区写入套接字。
4. 处理 HTTP 请求并生成适当的响应。
5. 管理连接状态并维护用户数量。

主要方法说明：<br>

- 构造函数 (HttpConn::HttpConn())：初始化对象，将文件描述符设置为 -1，并将连接标记为已关闭。
- 析构函数 (HttpConn::~HttpConn())：确保连接已正确关闭。
- init(int fd, const sockaddr_in &addr)：使用给定的文件描述符和地址初始化连接，重置缓冲区，并增加用户计数。
- read(int *saveErrno)：将套接字中的数据读入读取缓冲区。如果 isET 为真，则使用边沿触发模式。
- write(int *saveErrno)：使用分散-聚集 I/O 将写入缓冲区和响应文件中的数据写入套接字。处理部分写入并相应地调整 iovec 结构。
- Close()：关闭连接，减少用户计数，并记录事件。
- process()：通过解析读取缓冲区、初始化响应和准备 iovec 结构进行写入来处理 HTTP 请求。
- toWriteBytes()：返回要从 iovec 结构写入的总字节数。
- isKeepAlive() const：根据请求检查是否应保持连接。

静态成员：<br>

- isET：指示连接是否处于边缘触发模式。
- srcDir：用于提供静态文件的目录路径。
- userCnt：活动用户数量的原子计数器。

此代码旨在有效处理多个 HTTP 连接，利用缓冲区管理、分散-聚集 I/O 和连接生命周期管理来确保正确处理客户端请求和服务器响应。


# 前置知识和框架可参考

1. [最新版Web服务器项目详解 - 04 http连接处理（上）](https://mp.weixin.qq.com/s/BfnNl-3jc_x5WPrWEJGdzQ)
2. [最新版Web服务器项目详解 - 05 http连接处理（中）](https://mp.weixin.qq.com/s/wAQHU-QZiRt1VACMZZjNlw)
3. [最新版Web服务器项目详解 - 06 http连接处理（下）](https://mp.weixin.qq.com/s/451xNaSFHxcxfKlPBV3OCg)