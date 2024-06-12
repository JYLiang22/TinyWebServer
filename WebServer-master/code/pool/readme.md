# 线程池


## 功能概述

实现了一个简单的线程池，通过重用线程并最大限度地减少线程创建和销毁的开销来提高效率。<br>

以下是该线程池实现的主要组件和功能的细分：

1. **ThreadPool类**：该类封装了线程池的功能。

2. **私有结构体Pool**：该结构体保存了线程池的内部状态，包括用于同步的互斥锁（`mtx`）、用于线程协调的条件变量（`cond`）、布尔标志（`isClosed `）指示池是否关闭，以及线程要执行的任务队列（`tasks`）。

3. **构造函数`explicit ThreadPool(size_t threadCount)`**：用指定数量的线程（`threadCount`）初始化线程池。如果未提供计数，则默认为 8。在构造函数中，它创建指定数量的线程并为每个线程分配一个 lambda 函数。lambda 函数执行任务执行循环：它锁定互斥体，检查队列中的任务，如果可用则执行它们，如果队列为空且池未关闭则等待。线程是分离的，这意味着它们将继续独立运行。

4. **析构函数`~ThreadPool()`**：确保线程池对象被销毁时正确清理资源。它将“isClosed”标志设置为 true，使用“notify_all()”通知所有等待线程，并释放互斥体。

5. **`addTask`方法模板**：向线程池添加一个新任务。它将提供的任务（可以是任何可调用对象）转发到任务队列，使用“notify_one()”通知一个等待线程，并释放互斥体。

总体而言，该线程池允许以托管方式提交任务以供多个线程同时执行，从而通过避免为每个任务创建线程的开销来提高性能。


## 前置知识


### C++语言特性

1. 右值引用、移动语义、完美转发
   - [C++11的右值引用、移动语义（std::move）和完美转发（std::forward）详解](https://blog.csdn.net/weixin_43798887/article/details/117091176?ops_request_misc=%7B%22request_id%22:%22171331790616800197077232%22,%22scm%22:%2220140713.130102334.pc_all.%22%7D&request_id=171331790616800197077232&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-3-117091176-null-null.142%5Ev100%5Econtrol&utm_term=std::forward&spm=1018.2226.3001.4187)
   - [C++11中std::move和std::forward到底干了啥](https://blog.csdn.net/albertsh/article/details/118886009?ops_request_misc=%7B%22request_id%22:%22171331790616800197077232%22,%22scm%22:%2220140713.130102334.pc_all.%22%7D&request_id=171331790616800197077232&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-2-118886009-null-null.142%5Ev100%5Econtrol&utm_term=std::forward&spm=1018.2226.3001.4187)
   - [C++的std::move与std::forward原理大白话总结](https://blog.csdn.net/newchenxf/article/details/117995131?ops_request_misc=%7B%22request_id%22:%22171331790616800197077232%22,%22scm%22:%2220140713.130102334.pc_all.%22%7D&request_id=171331790616800197077232&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-4-117995131-null-null.142%5Ev100%5Econtrol&utm_term=std::forward&spm=1018.2226.3001.4187)
2. 类型强制转换
   - [深入理解C++中五种强制类型转换的使用场景](https://blog.csdn.net/weixin_43798887/article/details/118424172)
3. 智能指针
   - [C++11的智能指针shared_ptr、weak_ptr源码解析](https://blog.csdn.net/weixin_43798887/article/details/116464334)
   - [C++的智能指针auto_ptr、unique_ptr源码解析](https://blog.csdn.net/weixin_43798887/article/details/118104317)
4. std::function和std::bind
   - [C++11的std::function和std::bind用法详解](https://blog.csdn.net/qq_38410730/article/details/103637778?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565173716800227492494%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=171565173716800227492494&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-103637778-null-null.142^v100^control&utm_term=std%3A%3Afunction&spm=1018.2226.3001.4187)
5. assert
   - [assert断言函数简介](https://blog.csdn.net/weixin_61561736/article/details/124886522?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565547816800213026389%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565547816800213026389&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-4-124886522-null-null.142^v100^control&utm_term=assert&spm=1018.2226.3001.4187)
6. explict
   - [C++ explicit关键字详解](https://blog.csdn.net/guoyunfei123/article/details/89003369?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565195216800215019777%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565195216800215019777&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-89003369-null-null.142^v100^control&utm_term=explicit&spm=1018.2226.3001.4187)


### C++多线程

1. [【C++入门到精通】互斥锁 (Mutex) C++11 [ C++入门 ]](https://blog.csdn.net/m0_75215937/article/details/135041309?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565531916800213071534%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565531916800213071534&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-2-135041309-null-null.142^v100^control&utm_term=mutex&spm=1018.2226.3001.4187)
2. [【C++入门到精通】condition_variable（条件变量）C++11 [ C++入门 ]](https://blog.csdn.net/m0_75215937/article/details/135074541?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565534916800227491852%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565534916800227491852&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-135074541-null-null.142^v100^control&utm_term=condition_variable&spm=1018.2226.3001.4187)
3. [【C++入门到精通】Lock_guard与Unique_lock C++11 [ C++入门 ]](https://blog.csdn.net/m0_75215937/article/details/135041872?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565552716800182786124%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565552716800182786124&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-135041872-null-null.142^v100^control&utm_term=lock_guard&spm=1018.2226.3001.4187)
4. [C++11 多线程（std::thread）详解](https://blog.csdn.net/sjc_0910/article/details/118861539?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565575316800185845874%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565575316800185845874&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-118861539-null-null.142^v100^control&utm_term=std%3A%3Athread&spm=1018.2226.3001.4187)
5. [C++11多线程 unique_lock详解](https://blog.csdn.net/u012507022/article/details/85909567?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565479316800186586537%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565479316800186586537&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-3-85909567-null-null.142^v100^control&utm_term=unique_lock&spm=1018.2226.3001.4187)


# 数据库连接池


## 功能概述

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


## 前置知识

1. RAII(Resource Acquisition Is Initialization)
   RAII 背后的核心思想是将资源（例如内存分配、文件句柄、数据库连接、锁等）的生命周期与对象的生命周期联系起来。资源在对象初始化期间获取，并在对象超出范围时自动释放（通常在其析构函数中）。这确保了即使出现异常或提前返回，资源也能可靠、自动地释放。
2. 单例模式
   - [我给面试官讲解了单例模式后，他对我竖起了大拇指！](https://blog.csdn.net/weixin_41949328/article/details/107296517?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171575389116800180696112%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=171575389116800180696112&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-107296517-null-null.142^v100^control&utm_term=%E5%8D%95%E4%BE%8B%E6%A8%A1%E5%BC%8F&spm=1018.2226.3001.4187)
3. sem
   - [线程同步机制封装类](https://mp.weixin.qq.com/s?__biz=MzAxNzU2MzcwMw==&mid=2649274278&idx=3&sn=5840ff698e3f963c7855d702e842ec47&chksm=83ffbefeb48837e86fed9754986bca6db364a6fe2e2923549a378e8e5dec6e3cf732cdb198e2&scene=0&xtrack=1#rd)
   - [信号量 Semaphore 及 C++ 11 实现](https://zhuanlan.zhihu.com/p/512969481)
4. C++操作MySQL
   - [C/C++ 使用 MySQL API 操作 数据库](https://juejin.cn/post/7141190286838857735)