#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <chrono>

namespace engine {

/**
 * @brief Enumeration for order side (buy/sell)
 */
enum class OrderSide {
    BUY,
    SELL
};

/**
 * @brief Enumeration for order type
 */
enum class OrderType {
    LIMIT,   // Future: MARKET, STOP, etc.
};

/**
 * @brief Order status tracking
 */
enum class OrderStatus {
    NEW,
    PARTIALLY_FILLED,
    FILLED,
    CANCELED,
    REJECTED
};

/**
 * @brief Class representing a trading order
 * 
 * Implements RAII principles with proper ownership semantics.
 */
class Order {
public:
    using OrderId = std::uint64_t;
    using Price = double;
    using Quantity = std::uint64_t;
    using TimeStamp = std::chrono::time_point<std::chrono::system_clock>;
    
    /**
     * @brief Construct a new Order object
     * 
     * @param id Unique order identifier
     * @param side BUY or SELL
     * @param type Order type (currently only LIMIT)
     * @param price Order price
     * @param quantity Order quantity
     */
    Order(OrderId id, OrderSide side, OrderType type, Price price, Quantity quantity);

    // Getters
    OrderId getId() const { return id_; }
    OrderSide getSide() const { return side_; }
    OrderType getType() const { return type_; }
    Price getPrice() const { return price_; }
    Quantity getQuantity() const { return quantity_; }
    Quantity getFilledQuantity() const { return filledQuantity_; }
    Quantity getRemainingQuantity() const { return quantity_ - filledQuantity_; }
    TimeStamp getTimestamp() const { return timestamp_; }
    OrderStatus getStatus() const { return status_; }
    
    /**
     * @brief Record a fill against this order
     * 
     * @param fillQuantity Quantity that was filled
     * @return true if order is completely filled
     */
    bool fill(Quantity fillQuantity);
    
    /**
     * @brief Cancel the order
     */
    void cancel();
    
    /**
     * @brief String representation of the order
     */
    std::string toString() const;

private:
    OrderId id_;
    OrderSide side_;
    OrderType type_;
    Price price_;
    Quantity quantity_;
    Quantity filledQuantity_;
    TimeStamp timestamp_;
    OrderStatus status_;
};

} // namespace engine
