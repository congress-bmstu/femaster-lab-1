#include "utils.h"

namespace fem {

void Logger::printWithLevel(const std::string& level, const std::string& message) {
    std::cout << "[" << level << "] " << message << std::endl;
}

void Logger::info(const std::string& message) {
    printWithLevel("INFO", message);
}

void Logger::warning(const std::string& message) {
    printWithLevel("WARN", message);
}

void Logger::error(const std::string& message) {
    printWithLevel("ERROR", message);
}

void Logger::success(const std::string& message) {
    printWithLevel("OK", message);
}

void Logger::progress(int current, int total, const std::string& label) {
    int percent = (current * 100) / total;
    std::cout << "\r" << label << " [";
    
    int barWidth = 50;
    int pos = (barWidth * current) / total;
    
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    
    std::cout << "] " << percent << "%";
    std::cout.flush();
    
    if (current == total) std::cout << std::endl;
}

} // namespace fem
