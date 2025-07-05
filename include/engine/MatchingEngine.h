#pragma once

#include "OrderBook.h"
#include <unordered_map>
#include <string>

namespace engine {

/**
 * @brief Class representing a matching engine
 * 
 * Manages multiple order books for different instruments.
 * Note: In this prototype, we'll just use a single order book.
 */
class MatchingEngine {
public:
    MatchingEngine();
    
    /**
     * @brief Add an order to the matching engine
     * 
     * @param order The order to process
     * @return std::vector<Trade> Resulting trades
     */
    std::vector<Trade> processOrder(std::shared_ptr<Order> order);
    
    /**
     * @brief Get the order book
     * 
     * @return const OrderBook& Reference to the order book
     */
    const OrderBook& getOrderBook() const;
    
private:
    OrderBook orderBook_;
    
    /**
     * @brief Callback for trade notifications
     * 
     * @param trade The trade that occurred
     */
    void onTrade(const Trade& trade);
};

} // namespace engine
