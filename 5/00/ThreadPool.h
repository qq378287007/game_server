#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
using namespace std;

class ThreadPool
{
    vector<thread> workers;
    queue<function<void()>> tasks;

    mutex queue_mutex;
    condition_variable condition;
    bool stop{false};

public:
    ThreadPool(unsigned thread_num = thread::hardware_concurrency())
    {
        for (unsigned i = 0; i < thread_num; ++i){
            workers.emplace_back([this]{
                while(true){
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(queue_mutex);
                        while(!stop && tasks.empty())
                            condition.wait(lock);
                        //condition.wait(lock, [this]{ return stop || !tasks.empty(); });
                        if (stop && tasks.empty())//执行完所有任务才可能退出
                            return;
                        task = move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }
    ~ThreadPool()
    {
        {
            unique_lock<mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (thread &worker : workers)
            worker.join();
    }

    template <class F, class... Args>
    auto enqueue(F &&f, Args &&...args) -> future<typename result_of<F(Args...)>::type>
    {
        using return_type = typename result_of<F(Args...)>::type;
        auto task = make_shared<packaged_task<return_type()>>(bind(forward<F>(f), forward<Args>(args)...));
        future<return_type> res = task->get_future();
        {
            unique_lock<mutex> lock(queue_mutex);
            if (stop)
                throw runtime_error("enqueue on stopped ThreadPool");
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }
};
