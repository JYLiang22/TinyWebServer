# 功能概述

**BlockQueue Class**<br>
BlockQueue类是一个线程安全的阻塞队列，可以存储任何类型的元素。它使用双端队列来存储元素，使用互斥量来确保线程安全，并使用条件变量来管理生产者和消费者之间的同步。<br>

主要功能：
- 构造函数和析构函数：以最大容量初始化队列并清理资源。
- clear()：清空队列。
- empty()：检查队列是否为空。
- full()：检查队列是否已满。
- close()：关闭队列并通知所有等待线程。
- size()：返回队列的当前大小。
- capacity()：返回队列的最大容量。
- front() & back()：访问队列的前部和后部元素。
- Push_back() & Push_front()：将元素添加到队列的后面或前面。
- pop()：从队列前面删除一个元素，可以选择超时。
- flush()：通知一个正在等待的消费者。


**Log Class**<br>
Log类通过使用 BlockQueue 支持异步日志记录来管理日志记录。它根据日期和行数处理日志文件轮换，并允许设置不同的日志级别。<br>

主要功能：
- init()：初始化日志系统。
- write()：写入日志条目，支持可变参数。
- flush()：将日志缓冲区刷新到文件。
- getLevel() & setLevel()：获取和设置当前日志级别。
- isOpen()：检查日志系统是否打开。
- appendLogLevelTitle()：向日志条目添加日志级别前缀。
- asyncWrite()：将日志条目从队列写入文件。
- instance()：单例实例访问器。
- flushLogThread()：用于异步日志记录的线程函数。


# 前置知识

1. 设计思路可以参考： **Linux多线程服务端编程：使用muduoC++网络库(陈硕)** 中的第五章，把书本看过就可以理解代码了。
2. #define中 "\\" 的作用。
3. '# ## __VA_ARGS__ ##__VA_ARGS__'<br>
   1. [# ## __VA_ARGS__ ##__VA_ARGS__的使用说明](https://blog.csdn.net/hanxv_1987/article/details/106555870?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171679283516800178567352%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171679283516800178567352&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v31_ecpm-1-106555870-null-null.142^v100^control&utm_term=%23define%E9%87%8C%E9%9D%A2%E7%9A%84%23%23__VA__ARGS__&spm=1018.2226.3001.4187)
   2. [#、##、__VA_ARGS__的使用](https://blog.csdn.net/auccy/article/details/88833659?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171679284516800225510710%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171679284516800225510710&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-88833659-null-null.142^v100^control&utm_term=%23define%E9%87%8C%E9%9D%A2%E7%9A%84%23%23__VA__ARGS__&spm=1018.2226.3001.4187)
