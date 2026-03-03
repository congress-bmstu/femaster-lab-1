#include "element.h"
#include <cmath>

namespace fem {

Element::Element(int id, const std::vector<int>& nodeIds)
    : id_(id), nodeIds_(nodeIds) {
    if (nodeIds_.size() != 3) {
        throw std::runtime_error("Element must have exactly 3 nodes");
    }
}

void Element::computeGeometry(const std::vector<Node>& nodes, const std::unordered_map<int, int> &nodeIndexMap) {
    if (nodeIds_.size() != 3) return;
    
    const Node& n1 = nodes[ nodeIndexMap.at(nodeIds_[0]) ];
    const Node& n2 = nodes[ nodeIndexMap.at(nodeIds_[1]) ];
    const Node& n3 = nodes[ nodeIndexMap.at(nodeIds_[2]) ];
    
    // Площадь треугольника через определитель
    // S = 0.5 * |x1(y2-y3) + x2(y3-y1) + x3(y1-y2)|
    area_ = 0.5 * std::abs(
        n1.getX() * (n2.getY() - n3.getY()) +
        n2.getX() * (n3.getY() - n1.getY()) +
        n3.getX() * (n1.getY() - n2.getY())
    );
    
    localKComputed_ = false;
}

Eigen::Matrix<double, 2, 3> Element::computeShapeFunctionGradients(
    const std::vector<Node>& nodes, const std::unordered_map<int, int> &nodeIndexMap) const {
    
    Eigen::Matrix<double, 2, 3> gradN;
    
    if (nodeIds_.size() != 3) return gradN;
        
    const Node& n1 = nodes[ nodeIndexMap.at(nodeIds_[0]) ];
    const Node& n2 = nodes[ nodeIndexMap.at(nodeIds_[1]) ];
    const Node& n3 = nodes[ nodeIndexMap.at(nodeIds_[2]) ];
    
    double x1 = n1.getX(), y1 = n1.getY();
    double x2 = n2.getX(), y2 = n2.getY();
    double x3 = n3.getX(), y3 = n3.getY();
    
    // Определитель Якобиана (2 * площадь)
    double detJ = x1*(y2 - y3) - x2*(y1 - y3) + x3*(y1 - y2);
    
    if (std::abs(detJ) < 1e-12) {
        throw std::runtime_error("Degenerate element detected -- определитель матрицы Якоби равен нулю: элемент состоит из вершин " + std::to_string(nodeIds_[0]) + ", " + std::to_string(nodeIds_[1]) + ", " + std::to_string(nodeIds_[2]));
    }
    
    // Градиенты функций формы (из теории МКЭ для линейных треугольников)
    // ∇N1 = (y2-y3, x3-x2) / detJ
    // ∇N2 = (y3-y1, x1-x3) / detJ
    // ∇N3 = (y1-y2, x2-x1) / detJ
    
    gradN(0, 0) = (y2 - y3) / detJ;  // ∂N1/∂x
    gradN(1, 0) = (x3 - x2) / detJ;  // ∂N1/∂y
    
    gradN(0, 1) = (y3 - y1) / detJ;  // ∂N2/∂x
    gradN(1, 1) = (x1 - x3) / detJ;  // ∂N2/∂y
    
    gradN(0, 2) = (y1 - y2) / detJ;  // ∂N3/∂x
    gradN(1, 2) = (x2 - x1) / detJ;  // ∂N3/∂y
    
    return gradN;
}

Eigen::Matrix3d Element::computeLocalStiffnessMatrix(
    const std::vector<Node>& nodes, const std::unordered_map<int, int> &nodeIndexMap) const {
    
    if (localKComputed_) return localK_;
    
    Eigen::Matrix<double, 2, 3> gradN = computeShapeFunctionGradients(nodes, nodeIndexMap);
    
    // Локальная матрица жёсткости: K_ij = ∫∫ ∇Ni · ∇Nj dV
    // Для линейных треугольников градиенты постоянны, поэтому:
    // K_ij = (∇Ni · ∇Nj) * S * t (t = 1 для 2D)
    
    localK_ = Eigen::Matrix3d::Zero();
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            double dotProduct = gradN(0, i) * gradN(0, j) + 
                               gradN(1, i) * gradN(1, j);
            localK_(i, j) = dotProduct * area_;
        }
    }
    
    localKComputed_ = true;
    return localK_;
}

Eigen::Matrix<double, 3, 2> Element::getNodeCoordinates(
    const std::vector<Node>& nodes, const std::unordered_map<int, int> &nodeIndexMap) const {
    
    Eigen::Matrix<double, 3, 2> coords;
    
    for (int i = 0; i < 3; ++i) {
        const Node& node = nodes[ nodeIndexMap.at(nodeIds_[i]) ];
        coords(i, 0) = node.getX();
        coords(i, 1) = node.getY();
    }
    
    return coords;
}

} // namespace fem
