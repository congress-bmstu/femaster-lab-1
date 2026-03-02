#pragma once

#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>

namespace fem {

/**
 * @brief Утилиты для логирования и измерения времени
 */
class Logger {
public:
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);
    static void success(const std::string& message);
    
    // Прогресс-бар
    static void progress(int current, int total, const std::string& label = "");
    
private:
    static void printWithLevel(const std::string& level, const std::string& message);
};

/**
 * @brief Измерение времени выполнения
 */
class Timer {
public:
    Timer() { start(); }
    
    void start() { start_ = std::chrono::high_resolution_clock::now(); }
    
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start_).count();
    }
    
    double elapsedAndRestart() {
        double t = elapsed();
        start();
        return t;
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_;
};

} // namespace fem

