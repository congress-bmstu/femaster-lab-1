#include "vtk_writer.h"
#include <fstream>
#include <iomanip>

namespace fem {

bool VTKWriter::writeVTKLegacy(
    const std::string& filename,
    const Mesh& mesh,
    const FEMSolver& solver) {
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    writeVTKHeader(file, mesh.getNumNodes(), mesh.getNumElements());
    writeNodeCoordinates(file, mesh);
    writeElements(file, mesh);
    writeNodeData(file, solver);
    
    file.close();
    return true;
}

bool VTKWriter::writeVTU(
    const std::string& filename,
    const Mesh& mesh,
    const FEMSolver& solver) {
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // XML формат VTU
    file << "<?xml version=\"1.0\"?>\n";
    file << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n";
    file << "  <UnstructuredGrid>\n";
    
    size_t numNodes = mesh.getNumNodes();
    size_t numElements = mesh.getNumElements();
    
    // Points
    file << "    <Piece NumberOfPoints=\"" << numNodes 
         << "\" NumberOfCells=\"" << numElements << "\">\n";
    file << "      <Points>\n";
    file << "        <DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">\n";
    file << "          ";
    
    const auto& nodes = mesh.getNodes();
    for (const auto& node : nodes) {
        file << std::setprecision(10) << node.getX() << " " 
             << node.getY() << " 0.0 ";
    }
    file << "\n";
    file << "        </DataArray>\n";
    file << "      </Points>\n";
    
    // Cells (Triangles)
    file << "      <Cells>\n";
    file << "        <DataArray type=\"Int64\" Name=\"connectivity\" format=\"ascii\">\n";
    file << "          ";
    
    const auto& elements = mesh.getElements();
    for (const auto& elem : elements) {
        const auto& nodeIds = elem.getNodeIds();
        for (int id : nodeIds) {
            file << (id - 1) << " ";  // VTU использует 0-based indexing
        }
    }
    file << "\n";
    file << "        </DataArray>\n";
    
    file << "        <DataArray type=\"Int64\" Name=\"offsets\" format=\"ascii\">\n";
    file << "          ";
    for (size_t i = 1; i <= numElements; ++i) {
        file << (i * 3) << " ";
    }
    file << "\n";
    file << "        </DataArray>\n";
    
    file << "        <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n";
    file << "          ";
    for (size_t i = 0; i < numElements; ++i) {
        file << "5 ";  // 5 = VTK_TRIANGLE
    }
    file << "\n";
    file << "        </DataArray>\n";
    file << "      </Cells>\n";
    
    // Point Data (Psi, Velocity)
    file << "      <PointData>\n";
    
    // Psi
    file << "        <DataArray type=\"Float64\" Name=\"Psi\" format=\"ascii\">\n";
    file << "          ";
    const auto& psi = solver.getSolution();
    for (size_t i = 0; i < numNodes; ++i) {
        file << std::setprecision(10) << psi(i) << " ";
    }
    file << "\n";
    file << "        </DataArray>\n";
    
    // Velocity magnitude
    file << "        <DataArray type=\"Float64\" Name=\"Velocity\" format=\"ascii\">\n";
    file << "          ";
    const auto& vx = solver.getVx();
    const auto& vy = solver.getVy();
    for (size_t i = 0; i < numNodes; ++i) {
        double vel = std::sqrt(vx(i)*vx(i) + vy(i)*vy(i));
        file << std::setprecision(10) << vel << " ";
    }
    file << "\n";
    file << "        </DataArray>\n";
    
    file << "      </PointData>\n";
    file << "    </Piece>\n";
    file << "  </UnstructuredGrid>\n";
    file << "</VTKFile>\n";
    
    file.close();
    return true;
}

void VTKWriter::writeVTKHeader(
    std::ofstream& file,
    size_t numNodes,
    size_t numElements) {
    
    file << "# vtk DataFile Version 3.0\n";
    file << "FEM Solution - Stream Function\n";
    file << "ASCII\n";
    file << "DATASET UNSTRUCTURED_GRID\n";
    file << "POINTS " << numNodes << " float\n";
}

void VTKWriter::writeNodeCoordinates(std::ofstream& file, const Mesh& mesh) {
    const auto& nodes = mesh.getNodes();
    for (const auto& node : nodes) {
        file << std::setprecision(10) << node.getX() << " " 
             << node.getY() << " 0.0\n";
    }
}

void VTKWriter::writeElements(std::ofstream& file, const Mesh& mesh) {
    file << "CELLS " << mesh.getNumElements() << " " 
         << (mesh.getNumElements() * 4) << "\n";
    
    const auto& elements = mesh.getElements();
    for (const auto& elem : elements) {
        const auto& nodeIds = elem.getNodeIds();
        file << "3 ";
        for (int id : nodeIds) {
            file << (id - 1) << " ";  // 0-based indexing для VTK
        }
        file << "\n";
    }
    
    file << "CELL_TYPES " << mesh.getNumElements() << "\n";
    for (size_t i = 0; i < mesh.getNumElements(); ++i) {
        file << "5\n";  // VTK_TRIANGLE = 5
    }
}

void VTKWriter::writeNodeData(std::ofstream& file, const FEMSolver& solver) {
    file << "POINT_DATA " << solver.getNumDOF() << "\n";
    
    // Psi
    file << "SCALARS Psi float\n";
    file << "LOOKUP_TABLE default\n";
    const auto& psi = solver.getSolution();
    for (size_t i = 0; i < solver.getNumDOF(); ++i) {
        file << psi(i) << "\n";
    }
    
    // Velocity
    file << "SCALARS Velocity float\n";
    file << "LOOKUP_TABLE default\n";
    const auto& vx = solver.getVx();
    const auto& vy = solver.getVy();
    for (size_t i = 0; i < solver.getNumDOF(); ++i) {
        double vel = std::sqrt(vx(i)*vx(i) + vy(i)*vy(i));
        file << vel << "\n";
    }
    
    // Velocity vectors
    file << "VECTORS Velocity_Vectors float\n";
    for (size_t i = 0; i < solver.getNumDOF(); ++i) {
        file << vx(i) << " " << vy(i) << " 0.0\n";
    }
}

bool VTKWriter::writePsiField(
    const std::string& filename,
    const Mesh& mesh,
    const Eigen::VectorXd& psi) {
    
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    writeVTKHeader(file, mesh.getNumNodes(), mesh.getNumElements());
    writeNodeCoordinates(file, mesh);
    writeElements(file, mesh);
    
    file << "POINT_DATA " << mesh.getNumNodes() << "\n";
    file << "SCALARS Psi float\n";
    file << "LOOKUP_TABLE default\n";
    
    for (size_t i = 0; i < mesh.getNumNodes(); ++i) {
        file << psi(i) << "\n";
    }
    
    file.close();
    return true;
}

} // namespace fem
