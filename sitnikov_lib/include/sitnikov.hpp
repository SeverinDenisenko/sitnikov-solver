#pragma once

#include "solver_interface.hpp"

namespace si {

struct sitnikov_params_t {
    odes::real_t eccentricity;
    odes::real_t major_axis;
    odes::real_t z_start;
    odes::real_t z_dot_start;
    odes::real_t periods;

    odes::real_t time_delta;
};

struct sitnikov_solution {
    odes::real_t z;
    odes::real_t z_dot;
};

class sitnikov {
public:
    sitnikov(sitnikov_params_t params);

    void step();
    bool solved() const;
    sitnikov_solution current() const;

private:
    sitnikov_params_t params_;
    odes::uptr<odes::isolver> solver_;
};

} // namespace si
