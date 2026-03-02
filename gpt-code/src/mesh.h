#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "node.h"
#include "element.h"

namespace fem {

/**
 * @brief Информация о физической группе из GMSH
 */
struct PhysicalGroup {
    std::string name;           ///< Имя группы (Inlet, Outlet, Walls, etc.)
    int tag;                    ///< Тег группы
    int dimension;              ///< Размерность (1 для границ, 2 для области)
    std::vector<int> entityIds; ///< ID сущностей в группе
};

/**
 * @brief Класс сетки конечных элементов
 * 
 * Загружает сетку из формата GMSH (.msh), хранит узлы и элементы,
 * предоставляет доступ к физическим группам для применения ГУ
 */
class Mesh {
public:
    Mesh() = default;
    
    // Загрузка сетки из файла GMSH
    bool loadFromGmsh(const std::string& filename);
    
    // Геттеры
    const std::vector<Node>& getNodes() const { return nodes_; }
    const std::vector<Element>& getElements() const { return elements_; }
    const std::unordered_map<std::string, PhysicalGroup>& getPhysicalGroups() const;
    
    // Поиск узла по ID
    Node* getNodeById(int id);
    const Node* getNodeById(int id) const;
    
    // Поиск элементов, содержащих узел
    std::vector<int> findElementsContainingNode(int nodeId) const;
    
    // Получение узлов на границе по имени физической группы
    std::vector<int> getBoundaryNodeIds(const std::string& groupName) const;
    
    // Статистика сетки
    size_t getNumNodes() const { return nodes_.size(); }
    size_t getNumElements() const { return elements_.size(); }
    
    // Вычисление геометрических характеристик всех элементов
    void computeElementGeometries();
    
private:
    std::vector<Node> nodes_;                       ///< Все узлы сетки
    std::vector<Element> elements_;                 ///< Все элементы
    std::unordered_map<int, int> nodeIndexMap_;     ///< ID узла -> индекс в векторе
    std::unordered_map<std::string, PhysicalGroup> physicalGroups_; ///< Физические группы
    
    // Парсинг файла GMSH
    bool parseGmshVersion(std::ifstream& file);
    bool parseNodes(std::ifstream& file);
    bool parseElements(std::ifstream& file);
    bool parsePhysicalGroups(std::ifstream& file);
};

} // namespace fem

