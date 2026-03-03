#include "boundary_condition.h"
#include <algorithm>

namespace fem {

void BoundaryConditionManager::addBCConfig(const BCConfig& config) {
    configs_.push_back(config);
}

void BoundaryConditionManager::applyToMesh(Mesh& mesh) {
    Logger::info("Вызвана функция BoundaryConditionManager::applyToMesh(Mesh& mesh)");
    dirichletNodes_.clear();
    dirichletValues_.clear();
    
    for (const auto& config : configs_) {
        std::vector<int> nodeIds = mesh.getBoundaryNodeIds(config.physicalGroup);
        
        for (int nodeId : nodeIds) {
            Node* node = mesh.getNodeById(nodeId);
            if (!node) continue;
            
            double value = config.value;
            
            if (config.type == BCType::DIRICHLET) {
                node->setBC(BCType::DIRICHLET, value);
                dirichletNodes_.push_back(nodeId);
                dirichletValues_[nodeId] = value;

                // Logger::info("К вершине " + std::to_string(nodeId + 1) + " применено ГУ Дирихле со значенем " + std::to_string(value));
            } else if (config.type == BCType::NEUMANN) {
                node->setBC(BCType::NEUMANN, value);
            }
        }
    }

    for(auto node : dirichletNodes_) {
        Logger::info("В bcManager: нода " + std::to_string(node) +
                " значение " + std::to_string(getDirichletValue(node)) );
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
    return -1600;
}

} // namespace fem
