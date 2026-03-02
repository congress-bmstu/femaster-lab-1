#pragma once

#include <string>
#include <vector>
#include "boundary_condition.h"

namespace fem {

/**
 * @brief Конфигурация задачи из YAML файла
 * 
 * Содержит все параметры задачи: пути к файлам, параметры потока,
 * настройки решателя и граничные условия
 */
struct SolverConfig {
    int maxIterations = 1000;     ///< Максимум итераций решателя
    double tolerance = 1e-10;     ///< Точность решения
    std::string solverType = "CG"; ///< Тип решателя (CG, BiCGSTAB, Direct)
};

class ProblemConfig {
public:
    ProblemConfig() = default;
    
    // Загрузка конфигурации из YAML
    bool loadFromYaml(const std::string& filename);
    
    // Геттеры
    const std::string& getMeshFile() const { return meshFile_; }
    const std::string& getOutputFile() const { return outputFile_; }
    double getUInf() const { return U_inf_; }
    double getH() const { return H_; }
    double getD() const { return D_; }
    const std::vector<BCConfig>& getBCConfigs() const { return bcConfigs_; }
    const SolverConfig& getSolverConfig() const { return solverConfig_; }
    
    // Валидация конфигурации
    bool validate() const;
    
private:
    std::string meshFile_;                    ///< Путь к файлу сетки
    std::string outputFile_;                  ///< Путь к файлу результатов
    std::string inputFile_;                   ///< Путь к input.yaml
    
    // Параметры задачи
    double U_inf_ = 40.0;    ///< Скорость набегающего потока (мм/с)
    double H_ = 80.0;        ///< Расстояние между стенками (мм)
    double D_ = 40.0;        ///< Диаметр цилиндра (мм)
    
    std::vector<BCConfig> bcConfigs_;         ///< Конфигурации граничных условий
    SolverConfig solverConfig_;               ///< Настройки решателя
};

} // namespace fem

