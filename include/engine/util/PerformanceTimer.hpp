#pragma once

#include <chrono>
#include <string>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iomanip>

namespace engine {
namespace util {

/**
 * @brief Simple performance timer utility
 */
class PerformanceTimer {
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::nanoseconds;
    
    /**
     * @brief Start the timer
     */
    void start() {
        startTime_ = Clock::now();
    }
    
    /**
     * @brief Stop the timer
     */
    void stop() {
        endTime_ = Clock::now();
    }
    
    /**
     * @brief Get elapsed time in nanoseconds
     */
    int64_t elapsedNanoseconds() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(endTime_ - startTime_).count();
    }
    
    /**
     * @brief Get elapsed time in microseconds
     */
    double elapsedMicroseconds() const {
        return elapsedNanoseconds() / 1000.0;
    }
    
    /**
     * @brief Get elapsed time in milliseconds
     */
    double elapsedMilliseconds() const {
        return elapsedNanoseconds() / 1000000.0;
    }
    
    /**
     * @brief Get elapsed time in seconds
     */
    double elapsedSeconds() const {
        return elapsedNanoseconds() / 1000000000.0;
    }
    
private:
    TimePoint startTime_;
    TimePoint endTime_;
};

/**
 * @brief Performance benchmark runner
 */
class PerformanceBenchmark {
public:
    /**
     * @brief Run a benchmark function multiple times and report statistics
     * 
     * @param name Name of the benchmark
     * @param func Function to benchmark
     * @param iterations Number of iterations to run
     * @param warmupIterations Number of warmup iterations (not counted in stats)
     */
    template<typename Func>
    static void runBenchmark(const std::string& name, Func func, int iterations, int warmupIterations = 3) {
        std::cout << "Running benchmark: " << name << std::endl;
        
        // Warmup
        for (int i = 0; i < warmupIterations; ++i) {
            func();
        }
        
        // Actual benchmark
        std::vector<double> measurements;
        measurements.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            PerformanceTimer timer;
            timer.start();
            
            func();
            
            timer.stop();
            measurements.push_back(timer.elapsedMicroseconds());
        }
        
        // Calculate statistics
        double sum = std::accumulate(measurements.begin(), measurements.end(), 0.0);
        double mean = sum / iterations;
        
        std::sort(measurements.begin(), measurements.end());
        double median = iterations % 2 == 0 
            ? (measurements[iterations/2 - 1] + measurements[iterations/2]) / 2.0
            : measurements[iterations/2];
        
        double p95 = measurements[static_cast<size_t>(iterations * 0.95)];
        double p99 = measurements[static_cast<size_t>(iterations * 0.99)];
        
        double min = measurements.front();
        double max = measurements.back();
        
        // Calculate throughput (operations per second)
        double throughput = 1000000.0 / mean;
        
        // Print results
        std::cout << std::fixed << std::setprecision(3)
                  << "  Iterations:   " << iterations << std::endl
                  << "  Mean:         " << mean << " μs" << std::endl
                  << "  Median:       " << median << " μs" << std::endl
                  << "  P95:          " << p95 << " μs" << std::endl
                  << "  P99:          " << p99 << " μs" << std::endl
                  << "  Min:          " << min << " μs" << std::endl
                  << "  Max:          " << max << " μs" << std::endl
                  << "  Throughput:   " << std::setprecision(0) << throughput << " ops/sec" << std::endl
                  << std::endl;
    }
};

} // namespace util
} // namespace engine
