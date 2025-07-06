#pragma once

#include <atomic>
#include <cstdint>

namespace engine {
namespace util {

/**
 * @brief Thread-safe order ID generator
 */
class OrderIdGenerator {
public:
    /**
     * @brief Get the singleton instance of the ID generator
     */
    static OrderIdGenerator& getInstance() {
        static OrderIdGenerator instance;
        return instance;
    }
    
    /**
     * @brief Generate a new unique order ID
     * 
     * @return uint64_t A globally unique order ID
     */
    std::uint64_t getNextId() {
        return nextId_++;
    }
    
    // Delete copy and move constructors/operators
    OrderIdGenerator(const OrderIdGenerator&) = delete;
    OrderIdGenerator& operator=(const OrderIdGenerator&) = delete;
    OrderIdGenerator(OrderIdGenerator&&) = delete;
    OrderIdGenerator& operator=(OrderIdGenerator&&) = delete;

private:
    OrderIdGenerator() : nextId_(1) {}  // Start IDs from 1
    
    std::atomic<std::uint64_t> nextId_;
};

} // namespace util
} // namespace engine
