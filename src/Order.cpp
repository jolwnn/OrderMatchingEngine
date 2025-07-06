#include "engine/Order.hpp"
#include <sstream>
#include <iomanip>

namespace engine {

Order::Order(OrderId id, OrderSide side, OrderType type, Price price, Quantity quantity)
    : id_(id), 
      side_(side), 
      type_(type), 
      price_(price), 
      quantity_(quantity), 
      filledQuantity_(0), 
      timestamp_(std::chrono::system_clock::now()), 
      status_(OrderStatus::NEW) {
}

bool Order::fill(Quantity fillQuantity) {
    if (fillQuantity <= 0 || fillQuantity > getRemainingQuantity()) {
        return false;
    }
    
    filledQuantity_ += fillQuantity;
    
    if (filledQuantity_ == quantity_) {
        status_ = OrderStatus::FILLED;
        return true;
    } else {
        status_ = OrderStatus::PARTIALLY_FILLED;
        return false;
    }
}

void Order::cancel() {
    if (status_ != OrderStatus::FILLED) {
        status_ = OrderStatus::CANCELED;
    }
}

std::string Order::toString() const {
    std::ostringstream oss;
    oss << "Order{id=" << id_ 
        << ", side=" << (side_ == OrderSide::BUY ? "BUY" : "SELL")
        << ", type=" << (type_ == OrderType::LIMIT ? "LIMIT" : "MARKET");
    
    // Only display price for limit orders
    if (type_ == OrderType::LIMIT) {
        oss << ", price=" << std::fixed << std::setprecision(2) << price_;
    }
    
    oss << ", qty=" << quantity_
        << ", filled=" << filledQuantity_
        << ", status=";
        
    switch (status_) {
        case OrderStatus::NEW: oss << "NEW"; break;
        case OrderStatus::PARTIALLY_FILLED: oss << "PARTIALLY_FILLED"; break;
        case OrderStatus::FILLED: oss << "FILLED"; break;
        case OrderStatus::CANCELED: oss << "CANCELED"; break;
        case OrderStatus::REJECTED: oss << "REJECTED"; break;
    }
    
    oss << "}";
    return oss.str();
}

} // namespace engine
