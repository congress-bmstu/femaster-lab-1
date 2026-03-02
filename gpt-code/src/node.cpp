#include "node.h"

namespace fem {

Node::Node(int id, double x, double y)
    : id_(id), x_(x), y_(y) {}

void Node::setBC(BCType type, double value) {
    bcType_ = type;
    bcValue_ = value;
}

Eigen::Vector2d Node::getCoordinates() const {
    return Eigen::Vector2d(x_, y_);
}

} // namespace fem
