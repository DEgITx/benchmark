#include <cmath>
#include <thread>
#include <vector>
#include <atomic>
#include "degxlog.h"

const float pi = std::acos(-1);

namespace benchmark
{

uint64_t BENCHMARK(void (*func)(void), int sec)
{
    uint64_t counter = 0;
    auto start_timestamp = DXGetSystemNanoTime();
    while(DXGetSystemNanoTime() < start_timestamp + (((uint64_t)sec) * 1000 * 1000 * 1000))
    {
        func();
        counter++;
    }
    DX_DEBUG("benchmark", "score=%llu", counter);
    return counter;
}

uint64_t BENCHMARK_ASYNC(void (*func)(void), int sec)
{
    std::atomic<uint64_t> counter = 0;
    auto start_timestamp = DXGetSystemNanoTime();
    const int processor_count = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (int i = 0; i < processor_count; i++)
    {
        std::thread thread([func, &counter, start_timestamp, sec]() 
        {
            while(DXGetSystemNanoTime() < start_timestamp + (((uint64_t)sec) * 1000 * 1000 * 1000))
            {
                func();
                counter++;
            }
        });
        threads.push_back(std::move(thread));
    }
    for(auto& thread: threads)
        thread.join();
    DX_DEBUG("benchmark", "async score=%llu", counter.load());
    return counter;
}

float sin(float x)
{
  float res=0, pow=x, fact=1;
  for(int i=0; i<10; ++i)
  {
    res+=pow/fact;
    pow*=-1*x*x;
    fact*=(2*(i+1))*(2*(i+1)+1);
  }

  return res;
}

void test_sin()
{
    for(int i = 0; i < 4; i++)
        sin((pi / 2) * i);
}

void start()
{
    auto v1 = BENCHMARK(test_sin, 5);
    auto v2 = BENCHMARK_ASYNC(test_sin, 5);
    DX_DEBUG("start", "diff = %f", (double)v2 / v1)
}

}

int main()
{
    benchmark::start();
    return 0;
}