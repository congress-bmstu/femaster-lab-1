#include "mesh.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace fem {

bool Mesh::loadFromGmsh(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    if (!parseGmshVersion(file)) return false;
    if (!parseNodes(file)) return false;
    if (!parseElements(file)) return false;
    if (!parsePhysicalGroups(file)) return false;
    
    file.close();
    return true;
}

bool Mesh::parseGmshVersion(std::ifstream& file) {
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("$MeshFormat") != std::string::npos) {
            std::getline(file, line); // версия
            std::getline(file, line); // $EndMeshFormat
            return true;
        }
    }
    return false;
}

bool Mesh::parseNodes(std::ifstream& file) {
    std::string line;
    
    // Поиск секции $Nodes
    while (std::getline(file, line)) {
        if (line.find("$Nodes") != std::string::npos) {
            break;
        }
    }
    
    // Количество узлов
    std::getline(file, line);
    size_t numNodes = std::stoul(line);
    
    nodes_.reserve(numNodes);
    nodeIndexMap_.reserve(numNodes);
    
    for (size_t i = 0; i < numNodes; ++i) {
        std::getline(file, line);
        std::istringstream iss(line);
        
        int id;
        double x, y, z;
        iss >> id >> x >> y >> z;
        
        nodes_.emplace_back(id, x, y);
        nodeIndexMap_[id] = static_cast<int>(nodes_.size()) - 1;
    }
    
    std::getline(file, line); // $EndNodes
    return true;
}

bool Mesh::parseElements(std::ifstream& file) {
    std::string line;
    
    // Поиск секции $Elements
    while (std::getline(file, line)) {
        if (line.find("$Elements") != std::string::npos) {
            break;
        }
    }
    
    // Количество элементов
    std::getline(file, line);
    size_t numElements = std::stoul(line);
    
    elements_.reserve(numElements);
    
    for (size_t i = 0; i < numElements; ++i) {
        std::getline(file, line);
        std::istringstream iss(line);
        
        int id, elementType, numTags;
        iss >> id >> elementType >> numTags;
        
        // Пропускаем теги
        for (int j = 0; j < numTags; ++j) {
            int tag;
            iss >> tag;
        }
        
        // Тип 2 = треугольник в GMSH
        if (elementType == 2) {
            std::vector<int> nodeIds(3);
            for (int j = 0; j < 3; ++j) {
                iss >> nodeIds[j];
            }
            elements_.emplace_back(id, nodeIds);
        }
    }
    
    std::getline(file, line); // $EndElements
    return true;
}

bool Mesh::parsePhysicalGroups(std::ifstream& file) {
    std::string line;
    
    // Поиск секции $PhysicalNames (опционально)
    while (std::getline(file, line)) {
        if (line.find("$PhysicalNames") != std::string::npos) {
            std::getline(file, line); // количество
            size_t numGroups = std::stoul(line);
            
            for (size_t i = 0; i < numGroups; ++i) {
                std::getline(file, line);
                std::istringstream iss(line);
                
                int dim, tag;
                std::string name;
                iss >> dim >> tag;
                
                // Извлечение имени в кавычках
                size_t firstQuote = line.find('"');
                size_t secondQuote = line.find('"', firstQuote + 1);
                if (firstQuote != std::string::npos && secondQuote != std::string::npos) {
                    name = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
                }
                
                PhysicalGroup group;
                group.name = name;
                group.tag = tag;
                group.dimension = dim;
                physicalGroups_[name] = group;
            }
            
            std::getline(file, line); // $EndPhysicalNames
            break;
        }
        if (line.find("$EndElements") != std::string::npos) {
            break;
        }
    }
    
    return true;
}

Node* Mesh::getNodeById(int id) {
    auto it = nodeIndexMap_.find(id);
    if (it != nodeIndexMap_.end()) {
        return &nodes_[it->second];
    }
    return nullptr;
}

const Node* Mesh::getNodeById(int id) const {
    auto it = nodeIndexMap_.find(id);
    if (it != nodeIndexMap_.end()) {
        return &nodes_[it->second];
    }
    return nullptr;
}

std::vector<int> Mesh::findElementsContainingNode(int nodeId) const {
    std::vector<int> elementIds;
    for (size_t i = 0; i < elements_.size(); ++i) {
        const auto& elem = elements_[i];
        const auto& nodeIds = elem.getNodeIds();
        if (std::find(nodeIds.begin(), nodeIds.end(), nodeId) != nodeIds.end()) {
            elementIds.push_back(static_cast<int>(i));
        }
    }
    return elementIds;
}

std::vector<int> Mesh::getBoundaryNodeIds(const std::string& groupName) const {
    std::vector<int> boundaryNodes;
    
    auto it = physicalGroups_.find(groupName);
    if (it == physicalGroups_.end()) {
        return boundaryNodes;
    }
    
    // Для границ (dimension = 1) нужно найти узлы на этих рёбрах
    // В упрощённой версии возвращаем все узлы элементов на границе
    for (const auto& elem : elements_) {
        const auto& nodeIds = elem.getNodeIds();
        for (int nodeId : nodeIds) {
            if (std::find(boundaryNodes.begin(), boundaryNodes.end(), nodeId) 
                == boundaryNodes.end()) {
                boundaryNodes.push_back(nodeId);
            }
        }
    }
    
    return boundaryNodes;
}

void Mesh::computeElementGeometries() {
    for (auto& element : elements_) {
        element.computeGeometry(nodes_);
    }
}

const std::unordered_map<std::string, PhysicalGroup>& Mesh::getPhysicalGroups() const {
    return physicalGroups_;
}

} // namespace fem
