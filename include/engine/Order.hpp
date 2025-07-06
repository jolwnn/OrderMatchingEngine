#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <chrono>
#include <random>
#include "engine/util/OrderIdGenerator.hpp"

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

    /**
     * @brief Create a new order with an auto-generated ID
     * 
     * @param side BUY or SELL
     * @param type Order type (currently only LIMIT)
     * @param price Order price
     * @param quantity Order quantity
     * @return std::shared_ptr<Order> A shared pointer to the new order
     */
    static std::shared_ptr<Order> createOrder(
        OrderSide side, OrderType type, Price price, Quantity quantity) {
        
        OrderId id = util::OrderIdGenerator::getInstance().getNextId();
        return std::make_shared<Order>(id, side, type, price, quantity);
    }
    
    /**
     * @brief Create a random order for testing purposes
     * 
     * @param priceMin Minimum price
     * @param priceMax Maximum price
     * @param qtyMin Minimum quantity
     * @param qtyMax Maximum quantity
     * @return std::shared_ptr<Order> A shared pointer to the random order
     */
    static std::shared_ptr<Order> createRandomOrder(
        double priceMin = 90.0, double priceMax = 110.0,
        Quantity qtyMin = 1, Quantity qtyMax = 100) {
        
        static thread_local std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<> priceDist(priceMin, priceMax);
        std::uniform_int_distribution<Quantity> qtyDist(qtyMin, qtyMax);
        std::bernoulli_distribution sideDist(0.5); // 50% buy, 50% sell
        
        OrderSide side = sideDist(gen) ? OrderSide::BUY : OrderSide::SELL;
        double price = std::round(priceDist(gen) * 100) / 100; // Round to 2 decimal places
        Quantity qty = qtyDist(gen);
        
        return createOrder(side, OrderType::LIMIT, price, qty);
    }

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
