#include "engine/MatchingEngine.h"
#include <iostream>

namespace engine {

MatchingEngine::MatchingEngine() = default;

std::vector<Trade> MatchingEngine::processOrder(std::shared_ptr<Order> order) {
    // Process the order in our single order book
    // In the future, this would select the appropriate order book based on instrument
    return orderBook_.addOrder(order, [this](const Trade& trade) {
        this->onTrade(trade);
    });
}

const OrderBook& MatchingEngine::getOrderBook() const {
    return orderBook_;
}

void MatchingEngine::onTrade(const Trade& trade) {
    // In the future, this could notify subscribers, update positions, etc.
    std::cout << "TRADE EXECUTED: " << trade.toString() << std::endl;
}

} // namespace engine
