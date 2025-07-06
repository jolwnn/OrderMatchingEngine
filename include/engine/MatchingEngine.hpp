#pragma once

#include "OrderBook.hpp"
#include "OrderQueue.hpp"
#include <unordered_map>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <functional>

namespace engine {

/**
 * @brief Statistics for the matching engine
 */
struct MatchingEngineStats {
    std::atomic<uint64_t> totalOrdersProcessed{0};
    std::atomic<uint64_t> totalTradesExecuted{0};
    std::atomic<uint64_t> totalQuantityTraded{0};
};

/**
 * @brief Class representing a matching engine
 * 
 * Thread-safe implementation that supports concurrent order submission.
 */
class MatchingEngine {
public:
    /**
     * @brief Construct a new Matching Engine
     * 
     * @param numWorkers Number of worker threads to process orders (default: 1)
     */
    explicit MatchingEngine(size_t numWorkers = 1);
    
    /**
     * @brief Destructor - ensures clean shutdown of worker threads
     */
    ~MatchingEngine();
    
    /**
     * @brief Start the matching engine
     */
    void start();
    
    /**
     * @brief Stop the matching engine and join all worker threads
     */
    void stop();
    
    /**
     * @brief Submit an order for processing
     * 
     * Thread-safe method that enqueues an order for processing.
     * Does not block unless the queue is very large.
     * 
     * @param order The order to submit
     */
    void submitOrder(std::shared_ptr<Order> order);
    
    /**
     * @brief Process an order immediately (bypassing the queue)
     * 
     * This is a thread-safe method that directly processes an order.
     * 
     * @param order The order to process
     * @return std::vector<Trade> Resulting trades
     */
    std::vector<Trade> processOrderSync(std::shared_ptr<Order> order);
    
    /**
     * @brief Get the order book
     * 
     * Thread-safe method.
     * 
     * @return const OrderBook& Reference to the order book
     */
    const OrderBook& getOrderBook() const;
    
    /**
     * @brief Get the current statistics
     * 
     * @return const MatchingEngineStats& Reference to current statistics
     */
    const MatchingEngineStats& getStats() const { return stats_; }
    
    /**
     * @brief Register a callback for trade notifications
     * 
     * @param callback The callback function to register
     */
    void registerTradeCallback(std::function<void(const Trade&)> callback) {
        tradeCallback_ = callback;
    }
    
private:
    OrderBook orderBook_;
    OrderQueue orderQueue_;
    std::vector<std::thread> workerThreads_;
    std::atomic<bool> running_{false};
    size_t numWorkers_;
    MatchingEngineStats stats_;
    std::function<void(const Trade&)> tradeCallback_;
    
    /**
     * @brief Worker thread function that processes orders from the queue
     */
    void workerFunction();
    
    /**
     * @brief Internal callback for trade notifications
     * 
     * @param trade The trade that occurred
     */
    void onTrade(const Trade& trade);
};

} // namespace engine
