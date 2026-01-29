// DeadLetterQueue.hpp - Handle failed packets gracefully
#pragma once

#include <deque>
#include <mutex>
#include <chrono>
#include <functional>
#include <string>
#include <iostream>

namespace PacketAnalyzer2026::Resilience {

template<typename T>
class DeadLetterQueue {
private:
    struct FailedItem {
        T item;
        std::string error;
        std::chrono::system_clock::time_point failureTime;
        std::string failedStage;
        int retryCount;
    };

    std::deque<FailedItem> queue_;
    const size_t maxSize_;
    mutable std::mutex mutex_;

public:
    explicit DeadLetterQueue(size_t maxSize = 1000) : maxSize_(maxSize) {
        std::cout << "📮 Dead Letter Queue initialized (max size: " << maxSize_ << ")" << std::endl;
    }

    void storeFailure(T item, const std::exception& e, const std::string& stage) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (queue_.size() >= maxSize_) {
            // Remove oldest failures
            queue_.pop_front();
        }

        queue_.push_back({
            std::move(item),
            e.what(),
            std::chrono::system_clock::now(),
            stage,
            0
        });

        std::cout << "💀 Item stored in DLQ (stage: " << stage << "): " << e.what() << std::endl;
        
        // Alert if too many failures
        if (queue_.size() > 100) {
            std::cout << "🚨 High failure rate in DLQ: " << queue_.size() << " items (stage: " << stage << ")" << std::endl;
        }
    }

    void retryFailures(std::function<void(const T&)> retryFunction) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto tempQueue = std::move(queue_);
        queue_.clear();

        for (auto& failed : tempQueue) {
            try {
                retryFunction(failed.item);
                std::cout << "✅ Successfully reprocessed item from DLQ (stage: " << failed.failedStage << ")" << std::endl;
            } catch (const std::exception& e) {
                // Put it back if still failing, but limit retries
                failed.retryCount++;
                if (failed.retryCount < 3) {
                    queue_.push_back(std::move(failed));
                    std::cout << "🔄 Item returned to DLQ (retry " << failed.retryCount << "/3)" << std::endl;
                } else {
                    std::cout << "❌ Item permanently failed after 3 retries - discarded" << std::endl;
                }
            }
        }
    }

    void analyzeFailurePatterns() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::map<std::string, int> failuresByStage;
        std::map<std::string, int> failuresByError;

        for (const auto& failed : queue_) {
            failuresByStage[failed.failedStage]++;
            failuresByError[failed.error]++;
        }

        // Report patterns
        std::cout << "📊 DLQ Failure Analysis:" << std::endl;
        for (const auto& [stage, count] : failuresByStage) {
            if (count > 10) {
                std::cout << "⚠️  High failures in stage '" << stage << "': " << count << " items" << std::endl;
            }
        }
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.clear();
        std::cout << "🗑️ Dead Letter Queue cleared" << std::endl;
    }
};

} // namespace PacketAnalyzer2026::Resilience