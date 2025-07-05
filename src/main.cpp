#include "engine/MatchingEngine.h"
#include <iostream>
#include <memory>
#include <vector>
#include <string>

using namespace engine;

// Simple helper to create orders
std::shared_ptr<Order> createOrder(
    Order::OrderId id, 
    OrderSide side, 
    Order::Price price, 
    Order::Quantity quantity) {
    
    return std::make_shared<Order>(id, side, OrderType::LIMIT, price, quantity);
}

// Demo of the matching engine functionality
int main() {
    std::cout << "Order Matching Engine Demo" << std::endl;
    std::cout << "==========================" << std::endl << std::endl;
    
    // Create the matching engine
    MatchingEngine matchingEngine;
    
    // Add some initial orders
    std::cout << "Adding initial orders to the book:" << std::endl;
    
    // Add some buy orders
    matchingEngine.processOrder(createOrder(1, OrderSide::BUY, 100.0, 10));
    matchingEngine.processOrder(createOrder(2, OrderSide::BUY, 99.0, 20));
    matchingEngine.processOrder(createOrder(3, OrderSide::BUY, 98.0, 30));
    
    // Add some sell orders
    matchingEngine.processOrder(createOrder(4, OrderSide::SELL, 102.0, 15));
    matchingEngine.processOrder(createOrder(5, OrderSide::SELL, 103.0, 25));
    matchingEngine.processOrder(createOrder(6, OrderSide::SELL, 104.0, 35));
    
    // Print the current state of the order book
    std::cout << "\nCurrent Order Book:" << std::endl;
    std::cout << matchingEngine.getOrderBook().toString() << std::endl;
    
    // Demonstrate matching - this order will match with order #4
    std::cout << "\nAdding a new BUY order that will match:" << std::endl;
    auto trades1 = matchingEngine.processOrder(createOrder(7, OrderSide::BUY, 102.0, 5));
    std::cout << "Trades executed: " << trades1.size() << std::endl;
    
    // Print the updated order book
    std::cout << "\nUpdated Order Book:" << std::endl;
    std::cout << matchingEngine.getOrderBook().toString() << std::endl;
    
    // Add a sell order that matches multiple buy orders
    std::cout << "\nAdding a new SELL order that will match multiple BUY orders:" << std::endl;
    auto trades2 = matchingEngine.processOrder(createOrder(8, OrderSide::SELL, 98.0, 50));
    std::cout << "Trades executed: " << trades2.size() << std::endl;
    
    // Print the final order book
    std::cout << "\nFinal Order Book:" << std::endl;
    std::cout << matchingEngine.getOrderBook().toString() << std::endl;
    
    return 0;
}
