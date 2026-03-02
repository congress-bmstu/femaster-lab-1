#pragma once

#include <string>
#include <vector>
#include "node.h"
#include "mesh.h"

namespace fem {

/**
 * @brief Описание граничного условия для физической группы
 */
struct BCConfig {
    std::string physicalGroup;  ///< Имя физической группы
    BCType type;                ///< Тип условия
    double value;               ///< Значение (для Дирихле)
};

/**
 * @brief Класс управления граничными условиями
 * 
 * Применяет граничные условия к узлам сетки на основе
 * конфигурации и физических групп GMSH
 */
class BoundaryConditionManager {
public:
    BoundaryConditionManager() = default;
    
    // Добавление конфигурации ГУ
    void addBCConfig(const BCConfig& config);
    
    // Применение всех ГУ к сетке
    void applyToMesh(Mesh& mesh, double U_inf, double H);
    
    // Получение списка узлов с условием Дирихле
    std::vector<int> getDirichletNodes() const { return dirichletNodes_; }
    
    // Проверка, является ли узел Дирихле
    bool isDirichletNode(int nodeId) const;
    
    // Получение значения ГУ для узла
    double getDirichletValue(int nodeId) const;
    
private:
    std::vector<BCConfig> configs_;           ///< Конфигурации ГУ
    std::vector<int> dirichletNodes_;         ///< Узлы с условием Дирихле
    std::unordered_map<int, double> dirichletValues_; ///< Значения для узлов Дирихле
    
    // Вычисление полного потока Q = U_inf * H
    void computeFlowRate(double U_inf, double H);
    double Q_ = 0.0;  ///< Полный поток
};

} // namespace fem

