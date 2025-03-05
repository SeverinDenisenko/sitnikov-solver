#include <iomanip>
#include <iostream>

#include "yaml-cpp/yaml.h"

#include "sitnikov.hpp"

int main()
{
    try {
        YAML::Node config = YAML::LoadFile("config.yaml");

        odes::set_precision(25);

        si::sitnikov_params_t params;
        params.eccentricity = config["initial_values"]["eccentricity"].as<float>();
        params.z_start      = config["initial_values"]["z_start"].as<float>();
        params.z_dot_start  = config["initial_values"]["z_dot_start"].as<float>();
        params.periods      = config["initial_values"]["periods"].as<float>();

        params.time_delta = config["solver_params"]["time_delta"].as<float>();

        odes::integer_t skip_n_outs = config["solver_params"]["skip_n_outs"].as<odes::integer_t>();

        si::sitnikov_solver solver(params);

        si::sitnikov_solution sol = solver.current();
        std::cout << std::fixed << std::setprecision(10) << sol.t << " " << sol.z << " " << sol.z_dot << '\n';
        odes::integer_t counter = 0;
        while (!solver.solved()) {
            solver.step();
            ++counter;
            if (counter % skip_n_outs == 0) {
                si::sitnikov_solution sol = solver.current();
                std::cout << std::fixed << std::setprecision(10) << sol.t << " " << sol.z << " " << sol.z_dot << '\n';
            }
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cout << "Internal error: " << ex.what() << std::endl;
    }
}
