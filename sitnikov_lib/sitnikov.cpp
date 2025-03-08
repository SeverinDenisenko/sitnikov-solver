#include "sitnikov.hpp"

#include "adams_iterative_predictor_corrector_solver.hpp"
#include "boole_intergrator.hpp"
#include "rk4_solver.hpp"

namespace si {

sitnikov_solver::sitnikov_solver(sitnikov_params_t params)
    : params_(params)
{
    std::function<odes::real_t(odes::real_t)> solve_kepler_equation = [e = params_.eccentricity](odes::real_t t) {
        odes::real_t E0 = t;
        odes::real_t E;

        if (t == 0.0) {
            return odes::real_t(0.0);
        }

        while (true) {
            E = t + e * sin(E0);

            if (std::abs(E - E0) <= 1e-10) {
                break;
            }

            E0 = E;
        }

        return E;
    };

    odes::ode_t ode
        = [e = params_.eccentricity, solve_kepler_equation]([[maybe_unused]] odes::real_t t, odes::vector_t x) {
              odes::real_t z  = x[0];
              odes::real_t v  = x[1];
              odes::real_t E  = solve_kepler_equation(t);
              odes::real_t r  = (1.0 - e * cos(E)) / 2.0;
              odes::real_t dz = v;
              odes::real_t dv = -z / pow((r * r + z * z), (3.0 / 2.0));
              return odes::make_vector({ dz, dv });
          };

    odes::ode_params_t ode_params { .t0  = 0.0,
                                    .t1  = params_.periods * 2 * M_PI,
                                    .dt  = params_.time_delta,
                                    .x0  = odes::make_vector({ params_.z_start, params_.z_dot_start }),
                                    .ode = ode };

    odes::integer_t order = 4;
    odes::boole_integrator_params_t integrator_params { .order = 1'000 };

    odes::adams_interpolation_coefficients interpolation_coefficients(odes::adams_interpolation_coefficients_params_t {
        .order = order, .integrator = std::make_unique<odes::boole_integrator>(integrator_params) });

    odes::adams_extrapolation_coefficients extrapolation_coefficients(odes::adams_extrapolation_coefficients_params_t {
        .order = order, .integrator = std::make_unique<odes::boole_integrator>(integrator_params) });

    odes::adams_iterative_predictor_corrector_solver_params_t solver_params {
        .order                      = order,
        .iterations                 = 3,
        .interpolation_coefficients = std::move(interpolation_coefficients),
        .extrapolation_coefficients = std::move(extrapolation_coefficients),
        .initial_solver             = std::make_unique<odes::rk4_solver>(ode_params, odes::rk4_solver_params_t {})
    };

    solver_ = std::make_unique<odes::adams_iterative_predictor_corrector_solver>(ode_params, std::move(solver_params));
}

void sitnikov_solver::step()
{
    solver_->step();
}

bool sitnikov_solver::solved() const
{
    auto current   = solver_->current();
    bool in_bounds = current[0] > params_.z_min && current[0] < params_.z_max && current[1] > params_.z_dot_min
        && current[1] < params_.z_dot_max;
    return !(solver_->current_time() < solver_->end_time()) || !in_bounds;
}

sitnikov_solution sitnikov_solver::current() const
{
    odes::vector_t sol = solver_->current();
    return sitnikov_solution { .t = solver_->current_time(), .z = sol[0], .z_dot = sol[1] };
}

} // namespace si
