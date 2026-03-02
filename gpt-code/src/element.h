#pragma once

#include <Eigen/Dense>
#include <vector>
#include "node.h"

namespace fem {

/**
 * @brief Класс треугольного конечного элемента
 * 
 * Реализует линейные симплекс-элементы для 2D задачи
 * с использованием L-координат (площадных координат)
 */
class Element {
public:
    Element() = default;
    Element(int id, const std::vector<int>& nodeIds);
    
    // Геттеры
    int getId() const { return id_; }
    const std::vector<int>& getNodeIds() const { return nodeIds_; }
    double getArea() const { return area_; }
    
    // Вычисление геометрических характеристик
    void computeGeometry(const std::vector<Node>& nodes);
    
    // Вычисление локальной матрицы жёсткости
    Eigen::Matrix3d computeLocalStiffnessMatrix(
        const std::vector<Node>& nodes) const;
    
    // Вычисление градиентов функций формы
    Eigen::Matrix<double, 2, 3> computeShapeFunctionGradients(
        const std::vector<Node>& nodes) const;
    
    // Получение координат узлов элемента
    Eigen::Matrix<double, 3, 2> getNodeCoordinates(
        const std::vector<Node>& nodes) const;
    
private:
    int id_ = -1;                           ///< ID элемента
    std::vector<int> nodeIds_;              ///< ID узлов элемента (3 узла)
    double area_ = 0.0;                     ///< Площадь элемента
    mutable Eigen::Matrix3d localK_;        ///< Локальная матрица жёсткости
    mutable bool localKComputed_ = false;   ///< Флаг вычисления матрицы
};

} // namespace fem

