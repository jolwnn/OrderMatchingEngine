#pragma once

#include "Order.hpp"
#include <memory>
#include <vector>

namespace engine {

/**
 * @brief Represents a trade resulting from matched orders
 */
class Trade {
public:
    /**
     * @brief Create a new trade from matched buy and sell orders
     * 
     * @param buyOrderId ID of buy order
     * @param sellOrderId ID of sell order
     * @param price Execution price
     * @param quantity Executed quantity
     */
    Trade(Order::OrderId buyOrderId, Order::OrderId sellOrderId, 
          Order::Price price, Order::Quantity quantity);
    
    // Getters
    Order::OrderId getBuyOrderId() const { return buyOrderId_; }
    Order::OrderId getSellOrderId() const { return sellOrderId_; }
    Order::Price getPrice() const { return price_; }
    Order::Quantity getQuantity() const { return quantity_; }
    Order::TimeStamp getTimestamp() const { return timestamp_; }
    
    /**
     * @brief String representation of the trade
     */
    std::string toString() const;

private:
    Order::OrderId buyOrderId_;
    Order::OrderId sellOrderId_;
    Order::Price price_;
    Order::Quantity quantity_;
    Order::TimeStamp timestamp_;
};

} // namespace engine
