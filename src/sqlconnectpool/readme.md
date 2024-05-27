# 功能概述

此代码实现了资源获取即初始化 (RAII) 模式，用于使用连接池管理 SQL 连接。 我们来分解一下它的功能：

1. **SqlConnPool类**：
    - 管理 MySQL 连接池。
    - 提供初始化池、获取连接、释放连接、关闭池的方法。
    - 它实现了一种单例模式（`Instance()`），以确保整个程序中只有一个连接池实例。

2. **SqlConnPool::Instance()**:
    - 提供静态方法来访问连接池的单例实例。
    - 确保仅创建一个连接池实例。

3. **SqlConnPool::Init()**:
    - 通过创建指定数量的 MySQL 连接（`connSize`）并将它们添加到池中来初始化连接池。
    - 每个连接都是使用“mysql_init()”和“mysql_real_connect()”函数创建的。

4. **SqlConnPool::GetConn()**:
    - 从池中检索 MySQL 连接。
    - 如果池为空，它将等待直到连接可用。
    - 使用信号量（`sem_id`）来控制对池的访问，确保连接数不超过允许的最大连接数（`Max_Conn`）。

5. **SqlConnPool::FreeConn()**:
    - 将 MySQL 连接释放回池。
    - 增加信号量计数以指示连接可用。

6. **SqlConnPool::ClosePool()**:
    - 通过释放所有连接并调用`mysql_library_end()`来关闭连接池以清理MySQL资源。

7. **SqlConnPool::GetFreeConn()**:
    - 返回池中当前可用的空闲连接数。

8. **SqlConnPool析构函数**：
    - 调用“ClosePool()”以确保池对象被销毁时进行正确的清理。

9. **SqlConnRAII 类**：
    - 实现用于管理 SQL 连接的 RAII 模式。
    - 在其构造函数中从连接池获取连接并在其析构函数中释放它。
    - 确保当“SqlConnRAII”对象超出范围时自动释放连接。

总的来说，这段代码提供了一种安全有效的方法来使用连接池来管理SQL连接，并确保正确获取和释放连接，避免资源泄漏并提高性能。


# 前置知识

1. RAII(Resource Acquisition Is Initialization)
   RAII 背后的核心思想是将资源（例如内存分配、文件句柄、数据库连接、锁等）的生命周期与对象的生命周期联系起来。资源在对象初始化期间获取，并在对象超出范围时自动释放（通常在其析构函数中）。这确保了即使出现异常或提前返回，资源也能可靠、自动地释放。
2. 单例模式
   - [我给面试官讲解了单例模式后，他对我竖起了大拇指！](https://blog.csdn.net/weixin_41949328/article/details/107296517?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171575389116800180696112%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=171575389116800180696112&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-107296517-null-null.142^v100^control&utm_term=%E5%8D%95%E4%BE%8B%E6%A8%A1%E5%BC%8F&spm=1018.2226.3001.4187)
3. sem
   - [线程同步机制封装类](https://mp.weixin.qq.com/s?__biz=MzAxNzU2MzcwMw==&mid=2649274278&idx=3&sn=5840ff698e3f963c7855d702e842ec47&chksm=83ffbefeb48837e86fed9754986bca6db364a6fe2e2923549a378e8e5dec6e3cf732cdb198e2&scene=0&xtrack=1#rd)
   - [信号量 Semaphore 及 C++ 11 实现](https://zhuanlan.zhihu.com/p/512969481)
4. C++操作MySQL
   - [C/C++ 使用 MySQL API 操作 数据库](https://juejin.cn/post/7141190286838857735)