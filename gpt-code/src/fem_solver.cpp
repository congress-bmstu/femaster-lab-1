#include "fem_solver.h"
#include <Eigen/SparseCholesky>
#include <chrono>

namespace fem {

void FEMSolver::initialize(Mesh& mesh, const ProblemConfig& config) {
    mesh_ = &mesh;
    config_ = &config;
    
    numDOF_ = mesh.getNumNodes();
    
    psi_.resize(numDOF_);
    vx_.resize(numDOF_);
    vy_.resize(numDOF_);
    
    F_ = Eigen::VectorXd::Zero(numDOF_);
}

void FEMSolver::assembleStiffnessMatrix() {
    if (!mesh_ || !config_) return;
    
    const auto& nodes = mesh_->getNodes();
    const auto& elements = mesh_->getElements();
    
    // Подсчёт ненулевых элементов для эффективного создания sparse матрицы
    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(elements.size() * 9);  // 3x3 матрица на элемент
    
    for (const auto& element : elements) {
        assembleElementContribution(element, nodes);
        
        // Получение локальной матрицы
        Eigen::Matrix3d localK = element.computeLocalStiffnessMatrix(nodes);
        const auto& nodeIds = element.getNodeIds();
        
        // Преобразование ID узлов в индексы
        std::vector<int> indices(3);
        for (int i = 0; i < 3; ++i) {
            indices[i] = nodeIds[i] - 1;  // GMSH ID начинается с 1
        }
        
        // Добавление в глобальную матрицу
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                triplets.emplace_back(indices[i], indices[j], localK(i, j));
            }
        }
    }
    
    K_.resize(numDOF_, numDOF_);
    K_.setFromTriplets(triplets.begin(), triplets.end());
    K_.makeCompressed();
}

void FEMSolver::assembleElementContribution(
    const Element& element,
    const std::vector<Node>& nodes) {
    // В данной задаке вектор нагрузки F = 0 (уравнение Лапласа)
    // Метод оставлен для расширения на другие задачи
}

void FEMSolver::applyDirichletBoundaryConditions(
    const BoundaryConditionManager& bcManager) {
    
    const auto& dirichletNodes = bcManager.getDirichletNodes();
    
    // Модификация матрицы и вектора для условий Дирихле
    // Метод штрафных коэффициентов или прямая модификация
    
    for (int nodeId : dirichletNodes) {
        int idx = nodeId - 1;  // Преобразование ID в индекс
        double value = bcManager.getDirichletValue(nodeId);
        
        // Сохраняем значение для последующего использования
        psi_(idx) = value;
        
        // Прямая модификация строки и столбца матрицы
        // K[idx, :] = 0, K[:, idx] = 0, K[idx, idx] = 1
        // F[idx] = value
        
        // Для sparse матрицы используем другой подход
        // Добавляем большую величину на диагональ
        double penalty = 1e20;
        K_.coeffRef(idx, idx) += penalty;
        F_(idx) += penalty * value;
    }
}

SolutionResult FEMSolver::solve() {
    SolutionResult result;
    
    Timer timer;
    
    // Выбор решателя
    if (config_->getSolverConfig().solverType == "Direct" ||
        config_->getSolverConfig().solverType == "SimplicialLDLT") {
        
        Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
        solver.compute(K_);
        
        if (solver.info() != Eigen::Success) {
            result.success = false;
            return result;
        }
        
        psi_ = solver.solve(F_);
        result.success = (solver.info() == Eigen::Success);
        result.iterations = 1;  // Прямой метод
        
    } else {
        // Итерационный метод (CG)
        Eigen::ConjugateGradient<
            Eigen::SparseMatrix<double>,
            Eigen::Lower | Eigen::Upper
        > solver;
        
        solver.setMaxIterations(config_->getSolverConfig().maxIterations);
        solver.setTolerance(config_->getSolverConfig().tolerance);
        
        solver.compute(K_);
        psi_ = solver.solve(F_);
        
        result.success = (solver.info() == Eigen::Success);
        result.iterations = solver.iterations();
        result.residual = solver.error();
    }
    
    result.solveTime = timer.elapsed();
    
    // Вычисление числа обусловленности (приближённо)
    // Для больших матриц это дорого, поэтому опционально
    
    return result;
}

void FEMSolver::computeVelocities(const Mesh& mesh) {
    const auto& nodes = mesh.getNodes();
    const auto& elements = mesh.getElements();
    
    vx_.setZero();
    vy_.setZero();
    
    std::vector<int> nodeCount(numDOF_, 0);
    
    // Вычисление скоростей через производные функции тока
    // vx = ∂ψ/∂y, vy = -∂ψ/∂x
    for (const auto& element : elements) {
        computeElementVelocities(element, nodes);
        
        const auto& nodeIds = element.getNodeIds();
        Eigen::Matrix<double, 2, 3> gradN = 
            element.computeShapeFunctionGradients(nodes);
        
        // Значения ψ в узлах элемента
        Eigen::Vector3d psiElem;
        for (int i = 0; i < 3; ++i) {
            psiElem(i) = psi_(nodeIds[i] - 1);
        }
        
        // Градиент ψ в элементе
        double dpsi_dx = 0.0, dpsi_dy = 0.0;
        for (int i = 0; i < 3; ++i) {
            dpsi_dx += gradN(0, i) * psiElem(i);
            dpsi_dy += gradN(1, i) * psiElem(i);
        }
        
        // Скорости: vx = ∂ψ/∂y, vy = -∂ψ/∂x
        double vx_elem = dpsi_dy;
        double vy_elem = -dpsi_dx;
        
        // Распределение по узлам (усреднение)
        for (int i = 0; i < 3; ++i) {
            int idx = nodeIds[i] - 1;
            vx_(idx) += vx_elem;
            vy_(idx) += vy_elem;
            nodeCount[idx]++;
        }
    }
    
    // Усреднение по узлам
    for (size_t i = 0; i < numDOF_; ++i) {
        if (nodeCount[i] > 0) {
            vx_(i) /= nodeCount[i];
            vy_(i) /= nodeCount[i];
        }
    }
}

void FEMSolver::computeElementVelocities(
    const Element& element,
    const std::vector<Node>& nodes) {
    // Метод для расширения (вычисление в центрах элементов)
}

} // namespace fem
