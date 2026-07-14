#include "ThreadPool.h"

ThreadPool::ThreadPool(unsigned int numThreads) {
	for (unsigned int i = 0; i < numThreads; ++i) {
		m_workers.emplace_back(&ThreadPool::WorkerLoop, this);
	}
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_stop = true;
    }
    m_cv.notify_all();
    for (auto& t : m_workers) t.join();
}

void ThreadPool::Enqueue(std::function<void()> job) {
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_jobs.push(std::move(job));
        m_pendingJobs++;
    }
    m_cv.notify_one();
}

void ThreadPool::WorkerLoop() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_cv.wait(lock, [this] { return m_stop || !m_jobs.empty(); });
            if (m_stop && m_jobs.empty()) return;
            job = std::move(m_jobs.front());
            m_jobs.pop();
        }
        job();
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_pendingJobs--;
            if (m_pendingJobs == 0) m_doneCv.notify_all();
        }
    }
}

void ThreadPool::WaitAll() {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_doneCv.wait(lock, [this] { return m_pendingJobs == 0; });
}