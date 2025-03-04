#include <iomanip>
#include <iostream>

#include "yaml-cpp/yaml.h"

#include "sitnikov.hpp"

int main()
{
    try {
        YAML::Node config = YAML::LoadFile("config.yaml");

        odes::set_precision(50);

        si::sitnikov_params_t params;
        params.eccentricity = config["poincare_portrait"]["eccentricity"].as<float>();
        params.z_start      = 0.0;
        params.z_dot_start  = 0.0;
        params.periods      = config["poincare_portrait"]["periods"].as<float>();
        params.time_delta   = config["solver_params"]["time_delta"].as<float>();

        odes::real_t z_min  = config["poincare_portrait"]["z_min"].as<float>();
        odes::real_t z_max  = config["poincare_portrait"]["z_max"].as<float>();
        odes::real_t z_step = config["poincare_portrait"]["z_step"].as<float>();

        for (odes::real_t z = z_min; z < z_max; z += z_step) {
            params.z_start = z;

            si::sitnikov_solver solver(params);

            si::sitnikov_solution sol = solver.current();
            std::cout << std::fixed << std::setprecision(10) << sol.t << " " << sol.z << " " << sol.z_dot << '\n';
            while (!solver.solved()) {
                solver.step();
                si::sitnikov_solution sol = solver.current();
                std::cout << std::fixed << std::setprecision(10) << sol.t << " " << sol.z << " " << sol.z_dot << '\n';
            }
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cout << "Internal error: " << ex.what() << std::endl;
    }
}
