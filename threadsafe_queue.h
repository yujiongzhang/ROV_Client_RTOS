#ifndef THREADSAFE_QUEUE_H
#define THREADSAFE_QUEUE_H

#pragma once
//线程安全队列的类模板
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>                     //为了使用std::shared_ptr
#include <list>

using namespace std;

template<typename T> //利用template关键字实现泛型
class threadsafe_queue
{
private:
    mutable std::mutex mut;          //互斥量必须是可变的
                                     /*std::queue<T> data_queue;*/    //list实现就不会满了吧
    queue<T, list<T> > data_queue;

    std::condition_variable data_cond;
public:
    threadsafe_queue()
    {}
    threadsafe_queue(threadsafe_queue const& other)
    {
        std::lock_guard<std::mutex> lk(other.mut);
        data_queue = other.data_queue;

    }

    void push(T new_value)
    {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_value);
        data_cond.notify_one();
    }

    void wait_and_pop(T& value)        //将会等待有值可检索的时候才返回
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] {return !data_queue.empty(); });//可见条件变量实现阻塞队列
        value = data_queue.front();
        data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] {return !data_queue.empty(); });
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool try_pop(T& value)     //尝试从队列中弹出数据，总会直接返回(当有失败时)，即使没有值可检索
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty)
            return false;
        value = data_queue.front();
        data_queue.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};

#endif // THREADSAFE_QUEUE_H
