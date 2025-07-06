#pragma once

#include "Order.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <optional>

namespace engine {

/**
 * @brief Thread-safe queue for orders
 * 
 * Implements a producer-consumer pattern for order processing
 */
class OrderQueue {
public:
    OrderQueue() = default;
    
    /**
     * @brief Add an order to the queue
     * 
     * @param order The order to enqueue
     */
    void enqueue(std::shared_ptr<Order> order) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(order);
        }
        cv_.notify_one();
    }
    
    /**
     * @brief Try to get an order from the queue without blocking
     * 
     * @return std::optional<std::shared_ptr<Order>> The order or empty if queue is empty
     */
    std::optional<std::shared_ptr<Order>> tryDequeue() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return std::nullopt;
        }
        
        auto order = queue_.front();
        queue_.pop();
        return order;
    }
    
    /**
     * @brief Get an order from the queue, blocking if empty
     * 
     * @return std::shared_ptr<Order> The next order in the queue
     */
    std::shared_ptr<Order> dequeue() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty() || shutdown_; });
        
        if (shutdown_ && queue_.empty()) {
            return nullptr;  // Return nullptr when shutting down
        }
        
        auto order = queue_.front();
        queue_.pop();
        return order;
    }
    
    /**
     * @brief Check if the queue is empty
     * 
     * @return true if queue is empty, false otherwise
     */
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
    /**
     * @brief Get the current size of the queue
     * 
     * @return size_t Current queue size
     */
    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
    
    /**
     * @brief Signal shutdown to wake up any waiting consumers
     */
    void shutdown() {
        std::lock_guard<std::mutex> lock(mutex_);
        shutdown_ = true;
        cv_.notify_all();
    }
    
private:
    std::queue<std::shared_ptr<Order>> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool shutdown_ = false;
};

} // namespace engine
