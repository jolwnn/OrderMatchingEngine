#include "engine/OrderBook.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace engine {

OrderBook::OrderBook() = default;

std::vector<Trade> OrderBook::addOrder(OrderPtr order, TradeCallback tradeCallback) {
    // Lock exclusively as we're modifying the order book
    std::unique_lock<std::shared_mutex> lock(mutex_);
    std::vector<Trade> trades;
    
    // Try to match the order first
    if (order->getSide() == OrderSide::BUY) {
        trades = matchBuyOrder(order, tradeCallback);
    } else {
        trades = matchSellOrder(order, tradeCallback);
    }
    
    // If the order is not fully filled, add it to the book (only for limit orders)
    // Market orders that aren't fully filled will not be added to the book
    if (order->getRemainingQuantity() > 0 && 
        order->getStatus() != OrderStatus::CANCELED && 
        order->getType() == OrderType::LIMIT) {
        
        if (order->getSide() == OrderSide::BUY) {
            buyOrders_.insert(order);
        } else {
            sellOrders_.insert(order);
        }
        
        // Add to the order map for quick lookups
        orderMap_[order->getId()] = order;
    }
    
    return trades;
}

Order::Price OrderBook::getBestBidPrice() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (buyOrders_.empty()) {
        return 0.0;  // No bids
    }
    return (*buyOrders_.begin())->getPrice();
}

Order::Price OrderBook::getBestAskPrice() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (sellOrders_.empty()) {
        return std::numeric_limits<Order::Price>::max();  // No asks
    }
    return (*sellOrders_.begin())->getPrice();
}

std::string OrderBook::toString() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    std::ostringstream oss;
    oss << "ORDER BOOK\n"
        << "-------------------------------------------\n"
        << std::setw(10) << "BUY" << " | " << std::setw(10) << "SELL" << "\n"
        << "-------------------------------------------\n"
        << std::fixed << std::setprecision(2);
    
    // Show top 5 levels from each side
    int levels = 0;
    auto buyIt = buyOrders_.begin();
    auto sellIt = sellOrders_.begin();
    
    while ((buyIt != buyOrders_.end() || sellIt != sellOrders_.end()) && levels < 5) {
        oss << std::setw(10);
        
        if (buyIt != buyOrders_.end()) {
            oss << (*buyIt)->getPrice() << "x" << (*buyIt)->getRemainingQuantity();
            ++buyIt;
        } else {
            oss << "-";
        }
        
        oss << " | " << std::setw(10);
        
        if (sellIt != sellOrders_.end()) {
            oss << (*sellIt)->getPrice() << "x" << (*sellIt)->getRemainingQuantity();
            ++sellIt;
        } else {
            oss << "-";
        }
        
        oss << "\n";
        levels++;
    }
    
    return oss.str();
}

std::vector<Trade> OrderBook::matchBuyOrder(OrderPtr buyOrder, TradeCallback tradeCallback) {
    std::vector<Trade> trades;
    Order::Quantity remainingQty = buyOrder->getQuantity();
    
    // Match against sell orders
    while (remainingQty > 0 && !sellOrders_.empty()) {
        auto bestSell = sellOrders_.begin();
        auto sellOrder = *bestSell;
        
        // For limit orders, check if we can match price
        // Market orders always match against best available price
        if (buyOrder->getType() == OrderType::LIMIT && buyOrder->getPrice() < sellOrder->getPrice()) {
            break;  // No more matches possible for limit orders below the best sell price
        }
        
        // Calculate trade quantity
        Order::Quantity tradeQty = std::min(remainingQty, sellOrder->getRemainingQuantity());
        
        // Execute the trade
        buyOrder->fill(tradeQty);
        sellOrder->fill(tradeQty);
        remainingQty -= tradeQty;
        
        // Record the trade
        Trade trade(buyOrder->getId(), sellOrder->getId(), sellOrder->getPrice(), tradeQty);
        trades.push_back(trade);
        
        // Notify via callback if provided
        if (tradeCallback) {
            tradeCallback(trade);
        }
        
        // Remove sell order from book if fully filled
        if (sellOrder->getStatus() == OrderStatus::FILLED) {
            sellOrders_.erase(bestSell);
            orderMap_.erase(sellOrder->getId());
        }
    }
    
    // If it's a market order that couldn't be fully filled, mark remaining as canceled
    if (buyOrder->getType() == OrderType::MARKET && remainingQty > 0) {
        buyOrder->cancel();
    }
    
    return trades;
}

std::vector<Trade> OrderBook::matchSellOrder(OrderPtr sellOrder, TradeCallback tradeCallback) {
    std::vector<Trade> trades;
    Order::Quantity remainingQty = sellOrder->getQuantity();
    
    // Match against buy orders
    while (remainingQty > 0 && !buyOrders_.empty()) {
        auto bestBuy = buyOrders_.begin();
        auto buyOrder = *bestBuy;
        
        // For limit orders, check if we can match price
        // Market orders always match against best available price
        if (sellOrder->getType() == OrderType::LIMIT && sellOrder->getPrice() > buyOrder->getPrice()) {
            break;  // No more matches possible for limit orders above the best buy price
        }
        
        // Calculate trade quantity
        Order::Quantity tradeQty = std::min(remainingQty, buyOrder->getRemainingQuantity());
        
        // Execute the trade
        sellOrder->fill(tradeQty);
        buyOrder->fill(tradeQty);
        remainingQty -= tradeQty;
        
        // Record the trade
        Trade trade(buyOrder->getId(), sellOrder->getId(), buyOrder->getPrice(), tradeQty);
        trades.push_back(trade);
        
        // Notify via callback if provided
        if (tradeCallback) {
            tradeCallback(trade);
        }
        
        // Remove buy order from book if fully filled
        if (buyOrder->getStatus() == OrderStatus::FILLED) {
            buyOrders_.erase(bestBuy);
            orderMap_.erase(buyOrder->getId());
        }
    }
    
    // If it's a market order that couldn't be fully filled, mark remaining as canceled
    if (sellOrder->getType() == OrderType::MARKET && remainingQty > 0) {
        sellOrder->cancel();
    }
    
    return trades;
}

size_t OrderBook::getBuyOrderCount() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return buyOrders_.size();
}

size_t OrderBook::getSellOrderCount() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return sellOrders_.size();
}

} // namespace engine
