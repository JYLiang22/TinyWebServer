# TinyWebServer

参考：
1. [https://github.com/qinguoyi/TinyWebServer](https://github.com/qinguoyi/TinyWebServer)
2. [https://github.com/markparticle/WebServer](https://github.com/markparticle/WebServer)
3. [https://github.com/zhongluqiang/sylar-from-scratch](https://github.com/zhongluqiang/sylar-from-scratch)


## 功能


## 基础知识

[1 Linux系统编程入门](<project/Linux高并发服务器开发/1 Linux系统编程入门/笔记.md>)<br> 
[2 Linux多进程开发](<project/Linux高并发服务器开发/2 Linux多进程开发/笔记.md>)<br>
[3 Linux多线程开发](<project/Linux高并发服务器开发/3 Linux多线程开发/笔记.md>)<br> 
[4 Linux网络编程](<project/Linux高并发服务器开发/4 Linux网络编程/笔记.md>)<br>


## 各模块介绍

[线程池&数据库连接池](WebServer-master/code/pool/readme.md)<br>
[缓冲区](WebServer-master/code/buffer/readme.md)<br>
[日志系统](WebServer-master/code/log/readme.md)<br>
[定时器](WebServer-master/code/timer/readme.md)<br>
[http连接请求响应](WebServer-master/code/http/readme.md)<br>


## 单元测试


## 压力测试


# CoroutineLibrary


## 协程库基础知识

开始之前，先了解一下什么是协程，可参考：<br>

1. [浅谈协程](https://jasonkayzk.github.io/2022/06/03/%E6%B5%85%E8%B0%88%E5%8D%8F%E7%A8%8B/)
2. [有栈协程与无栈协程](https://mthli.xyz/stackful-stackless/)
3. [浅谈有栈协程与无栈协程](https://zhuanlan.zhihu.com/p/347445164)
4. [渡劫 C++ 协程](https://www.bennyhuo.com/2022/03/09/cpp-coroutines-01-intro/)


## ucontext族函数

正式开始编写协程库前，需要了解Linux下的ucontext族函数，具体可参考:
1. [ucontext-人人都可以实现的简单协程库](https://developer.aliyun.com/article/52886)
2. [代码实现](WebServer-master/corolib)
