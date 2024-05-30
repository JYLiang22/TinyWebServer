# 功能概述

1. HeapTimer 类：该类使用最小堆管理定时器集合，以确保高效访问最近到期的定时器。
2. TimerNode 结构：表示每个定时器，包含一个 id、一个到期时间戳和一个回调函数 cb。
3. 私有方法：

    - del(size_t index)：按索引从堆中删除定时器节点。
    - siftup(size_t index)：将节点在堆中向上移动以维护堆属性。
    - siftdown(size_t index, size_t n)：将节点在堆中向下移动以维护堆属性。
    - swapNode(size_t index, size_t j)：交换堆中的两个节点并更新它们在引用映射中的索引。

4. 公共方法：

    - HeapTimer()：为堆保留空间的构造函数。
    - ~HeapTimer()：清除堆的析构函数。
    - adjust(int id, int newExpires): 调整现有定时器的到期时间。
    - add(int id, int timeOut, const TimeoutCallBack &cb): 添加新定时器或更新现有定时器。
    - doWork(int id): 执行定时器的回调函数并将其从堆中移除。
    - clear(): 清除所有定时器。
    - tick(): 检查并执行已到期的定时器。
    - pop(): 从堆中移除顶部（最小）定时器节点。
    - getNextTick(): 返回下一个定时器到期的时间。

此实现确保添加、调整和移除定时器是高效的，并利用最小堆的属性。 tick 方法确保及时处理已到期的定时器。


# 前置知识

1. 可参考： **Linux多线程服务端编程：使用muduoC++网络库(陈硕)** 中的7.8-7.10节，对比实现方法之间的差异。
