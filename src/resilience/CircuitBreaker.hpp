// CircuitBreaker.hpp - Prevent cascading failures in packet processing
#pragma once

#include <chrono>
#include <atomic>
#include <functional>
#include <optional>
#include <string>
#include <iostream>

namespace PacketAnalyzer2026::Resilience {

class CircuitBreaker {
public:
    enum class State {
        CLOSED,    // Normal operation
        OPEN,      // Failing fast
        HALF_OPEN  // Testing if service recovered
    };

private:
    std::string name_;
    State currentState_;
    std::atomic<int> failureCount_{0};
    const int failureThreshold_;
    std::chrono::steady_clock::time_point lastFailureTime_;
    const std::chrono::seconds resetTimeout_;
    mutable std::mutex mutex_;

public:
    CircuitBreaker(const std::string& name, 
                   int failureThreshold = 5, 
                   std::chrono::seconds resetTimeout = std::chrono::seconds(30))
        : name_(name)
        , currentState_(State::CLOSED)
        , failureThreshold_(failureThreshold)
        , resetTimeout_(resetTimeout)
    {
        std::cout << "🔧 Circuit Breaker '" << name_ << "' initialized (threshold: " 
                  << failureThreshold_ << ", timeout: " << resetTimeout_.count() << "s)" << std::endl;
    }

    template<typename Func>
    std::optional<decltype(std::declval<Func>()())> execute(Func operation) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (currentState_ == State::OPEN) {
            if (std::chrono::steady_clock::now() - lastFailureTime_ > resetTimeout_) {
                currentState_ = State::HALF_OPEN;
                std::cout << "🔄 Circuit Breaker '" << name_ << "' transitioning to HALF_OPEN" << std::endl;
            } else {
                std::cout << "⚡ Circuit Breaker '" << name_ << "' OPEN - operation blocked" << std::endl;
                return std::nullopt;  // Fail fast
            }
        }

        try {
            auto result = operation();
            
            // Success - reset if in half-open
            if (currentState_ == State::HALF_OPEN) {
                currentState_ = State::CLOSED;
                failureCount_ = 0;
                std::cout << "✅ Circuit Breaker '" << name_ << "' recovered - state: CLOSED" << std::endl;
            }
            
            return result;
            
        } catch (const std::exception& e) {
            failureCount_++;
            lastFailureTime_ = std::chrono::steady_clock::now();
            
            if (failureCount_ >= failureThreshold_) {
                currentState_ = State::OPEN;
                std::cout << "🚨 Circuit Breaker '" << name_ << "' OPENED after " 
                          << failureCount_ << " failures: " << e.what() << std::endl;
            } else if (currentState_ == State::HALF_OPEN) {
                currentState_ = State::OPEN;
                std::cout << "🚨 Circuit Breaker '" << name_ << "' back to OPEN from HALF_OPEN" << std::endl;
            }
            
            throw;  // Re-throw for caller to handle
        }
    }

    State getState() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentState_;
    }

    int getFailureCount() const {
        return failureCount_.load();
    }

    std::string getName() const {
        return name_;
    }

    bool isOpen() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentState_ == State::OPEN;
    }
};

} // namespace PacketAnalyzer2026::Resilience