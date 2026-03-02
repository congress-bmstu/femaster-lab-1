#pragma once

#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>
#include "mesh.h"
#include "boundary_condition.h"
#include "problem_config.h"
#include "utils.h"

namespace fem {

/**
 * @brief Структура результатов решения
 */
struct SolutionResult {
    bool success = false;           ///< Успешность решения
    int iterations = 0;             ///< Количество итераций
    double residual = 0.0;          ///< Остаток
    double assemblyTime = 0.0;      ///< Время сборки матрицы (с)
    double solveTime = 0.0;         ///< Время решения (с)
};

/**
 * @brief Основной решатель задачи МКЭ
 * 
 * Выполняет сборку глобальной матрицы жёсткости, применение
 * граничных условий и решение СЛАУ
 */
class FEMSolver {
public:
    FEMSolver() = default;
    
    // Инициализация решателя
    void initialize(Mesh& mesh, const ProblemConfig& config);
    
    // Сборка глобальной матрицы жёсткости
    void assembleStiffnessMatrix();
    
    // Применение граничных условий Дирихле
    void applyDirichletBoundaryConditions(
        const BoundaryConditionManager& bcManager);
    
    // Решение СЛАУ
    SolutionResult solve();
    
    // Получение решения
    const Eigen::VectorXd& getSolution() const { return psi_; }
    
    // Вычисление скоростей в узлах
    void computeVelocities(const Mesh& mesh);
    const Eigen::VectorXd& getVx() const { return vx_; }
    const Eigen::VectorXd& getVy() const { return vy_; }
    
    // Статистика
    size_t getNumDOF() const { return numDOF_; }
    double getConditionNumber() const { return conditionNumber_; }
    
private:
    Mesh* mesh_ = nullptr;                          ///< Указатель на сетку
    const ProblemConfig* config_ = nullptr;         ///< Указатель на конфигурацию
    
    Eigen::SparseMatrix<double> K_;                 ///< Глобальная матрица жёсткости
    Eigen::VectorXd F_;                             ///< Глобальный вектор нагрузки
    Eigen::VectorXd psi_;                           ///< Решение (функция тока)
    Eigen::VectorXd vx_;                            ///< Скорость X
    Eigen::VectorXd vy_;                            ///< Скорость Y
    
    size_t numDOF_ = 0;                             ///< Количество степеней свободы
    double conditionNumber_ = 0.0;                  ///< Число обусловленности
    
    // Сборка вклада элемента в глобальную матрицу
    void assembleElementContribution(
        const Element& element,
        const std::vector<Node>& nodes);
    
    // Вычисление скоростей через производные функции тока
    void computeElementVelocities(
        const Element& element,
        const std::vector<Node>& nodes);
};

} // namespace fem

