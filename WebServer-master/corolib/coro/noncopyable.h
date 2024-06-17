/*
 * @file noncopyable.h
 * @brief 不可拷贝对象封装
 * @date 2024.6.17
 * @cite https://github.com/zhongluqiang/sylar-from-scratch/blob/main/sylar/noncopyable.h
*/

#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

namespace coro{

/*
    * @brief 对象无法拷贝、赋值
*/
class Noncopyable{
public:
    /*
     * @brief 默认构造函数
    */
    Noncopyable() = default;

    /*
     * @brief 默认析构函数
    */
    ~Noncopyable() = default;

    /*
     * @brief 拷贝构造函数，禁用
    */
    Noncopyable(const Noncopyable&) = delete;

    /*
     * @brief 赋值函数，禁用
    */
    Noncopyable operator=(const Noncopyable&) = delete;
};

}

#endif
