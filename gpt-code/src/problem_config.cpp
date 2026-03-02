#include "problem_config.h"
#include <fstream>
#include <sstream>
#include <yaml-cpp/yaml.h>

namespace fem {

bool ProblemConfig::loadFromYaml(const std::string& filename) {
    inputFile_ = filename;
    
    try {
        YAML::Node config = YAML::LoadFile(filename);
        
        // Основные параметры
        if (config["mesh_file"]) {
            meshFile_ = config["mesh_file"].as<std::string>();
        }
        if (config["output_file"]) {
            outputFile_ = config["output_file"].as<std::string>();
        }
        
        // Параметры задачи
        if (config["problem"]["U_inf"]) {
            U_inf_ = config["problem"]["U_inf"].as<double>();
        }
        if (config["problem"]["H"]) {
            H_ = config["problem"]["H"].as<double>();
        }
        if (config["problem"]["D"]) {
            D_ = config["problem"]["D"].as<double>();
        }
        
        // Граничные условия
        if (config["boundary_conditions"]) {
            for (const auto& bc : config["boundary_conditions"]) {
                BCConfig bcConfig;
                bcConfig.physicalGroup = bc["group"].as<std::string>();
                
                std::string type = bc["type"].as<std::string>();
                if (type == "Dirichlet") {
                    bcConfig.type = BCType::DIRICHLET;
                } else if (type == "Neumann") {
                    bcConfig.type = BCType::NEUMANN;
                } else {
                    bcConfig.type = BCType::NONE;
                }
                
                bcConfig.value = bc["value"].as<double>();
                bcConfigs_.push_back(bcConfig);
            }
        }
        
        // Настройки решателя
        if (config["solver"]["max_iterations"]) {
            solverConfig_.maxIterations = config["solver"]["max_iterations"].as<int>();
        }
        if (config["solver"]["tolerance"]) {
            solverConfig_.tolerance = config["solver"]["tolerance"].as<double>();
        }
        if (config["solver"]["type"]) {
            solverConfig_.solverType = config["solver"]["type"].as<std::string>();
        }
        
    } catch (const std::exception& e) {
        return false;
    }
    
    return true;
}

bool ProblemConfig::validate() const {
    if (meshFile_.empty()) return false;
    if (outputFile_.empty()) return false;
    if (U_inf_ <= 0) return false;
    if (H_ <= 0) return false;
    if (D_ <= 0) return false;
    if (bcConfigs_.empty()) return false;
    
    return true;
}

} // namespace fem
