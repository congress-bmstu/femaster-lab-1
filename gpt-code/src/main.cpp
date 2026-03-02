#include <iostream>
#include "problem_config.h"
#include "mesh.h"
#include "boundary_condition.h"
#include "fem_solver.h"
#include "vtk_writer.h"
#include "utils.h"

using namespace fem;

int main(int argc, char* argv[]) {
    Logger::info("=== FEM Solver for Irrotational Flow ===");
    
    // Определение пути к конфигурации
    std::string configFile = "input.yaml";
    if (argc > 1) {
        configFile = argv[1];
    }
    
    // Загрузка конфигурации
    Logger::info("Loading configuration from: " + configFile);
    ProblemConfig config;
    if (!config.loadFromYaml(configFile)) {
        Logger::error("Failed to load configuration file");
        return 1;
    }
    
    if (!config.validate()) {
        Logger::error("Configuration validation failed");
        return 1;
    }
    
    Logger::success("Configuration loaded successfully");
    
    // Загрузка сетки
    Logger::info("Loading mesh from: " + config.getMeshFile());
    Mesh mesh;
    if (!mesh.loadFromGmsh(config.getMeshFile())) {
        Logger::error("Failed to load mesh file");
        return 1;
    }
    Logger::success("Mesh loaded: " + std::to_string(mesh.getNumNodes()) + 
                   " nodes, " + std::to_string(mesh.getNumElements()) + " elements");
    
    // Вычисление геометрии элементов
    mesh.computeElementGeometries();
    
    // Применение граничных условий
    Logger::info("Applying boundary conditions");
    BoundaryConditionManager bcManager;
    for (const auto& bcConfig : config.getBCConfigs()) {
        bcManager.addBCConfig(bcConfig);
    }
    bcManager.applyToMesh(mesh, config.getUInf(), config.getH());
    Logger::success("Boundary conditions applied");
    
    // Инициализация решателя
    Logger::info("Initializing FEM solver");
    FEMSolver solver;
    solver.initialize(mesh, config);
    
    // Сборка матрицы
    Logger::info("Assembling stiffness matrix");
    Timer assemblyTimer;
    solver.assembleStiffnessMatrix();
    Logger::success("Matrix assembled in " + 
                   std::to_string(assemblyTimer.elapsed()) + " s");
    
    // Применение ГУ Дирихле
    solver.applyDirichletBoundaryConditions(bcManager);
    
    // Решение СЛАУ
    Logger::info("Solving system of equations");
    Timer solveTimer;
    SolutionResult result = solver.solve();
    
    if (!result.success) {
        Logger::error("Solver failed");
        return 1;
    }
    
    Logger::success("Solution found in " + std::to_string(result.solveTime) + 
                   " s (" + std::to_string(result.iterations) + " iterations)");
    
    // Вычисление скоростей
    Logger::info("Computing velocity field");
    solver.computeVelocities(mesh);
    
    // Экспорт результатов
    Logger::info("Writing results to: " + config.getOutputFile());
    VTKWriter writer;
    if (!writer.writeVTU(config.getOutputFile(), mesh, solver)) {
        Logger::error("Failed to write output file");
        return 1;
    }
    Logger::success("Results written successfully");
    
    // Вывод статистики
    Logger::info("=== Solution Statistics ===");
    Logger::info("DOF: " + std::to_string(solver.getNumDOF()));
    Logger::info("Residual: " + std::to_string(result.residual));
    Logger::info("Total time: " + 
                std::to_string(assemblyTimer.elapsed() + result.solveTime) + " s");
    
    Logger::success("=== Computation Complete ===");
    
    return 0;
}
