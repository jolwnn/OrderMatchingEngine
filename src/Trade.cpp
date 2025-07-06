#include "engine/Trade.hpp"
#include <sstream>
#include <iomanip>

namespace engine {

Trade::Trade(Order::OrderId buyOrderId, Order::OrderId sellOrderId, 
             Order::Price price, Order::Quantity quantity)
    : buyOrderId_(buyOrderId), 
      sellOrderId_(sellOrderId), 
      price_(price), 
      quantity_(quantity), 
      timestamp_(std::chrono::system_clock::now()) {
}

std::string Trade::toString() const {
    std::ostringstream oss;
    oss << "Trade{buy=" << buyOrderId_ 
        << ", sell=" << sellOrderId_
        << ", price=" << std::fixed << std::setprecision(2) << price_
        << ", qty=" << quantity_ << "}";
    return oss.str();
}

} // namespace engine
