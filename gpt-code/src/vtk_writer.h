#pragma once

#include <string>
#include "mesh.h"
#include "fem_solver.h"

namespace fem {

/**
 * @brief Класс экспорта результатов в формат VTK для ParaView
 * 
 * Создаёт файлы в формате VTK Legacy или VTU для визуализации
 */
class VTKWriter {
public:
    VTKWriter() = default;
    
    // Экспорт в формат VTK Legacy (.vtk)
    bool writeVTKLegacy(
        const std::string& filename,
        const Mesh& mesh,
        const FEMSolver& solver);
    
    // Экспорт в формат VTU (XML, рекомендуется для ParaView)
    bool writeVTU(
        const std::string& filename,
        const Mesh& mesh,
        const FEMSolver& solver);
    
    // Экспорт только функции тока
    bool writePsiField(
        const std::string& filename,
        const Mesh& mesh,
        const Eigen::VectorXd& psi);
    
private:
    // Запись заголовка VTK файла
    void writeVTKHeader(std::ofstream& file, size_t numNodes, size_t numElements);
    
    // Запись координат узлов
    void writeNodeCoordinates(std::ofstream& file, const Mesh& mesh);
    
    // Запись элементов (треугольники)
    void writeElements(std::ofstream& file, const Mesh& mesh);
    
    // Запись данных в узлах (psi, vx, vy)
    void writeNodeData(std::ofstream& file, const FEMSolver& solver);
    
    // Запись данных в элементах (скорости, градиенты)
    void writeCellData(std::ofstream& file, const Mesh& mesh, const FEMSolver& solver);
};

} // namespace fem

