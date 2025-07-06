#include "engine/MatchingEngine.hpp"
#include "engine/util/PerformanceTimer.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <random>
#include <atomic>
#include <mutex>
#include <iomanip>
#include <algorithm>

using namespace engine;
using namespace engine::util;
using namespace std::chrono_literals;

// Mutex for clean console output
std::mutex coutMutex;

// Thread-safe console output helper
template<typename... Args>
void printThreadSafe(Args&&... args) {
    std::lock_guard<std::mutex> lock(coutMutex);
    (std::cout << ... << std::forward<Args>(args));
    std::cout << std::endl;
}

// Order producer function - generates and submits random orders
void orderProducer(
    MatchingEngine& engine, 
    int numOrders, 
    int delayMs = 0,
    std::string producerName = "Producer") {
    
    for (int i = 0; i < numOrders; ++i) {
        // Create a random order
        auto order = Order::createRandomOrder();
        
        // Submit to the matching engine
        engine.submitOrder(order);
        
        printThreadSafe(
            producerName, " submitted ", 
            (order->getSide() == OrderSide::BUY ? "BUY" : "SELL"), 
            " order: price=", std::fixed, std::setprecision(2), order->getPrice(), 
            " qty=", order->getQuantity(), 
            " [", i + 1, "/", numOrders, "]"
        );
        
        // Optional delay between orders
        if (delayMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }
}

// Demo of basic functionality with a sequential approach
void runBasicDemo(MatchingEngine& engine) {
    std::cout << "\n==== Basic Demo ====" << std::endl;
    
    // Add some initial orders
    std::cout << "Adding initial orders to the book:" << std::endl;
    
    // Add some buy orders
    engine.processOrderSync(Order::createOrder(OrderSide::BUY, OrderType::LIMIT, 100.0, 10));
    engine.processOrderSync(Order::createOrder(OrderSide::BUY, OrderType::LIMIT, 99.0, 20));
    engine.processOrderSync(Order::createOrder(OrderSide::BUY, OrderType::LIMIT, 98.0, 30));
    
    // Add some sell orders
    engine.processOrderSync(Order::createOrder(OrderSide::SELL, OrderType::LIMIT, 102.0, 15));
    engine.processOrderSync(Order::createOrder(OrderSide::SELL, OrderType::LIMIT, 103.0, 25));
    engine.processOrderSync(Order::createOrder(OrderSide::SELL, OrderType::LIMIT, 104.0, 35));
    
    // Print the current state of the order book
    std::cout << "\nCurrent Order Book:" << std::endl;
    std::cout << engine.getOrderBook().toString() << std::endl;
    
    // Demonstrate matching - this order will match with a sell order
    std::cout << "\nAdding a new BUY order that will match:" << std::endl;
    auto trades1 = engine.processOrderSync(Order::createOrder(OrderSide::BUY, OrderType::LIMIT, 102.0, 5));
    std::cout << "Trades executed: " << trades1.size() << std::endl;
    
    // Print the updated order book
    std::cout << "\nUpdated Order Book:" << std::endl;
    std::cout << engine.getOrderBook().toString() << std::endl;
    
    // Add a sell order that matches multiple buy orders
    std::cout << "\nAdding a new SELL order that will match multiple BUY orders:" << std::endl;
    auto trades2 = engine.processOrderSync(Order::createOrder(OrderSide::SELL, OrderType::LIMIT, 98.0, 50));
    std::cout << "Trades executed: " << trades2.size() << std::endl;
    
    // Print the final order book
    std::cout << "\nFinal Order Book:" << std::endl;
    std::cout << engine.getOrderBook().toString() << std::endl;
}

// Demo of concurrent order submission with multiple producers
void runConcurrentDemo(int numProducers, int ordersPerProducer) {
    std::cout << "\n==== Concurrent Demo ====" << std::endl;
    std::cout << "Starting matching engine with " << numProducers << " order producers" << std::endl;
    std::cout << "Each producer will submit " << ordersPerProducer << " orders" << std::endl;
    
    // Create the matching engine with 2 worker threads
    MatchingEngine engine(2);
    
    // Register trade callback
    std::atomic<int> tradeCount{0};
    engine.registerTradeCallback([&tradeCount](const Trade& trade) {
        tradeCount++;
    });
    
    // Start the engine
    engine.start();
    
    // Create and start producer threads
    std::vector<std::thread> producers;
    PerformanceTimer timer;
    
    timer.start();
    for (int i = 0; i < numProducers; ++i) {
        std::string name = "Producer-" + std::to_string(i+1);
        producers.emplace_back(
            orderProducer, 
            std::ref(engine), 
            ordersPerProducer, 
            0, // No delay between orders for maximum throughput
            name
        );
    }
    
    // Wait for all producers to finish
    for (auto& producer : producers) {
        producer.join();
    }
    
    // Wait a bit to process any remaining orders
    std::this_thread::sleep_for(500ms);
    
    timer.stop();
    
    // Print the final order book
    std::cout << "\nFinal Order Book:" << std::endl;
    std::cout << engine.getOrderBook().toString() << std::endl;
    
    // Print statistics
    double elapsed = timer.elapsedMilliseconds();
    int totalOrders = numProducers * ordersPerProducer;
    double throughput = totalOrders / (elapsed / 1000.0);
    
    std::cout << "\nPerformance Statistics:" << std::endl;
    std::cout << "  Total Orders:      " << totalOrders << std::endl;
    std::cout << "  Total Trades:      " << tradeCount.load() << std::endl;
    std::cout << "  Elapsed Time:      " << std::fixed << std::setprecision(2) << elapsed << " ms" << std::endl;
    std::cout << "  Throughput:        " << std::fixed << std::setprecision(2) << throughput << " orders/sec" << std::endl;
    std::cout << "  Book Status:       " << engine.getOrderBook().getBuyOrderCount() << " buy orders, " 
              << engine.getOrderBook().getSellOrderCount() << " sell orders" << std::endl;
    
    // Stop the engine
    engine.stop();
}

// Performance benchmark for order processing
void runPerformanceBenchmark() {
    std::cout << "\n==== Performance Benchmark ====" << std::endl;
    
    // Create a matching engine with a single worker
    MatchingEngine engine(1);
    engine.start();
    
    // Create a batch of random orders
    const int batchSize = 1000;
    std::vector<std::shared_ptr<Order>> orders;
    orders.reserve(batchSize);
    
    for (int i = 0; i < batchSize; ++i) {
        orders.push_back(Order::createRandomOrder());
    }
    
    // Benchmark synchronous order processing (direct)
    PerformanceBenchmark::runBenchmark(
        "Synchronous Order Processing",
        [&engine, &orders]() {
            // Process 10 orders per benchmark iteration
            for (int i = 0; i < 10; ++i) {
                int idx = i % orders.size();
                engine.processOrderSync(orders[idx]);
            }
        },
        100
    );
    
    // Benchmark asynchronous order submission
    PerformanceBenchmark::runBenchmark(
        "Asynchronous Order Submission",
        [&engine, &orders]() {
            // Submit 100 orders per benchmark iteration
            for (int i = 0; i < 100; ++i) {
                int idx = i % orders.size();
                engine.submitOrder(orders[idx]);
            }
        },
        100
    );
    
    // Wait for all orders to be processed
    std::this_thread::sleep_for(1s);
    
    // Clean up
    engine.stop();
}

int main(int argc, char* argv[]) {
    std::cout << "Concurrent Order Matching Engine Demo" << std::endl;
    std::cout << "====================================" << std::endl;
    
    // Get the number of hardware threads
    unsigned int numThreads = std::thread::hardware_concurrency();
    std::cout << "Running on a machine with " << numThreads << " hardware threads" << std::endl;
    
    try {
        // Create an engine for the basic demo
        MatchingEngine basicEngine(1);
        basicEngine.start();
        
        // Run the basic demo
        runBasicDemo(basicEngine);
        basicEngine.stop();
        
        // Run the concurrent demo with multiple producers
        runConcurrentDemo(4, 100);
        
        // Run performance benchmarks
        runPerformanceBenchmark();
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
