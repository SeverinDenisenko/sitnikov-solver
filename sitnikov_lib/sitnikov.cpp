#include "sitnikov.hpp"

#include "adams_iterative_predictor_corrector_solver.hpp"
#include "boole_intergrator.hpp"
#include "full_pivot_gauss_solver.hpp"
#include "jacoby_matrix_evaluators.hpp"
#include "newton_root_finder.hpp"
#include "rk4_solver.hpp"

namespace si {

sitnikov_solver::sitnikov_solver(sitnikov_params_t params)
    : params_(params)
{
    odes::ode_t ode = [e = params_.eccentricity]([[maybe_unused]] odes::real_t t, odes::vector_t x) {
        odes::real_t z  = x[0];
        odes::real_t v  = x[1];
        odes::real_t r  = 0.5 * (1 - e * cos(t * 2 * mpfr::const_pi()));
        odes::real_t dz = 2 * r * v;
        odes::real_t dv = -2 * r * z / pow((r * r + z * z), (3 / 2));
        return odes::make_vector({ dz, dv });
    };

    odes::ode_params_t ode_params { .t0  = 0.0,
                                    .t1  = params_.periods * 2 * mpfr::const_pi(),
                                    .dt  = params_.time_delta,
                                    .x0  = odes::make_vector({ params_.z_start, params_.z_dot_start }),
                                    .ode = ode };

    odes::integer_t order = 3;
    odes::boole_integrator_params_t integrator_params { .order = 1'000 };

    odes::adams_interpolation_coefficients interpolation_coefficients(odes::adams_interpolation_coefficients_params_t {
        .order = order, .integrator = std::make_unique<odes::boole_integrator>(integrator_params) });

    odes::adams_extrapolation_coefficients extrapolation_coefficients(odes::adams_extrapolation_coefficients_params_t {
        .order = order, .integrator = std::make_unique<odes::boole_integrator>(integrator_params) });

    odes::adams_iterative_predictor_corrector_solver_params_t solver_params {
        .order                      = order,
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
    return !(solver_->current_time() < solver_->end_time());
}

sitnikov_solution sitnikov_solver::current() const
{
    odes::vector_t sol = solver_->current();
    return sitnikov_solution { .t = solver_->current_time(), .z = sol[0], .z_dot = sol[1] };
}

} // namespace si
