#pragma once

#include <Eigen/Dense>
#include <vector>

namespace fem {

/**
 * @brief Типы граничных условий
 */
enum class BCType {
    NONE,       ///< Нет граничного условия
    DIRICHLET,  ///< Условие Дирихле (ψ = const)
    NEUMANN     ///< Условие Неймана (∂ψ/∂n = 0)
};

/**
 * @brief Класс узла сетки
 * 
 * Хранит координаты узла, значение функции тока и информацию
 * о граничных условиях
 */
class Node {
public:
    Node() = default;
    Node(int id, double x, double y);
    
    // Геттеры
    int getId() const { return id_; }
    double getX() const { return x_; }
    double getY() const { return y_; }
    double getPsi() const { return psi_; }
    BCType getBCType() const { return bcType_; }
    double getBCValue() const { return bcValue_; }
    const std::vector<int> &getTags() const { return tags_; }
    
    // Сеттеры
    void setPsi(double psi) { psi_ = psi; }
    void setBC(BCType type, double value = 0.0);
    void addTag(int tag);
    
    // Координаты как вектор Eigen
    Eigen::Vector2d getCoordinates() const;
    
private:
    int id_ = -1;                    ///< ID узла
    double x_ = 0.0;                 ///< Координата X
    double y_ = 0.0;                 ///< Координата Y
    double psi_ = 0.0;               ///< Значение функции тока
    BCType bcType_ = BCType::NONE;   ///< Тип граничного условия
    double bcValue_ = 0.0;           ///< Значение граничного условия

    std::vector<int> tags_;
};

} // namespace fem

