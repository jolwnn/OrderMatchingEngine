# Order Matching Engine

A order book and order matching engine implemented in C++17. This project implements a limit and market order book with price-time priority matching, with concurrent order processing capabilities. (This project is 100% for my own learning, feel free to refer to it if you would like to implement your own!)

## Features

- Limit Order Book (LOB) implementation
- Price-time priority matching algorithm
- Buy/sell order placement
- Trade execution
- Order book state visualization
- Thread-safe, concurrent order processing
- Producer-consumer pattern for order submission/processing
- Performance measurement and benchmarking
- Atomic order ID generation

## Project Structure

```
.
├── CMakeLists.txt          # Build configuration
├── include/                # Header files
│   └── engine/             # Engine components headers
│       ├── MatchingEngine.h
│       ├── Order.h
│       ├── OrderBook.h
│       ├── OrderQueue.h
│       ├── Trade.h
│       └── util/           # Utility classes
│           ├── OrderIdGenerator.h
│           └── PerformanceTimer.h
└── src/                   # Source files
    ├── MatchingEngine.cpp
    ├── Order.cpp
    ├── OrderBook.cpp
    ├── Trade.cpp
    └── main.cpp           # Demo application
```

## Core Components

- **Order**: Represents a buy or sell order with price, quantity, and time priority
- **OrderBook**: Thread-safe implementation that maintains separate buy and sell order books with matching logic
- **OrderQueue**: Thread-safe queue that implements a producer-consumer pattern for order processing
- **Trade**: Represents a match between two orders
- **MatchingEngine**: Multi-threaded coordinator for order processing
- **OrderIdGenerator**: Thread-safe generator of unique order IDs
- **PerformanceTimer**: Utilities for performance measurement and benchmarking

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

## Concurrency Design

The project implements concurrency in several key areas:

1. **Thread-Safe Order Book**: Using a readers-writer lock (std::shared_mutex) to allow multiple concurrent reads but exclusive writes.

2. **Producer-Consumer Pattern**: Orders are submitted by producer threads and processed by consumer threads via a thread-safe queue.

3. **Atomic Order ID Generation**: Ensures unique order IDs even under heavy concurrent access.

4. **Worker Thread Pool**: The MatchingEngine uses a configurable number of worker threads to process orders.

## Performance Considerations

1. **Minimal Locking**: Lock granularity is minimized to reduce contention.

2. **Data Structure Efficiency**: Uses std::multiset with custom comparators for price-time priority.

3. **Benchmarking**: Includes utilities to measure and compare performance.

## Future Enhancements

The concurrent foundation can be further extended to support:

- Lock-free data structures for critical paths
- NUMA-aware memory management
- Advanced order types (market orders, stop orders, etc.)
- Multiple tradable instruments with isolated order books
- FIX protocol integration
- REST or WebSocket API interfaces
- Persistence layer for order storage
- Batched order processing for higher throughput
