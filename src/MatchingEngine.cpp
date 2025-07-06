#include "engine/MatchingEngine.hpp"
#include <iostream>
#include <chrono>
#include <functional>

namespace engine {

MatchingEngine::MatchingEngine(size_t numWorkers)
    : numWorkers_(numWorkers) {
}

MatchingEngine::~MatchingEngine() {
    if (running_) {
        stop();
    }
}

void MatchingEngine::start() {
    if (running_) return;
    
    running_ = true;
    
    // Create worker threads
    for (size_t i = 0; i < numWorkers_; ++i) {
        workerThreads_.emplace_back(&MatchingEngine::workerFunction, this);
    }
    
    std::cout << "Matching engine started with " << numWorkers_ << " worker threads." << std::endl;
}

void MatchingEngine::stop() {
    if (!running_) return;
    
    running_ = false;
    
    // Signal all workers to stop
    orderQueue_.shutdown();
    
    // Join all worker threads
    for (auto& thread : workerThreads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    workerThreads_.clear();
    std::cout << "Matching engine stopped." << std::endl;
}

void MatchingEngine::submitOrder(std::shared_ptr<Order> order) {
    if (!running_) {
        throw std::runtime_error("Matching engine is not running");
    }
    
    orderQueue_.enqueue(order);
}

std::vector<Trade> MatchingEngine::processOrderSync(std::shared_ptr<Order> order) {
    // Process the order in our single order book
    // In the future, this would select the appropriate order book based on instrument
    auto trades = orderBook_.addOrder(order, [this](const Trade& trade) {
        this->onTrade(trade);
    });
    
    // Update statistics
    stats_.totalOrdersProcessed++;
    stats_.totalTradesExecuted += trades.size();
    for (const auto& trade : trades) {
        stats_.totalQuantityTraded += trade.getQuantity();
    }
    
    return trades;
}

const OrderBook& MatchingEngine::getOrderBook() const {
    return orderBook_;
}

void MatchingEngine::workerFunction() {
    while (running_) {
        auto order = orderQueue_.dequeue();
        
        // Check for shutdown signal
        if (!order) break;
        
        // Process the order
        processOrderSync(order);
    }
}

void MatchingEngine::onTrade(const Trade& trade) {
    // In the future, this could notify subscribers, update positions, etc.
    std::cout << "TRADE EXECUTED: " << trade.toString() << std::endl;
    
    // Forward to external callback if registered
    if (tradeCallback_) {
        tradeCallback_(trade);
    }
}

} // namespace engine
