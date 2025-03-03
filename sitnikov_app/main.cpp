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
        params.eccentricity = config["initial_values"]["eccentricity"].as<float>();
        params.major_axis   = config["initial_values"]["major_axis"].as<float>();
        params.z_start      = config["initial_values"]["z_start"].as<float>();
        params.z_dot_start  = config["initial_values"]["z_dot_start"].as<float>();
        params.periods      = config["initial_values"]["periods"].as<float>();

        params.time_delta = config["solver_params"]["time_delta"].as<float>();

        si::sitnikov solver(params);

        std::cout << std::fixed << std::setprecision(10) << solver.current().z << " " << solver.current().z_dot << '\n';
        while (!solver.solved()) {
            solver.step();
            si::sitnikov_solution sol = solver.current();
            std::cout << std::fixed << std::setprecision(10) << sol.z << " " << sol.z_dot << '\n';
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cout << "Internal error: " << ex.what() << std::endl;
    }
}
