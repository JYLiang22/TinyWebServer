# 功能概述

Buffer 类提供了一种管理动态数据缓冲区的有效方法，对于网络编程或文件 I/O 操作特别有用。该类支持：

1. 读取和写入文件描述符。
2. 动态管理内部缓冲区空间。
3. 以字符串形式检索数据。
4. 以各种形式附加数据（字符串、原始数据、另一个缓冲区）。
5. readv 和 write 系统调用的使用可确保缓冲区操作针对性能进行优化，利用分散/聚集 I/O 进行高效的数据处理。


# 前置知识

1. 实现原理可以参考：**Linux多线程服务端编程：使用muduoC++网络库(陈硕)**中的7.4节，把书本看过就可以理解代码了。
2. [struct iovec 结构体定义与使用](https://blog.csdn.net/baidu_15952103/article/details/109888362?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171627331016800227427263%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171627331016800227427263&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-2-109888362-null-null.142^v100^control&utm_term=iovec&spm=1018.2226.3001.4187)
