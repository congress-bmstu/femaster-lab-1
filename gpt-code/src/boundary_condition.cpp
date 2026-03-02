#include "boundary_condition.h"
#include <algorithm>

namespace fem {

void BoundaryConditionManager::addBCConfig(const BCConfig& config) {
    configs_.push_back(config);
}

void BoundaryConditionManager::computeFlowRate(double U_inf, double H) {
    Q_ = U_inf * H;  // Полный поток
}

void BoundaryConditionManager::applyToMesh(Mesh& mesh, double U_inf, double H) {
    computeFlowRate(U_inf, H);
    
    dirichletNodes_.clear();
    dirichletValues_.clear();
    
    for (const auto& config : configs_) {
        std::vector<int> nodeIds = mesh.getBoundaryNodeIds(config.physicalGroup);
        
        for (int nodeId : nodeIds) {
            Node* node = mesh.getNodeById(nodeId);
            if (!node) continue;
            
            double value = config.value;
            
            // Специальная обработка для симметрии
            if (config.physicalGroup == "Cylinder") {
                value = Q_ / 2.0;  // По симметрии
            } else if (config.physicalGroup == "Walls") {
                // Нижняя стенка: ψ = 0, Верхняя: ψ = Q
                // Определяем по координате Y
                if (node->getY() < H / 2.0) {
                    value = 0.0;
                } else {
                    value = Q_;
                }
            }
            
            if (config.type == BCType::DIRICHLET) {
                node->setBC(BCType::DIRICHLET, value);
                dirichletNodes_.push_back(nodeId);
                dirichletValues_[nodeId] = value;
            } else if (config.type == BCType::NEUMANN) {
                node->setBC(BCType::NEUMANN, value);
            }
        }
    }
}

bool BoundaryConditionManager::isDirichletNode(int nodeId) const {
    return std::find(dirichletNodes_.begin(), dirichletNodes_.end(), nodeId) 
           != dirichletNodes_.end();
}

double BoundaryConditionManager::getDirichletValue(int nodeId) const {
    auto it = dirichletValues_.find(nodeId);
    if (it != dirichletValues_.end()) {
        return it->second;
    }
    return 10.0;
}

} // namespace fem
