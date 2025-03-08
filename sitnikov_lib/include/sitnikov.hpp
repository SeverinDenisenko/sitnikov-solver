#pragma once

#include "solver_interface.hpp"
#include <limits>

namespace si {

struct sitnikov_params_t {
    odes::real_t eccentricity;
    odes::real_t z_start;
    odes::real_t z_dot_start;
    odes::real_t periods;

    odes::real_t time_delta;

    // conditions to break out of integration loop
    odes::real_t z_min     = -std::numeric_limits<odes::real_t>::infinity();
    odes::real_t z_max     = std::numeric_limits<odes::real_t>::infinity();
    odes::real_t z_dot_min = -std::numeric_limits<odes::real_t>::infinity();
    odes::real_t z_dot_max = std::numeric_limits<odes::real_t>::infinity();
};

struct sitnikov_solution {
    odes::real_t t;
    odes::real_t z;
    odes::real_t z_dot;
};

class sitnikov_solver {
public:
    sitnikov_solver(sitnikov_params_t params);

    void step();
    bool solved() const;
    sitnikov_solution current() const;

private:
    sitnikov_params_t params_;
    odes::uptr<odes::isolver> solver_;
};

} // namespace si
