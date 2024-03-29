#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "ThreadPool.h"
using namespace std;

int main()
{
    {
        ThreadPool pool;

        vector<future<int>> results;
        for (int i = 0; i < 8; ++i)
        {
            results.emplace_back(
                pool.enqueue([i]{
                    cout << "hello " << i << endl;
                    this_thread::sleep_for(chrono::seconds(1));
                    cout << "world " << i << endl;
                    return i*i; 
                })
            );
        }
        for (future<int> &result : results)
            cout << result.get() << ' ';
    }

    return 0;
}
