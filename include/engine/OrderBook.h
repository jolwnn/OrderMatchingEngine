#pragma once

#include "Order.h"
#include "Trade.h"
#include <map>
#include <set>
#include <memory>
#include <vector>
#include <functional>

namespace engine {

/**
 * @brief Comparison functor for buy orders (highest price first)
 */
struct BuyOrderComparator {
    bool operator()(const std::shared_ptr<Order>& lhs, const std::shared_ptr<Order>& rhs) const {
        // For buy orders, higher prices have priority
        if (lhs->getPrice() != rhs->getPrice()) {
            return lhs->getPrice() > rhs->getPrice();
        }
        // If prices equal, older orders have priority (FIFO)
        return lhs->getTimestamp() < rhs->getTimestamp();
    }
};

/**
 * @brief Comparison functor for sell orders (lowest price first)
 */
struct SellOrderComparator {
    bool operator()(const std::shared_ptr<Order>& lhs, const std::shared_ptr<Order>& rhs) const {
        // For sell orders, lower prices have priority
        if (lhs->getPrice() != rhs->getPrice()) {
            return lhs->getPrice() < rhs->getPrice();
        }
        // If prices equal, older orders have priority (FIFO)
        return lhs->getTimestamp() < rhs->getTimestamp();
    }
};

/**
 * @brief Class representing a limit order book
 * 
 * Maintains separate books for buy and sell orders and implements matching logic.
 */
class OrderBook {
public:
    using OrderPtr = std::shared_ptr<Order>;
    using PriceLevel = std::multiset<OrderPtr, BuyOrderComparator>;
    using OrderMap = std::map<Order::OrderId, OrderPtr>;
    using TradeCallback = std::function<void(const Trade&)>;
    
    OrderBook();
    
    /**
     * @brief Add an order to the book and perform matching
     * 
     * @param order The order to add
     * @param tradeCallback Callback for trade notifications
     * @return std::vector<Trade> Vector of trades generated from this order
     */
    std::vector<Trade> addOrder(OrderPtr order, TradeCallback tradeCallback = nullptr);
    
    /**
     * @brief Get best bid price (highest buy price)
     * 
     * @return Order::Price The best bid price or 0 if no bids
     */
    Order::Price getBestBidPrice() const;
    
    /**
     * @brief Get best ask price (lowest sell price)
     * 
     * @return Order::Price The best ask price or max double if no asks
     */
    Order::Price getBestAskPrice() const;
    
    /**
     * @brief Print the current state of the order book
     */
    std::string toString() const;
    
private:
    using BuyOrderBook = std::multiset<OrderPtr, BuyOrderComparator>;
    using SellOrderBook = std::multiset<OrderPtr, SellOrderComparator>;
    
    BuyOrderBook buyOrders_;
    SellOrderBook sellOrders_;
    OrderMap orderMap_;  // For fast lookups by ID
    
    /**
     * @brief Match a new buy order against the sell book
     * 
     * @param buyOrder Buy order to match
     * @param tradeCallback Callback for trade notifications
     * @return std::vector<Trade> Resulting trades
     */
    std::vector<Trade> matchBuyOrder(OrderPtr buyOrder, TradeCallback tradeCallback);
    
    /**
     * @brief Match a new sell order against the buy book
     * 
     * @param sellOrder Sell order to match
     * @param tradeCallback Callback for trade notifications
     * @return std::vector<Trade> Resulting trades
     */
    std::vector<Trade> matchSellOrder(OrderPtr sellOrder, TradeCallback tradeCallback);
};

} // namespace engine
