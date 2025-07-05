# Order Matching Engine

A professional-grade order book and order matching engine implemented in C++17. This project implements a limit order book with price-time priority matching.

## Features

- Limit Order Book (LOB) implementation
- Price-time priority matching algorithm
- Buy/sell order placement
- Trade execution
- Order book state visualization

## Project Structure

```
.
├── CMakeLists.txt       # Build configuration
├── include/             # Header files
│   └── engine/          # Engine components headers
│       ├── MatchingEngine.h
│       ├── Order.h
│       ├── OrderBook.h
│       └── Trade.h
└── src/                # Source files
    ├── MatchingEngine.cpp
    ├── Order.cpp
    ├── OrderBook.cpp
    ├── Trade.cpp
    └── main.cpp        # Demo application
```

## Core Components

- **Order**: Represents a buy or sell order with price, quantity, and time priority
- **OrderBook**: Maintains separate buy and sell order books with matching logic
- **Trade**: Represents a match between two orders
- **MatchingEngine**: Coordinates the processing of orders

## Build Instructions

### Prerequisites

- CMake 3.14 or higher
- C++17 compatible compiler (GCC, Clang, or MSVC)

### Building

```bash
# Create build directory
mkdir build && cd build

# Generate build files
cmake ..

# Build the project
cmake --build .
```

### Running

```bash
# From the build directory
./OrderMatchingEngine
```

## Future Enhancements

This project is designed as an extensible foundation that can be extended to support:

- Concurrency for high-performance trading
- Additional order types (market orders, stop orders, etc.)
- Multiple tradable instruments
- FIX protocol integration
- REST or WebSocket API interfaces
- Persistence layer for order storage

## License

This project is for educational purposes and personal portfolio use.
