// ThreadPool.hpp - High-performance packet processing thread pools
#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <iostream>

namespace PacketAnalyzer2026::Performance {

class ThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_{false};
    std::string name_;
    std::atomic<size_t> activeTasks_{0};
    std::atomic<size_t> totalTasks_{0};

public:
    ThreadPool(size_t numThreads, const std::string& name) : name_(name) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back([this, i] {
                for (;;) {
                    std::function<void()> task;
                    
                    {
                        std::unique_lock<std::mutex> lock(queueMutex_);
                        condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                        
                        if (stop_ && tasks_.empty()) return;
                        
                        task = std::move(tasks_.front());
                        tasks_.pop();
                        activeTasks_++;
                    }
                    
                    try {
                        task();
                    } catch (const std::exception& e) {
                        std::cout << "❌ Task failed in " << name_ << " pool: " << e.what() << std::endl;
                    }
                    
                    activeTasks_--;
                }
            });
        }
        
        std::cout << "🧵 Thread Pool '" << name_ << "' initialized with " << numThreads << " threads" << std::endl;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            stop_ = true;
        }
        
        condition_.notify_all();
        
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        
        std::cout << "🧵 Thread Pool '" << name_ << "' destroyed" << std::endl;
    }

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> result = task->get_future();
        
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            
            if (stop_) {
                throw std::runtime_error("Cannot enqueue on stopped ThreadPool");
            }
            
            tasks_.emplace([task]() { (*task)(); });
            totalTasks_++;
        }
        
        condition_.notify_one();
        return result;
    }

    size_t queueSize() const {
        std::unique_lock<std::mutex> lock(queueMutex_);
        return tasks_.size();
    }

    size_t activeTaskCount() const {
        return activeTasks_.load();
    }

    size_t totalTaskCount() const {
        return totalTasks_.load();
    }

    std::string getName() const {
        return name_;
    }

    double getUtilizationPercent() const {
        return (static_cast<double>(activeTasks_) / workers_.size()) * 100.0;
    }
};

struct ThreadPoolMetrics {
    struct PoolMetrics {
        size_t queueSize;
        size_t activeTasks;
        size_t totalTasks;
        double utilizationPercent;
    };
    
    PoolMetrics capture;
    PoolMetrics parsing;
    PoolMetrics storage;
    PoolMetrics ui;
};

class PacketProcessingThreadPool {
private:
    ThreadPool capturePool_;
    ThreadPool parsingPool_;
    ThreadPool storagePool_;
    ThreadPool uiPool_;

public:
    PacketProcessingThreadPool() 
        : capturePool_(2, "Capture")      // High priority, small pool
        , parsingPool_(4, "Parsing")      // Main processing
        , storagePool_(2, "Storage")      // I/O operations
        , uiPool_(1, "UI")                // UI updates
    {
        std::cout << "🚀 Packet Processing Thread Pool System initialized" << std::endl;
    }

    ThreadPool& getCapturePool() { return capturePool_; }
    ThreadPool& getParsingPool() { return parsingPool_; }
    ThreadPool& getStoragePool() { return storagePool_; }
    ThreadPool& getUIPool() { return uiPool_; }

    ThreadPoolMetrics getSystemMetrics() const {
        ThreadPoolMetrics metrics;
        
        metrics.capture = {
            capturePool_.queueSize(),
            capturePool_.activeTaskCount(),
            capturePool_.totalTaskCount(),
            capturePool_.getUtilizationPercent()
        };
        
        metrics.parsing = {
            parsingPool_.queueSize(),
            parsingPool_.activeTaskCount(),
            parsingPool_.totalTaskCount(),
            parsingPool_.getUtilizationPercent()
        };
        
        metrics.storage = {
            storagePool_.queueSize(),
            storagePool_.activeTaskCount(),
            storagePool_.totalTaskCount(),
            storagePool_.getUtilizationPercent()
        };
        
        metrics.ui = {
            uiPool_.queueSize(),
            uiPool_.activeTaskCount(),
            uiPool_.totalTaskCount(),
            uiPool_.getUtilizationPercent()
        };
        
        return metrics;
    }

    void printStatus() const {
        auto metrics = getSystemMetrics();
        std::cout << "📊 Thread Pool Status:" << std::endl;
        std::cout << "   🔧 Capture: " << metrics.capture.utilizationPercent << "% utilization" << std::endl;
        std::cout << "   ⚙️ Parsing: " << metrics.parsing.utilizationPercent << "% utilization" << std::endl;
        std::cout << "   💾 Storage: " << metrics.storage.utilizationPercent << "% utilization" << std::endl;
        std::cout << "   🖥️ UI: " << metrics.ui.utilizationPercent << "% utilization" << std::endl;
    }
};

} // namespace PacketAnalyzer2026::Performance