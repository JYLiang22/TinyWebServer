# 功能概述

实现了一个简单的线程池，通过重用线程并最大限度地减少线程创建和销毁的开销来提高效率。<br>

以下是该线程池实现的主要组件和功能的细分：

1. **ThreadPool类**：该类封装了线程池的功能。

2. **私有结构体Pool**：该结构体保存了线程池的内部状态，包括用于同步的互斥锁（`mtx`）、用于线程协调的条件变量（`cond`）、布尔标志（`isClosed `）指示池是否关闭，以及线程要执行的任务队列（`tasks`）。

3. **构造函数`explicit ThreadPool(size_t threadCount)`**：用指定数量的线程（`threadCount`）初始化线程池。如果未提供计数，则默认为 8。在构造函数中，它创建指定数量的线程并为每个线程分配一个 lambda 函数。lambda 函数执行任务执行循环：它锁定互斥体，检查队列中的任务，如果可用则执行它们，如果队列为空且池未关闭则等待。线程是分离的，这意味着它们将继续独立运行。

4. **析构函数`~ThreadPool()`**：确保线程池对象被销毁时正确清理资源。它将“isClosed”标志设置为 true，使用“notify_all()”通知所有等待线程，并释放互斥体。

5. **`addTask`方法模板**：向线程池添加一个新任务。它将提供的任务（可以是任何可调用对象）转发到任务队列，使用“notify_one()”通知一个等待线程，并释放互斥体。

总体而言，该线程池允许以托管方式提交任务以供多个线程同时执行，从而通过避免为每个任务创建线程的开销来提高性能。


# 前置知识


## C++语言特性

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


## C++多线程

1. [【C++入门到精通】互斥锁 (Mutex) C++11 [ C++入门 ]](https://blog.csdn.net/m0_75215937/article/details/135041309?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565531916800213071534%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565531916800213071534&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-2-135041309-null-null.142^v100^control&utm_term=mutex&spm=1018.2226.3001.4187)
2. [【C++入门到精通】condition_variable（条件变量）C++11 [ C++入门 ]](https://blog.csdn.net/m0_75215937/article/details/135074541?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565534916800227491852%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565534916800227491852&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-135074541-null-null.142^v100^control&utm_term=condition_variable&spm=1018.2226.3001.4187)
3. [【C++入门到精通】Lock_guard与Unique_lock C++11 [ C++入门 ]](https://blog.csdn.net/m0_75215937/article/details/135041872?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565552716800182786124%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565552716800182786124&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-135041872-null-null.142^v100^control&utm_term=lock_guard&spm=1018.2226.3001.4187)
4. [C++11 多线程（std::thread）详解](https://blog.csdn.net/sjc_0910/article/details/118861539?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565575316800185845874%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565575316800185845874&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-118861539-null-null.142^v100^control&utm_term=std%3A%3Athread&spm=1018.2226.3001.4187)
5. [C++11多线程 unique_lock详解](https://blog.csdn.net/u012507022/article/details/85909567?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565479316800186586537%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565479316800186586537&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-3-85909567-null-null.142^v100^control&utm_term=unique_lock&spm=1018.2226.3001.4187)