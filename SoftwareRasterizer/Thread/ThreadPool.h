#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(unsigned int numThreads);
    ~ThreadPool();

    void Enqueue(std::function<void()> job);
    void WaitAll(); // blocks until all currently enqueued jobs complete

private:
    void WorkerLoop();

    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_jobs;
    std::mutex m_queueMutex;
    std::condition_variable m_cv;
    std::condition_variable m_doneCv;
    bool m_stop = false;
    int m_pendingJobs = 0;
};