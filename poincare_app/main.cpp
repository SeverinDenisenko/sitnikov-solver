#include <cmath>
#include <cstddef>
#include <ctime>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "types.hpp"
#include "yaml-cpp/yaml.h"

#include "sitnikov.hpp"

class threadpool {
public:
    using task_t = std::function<void(void)>;

    threadpool()
        : threads_count_(std::thread::hardware_concurrency())
        , round_robin_counter_(0)
        , tasks_(threads_count_)
        , mutexes_(threads_count_)
        , stop_signals_(threads_count_, 0)
    {
        for (size_t thread = 0; thread < threads_count_; ++thread) {
            threads_.emplace_back(std::thread([this, id = thread]() {
                task_t task = {};

                while (true) {
                    {
                        // check if we have been stoped
                        std::unique_lock lk(mutexes_[id]);
                        if (tasks_[id].empty() && stop_signals_[id] == 1) {
                            return;
                        }

                        // if queue is empty -- pass
                        // if there is task -- extract it
                        if (tasks_[id].empty()) {
                            continue;
                        } else {
                            task = tasks_[id].front();
                            tasks_[id].pop_front();
                        }
                    }

                    if (task) {
                        task();
                        task = {};
                    }
                }
            }));
        }
    }

    void put(task_t task)
    {
        size_t thread = round_robin_counter_++ % threads_count_;

        std::unique_lock lk(mutexes_[thread]);
        tasks_[thread].push_back(std::move(task));
    }

    void join()
    {
        for (size_t thread = 0; thread < threads_count_; ++thread) {
            std::unique_lock lk(mutexes_[thread]);
            stop_signals_[thread] = 1;
        }

        for (auto& thread : threads_) {
            thread.join();
        }
    }

private:
    size_t threads_count_;
    size_t round_robin_counter_;
    std::vector<std::list<task_t>> tasks_;
    std::vector<std::mutex> mutexes_;
    std::vector<size_t> stop_signals_;
    std::vector<std::thread> threads_;
};

int main()
{
    try {
        YAML::Node config = YAML::LoadFile("config.yaml");

        odes::set_precision(25);

        si::sitnikov_params_t params;
        params.eccentricity = config["poincare_portrait"]["eccentricity"].as<float>();
        params.z_start      = 0.0;
        params.z_dot_start  = 0.0;
        params.periods      = config["poincare_portrait"]["periods"].as<float>();
        params.time_delta   = config["solver_params"]["time_delta"].as<float>();

        params.z_min        = config["poincare_portrait"]["z_min"].as<float>();
        params.z_max        = config["poincare_portrait"]["z_max"].as<float>();
        odes::real_t z_step = config["poincare_portrait"]["z_step"].as<float>();

        params.z_dot_min        = config["poincare_portrait"]["z_dot_min"].as<float>();
        params.z_dot_max        = config["poincare_portrait"]["z_dot_max"].as<float>();
        odes::real_t z_dot_step = config["poincare_portrait"]["z_dot_step"].as<float>();

        odes::integer_t skip_n_outs = static_cast<odes::integer_t>(2.0 * M_PI / params.time_delta);
        odes::integer_t total_tasks = static_cast<odes::integer_t>(
            (params.z_dot_max - params.z_dot_min) / z_dot_step * (params.z_max - params.z_min) / z_step);
        odes::integer_t tasks_done = 0;

        std::mutex output_mutex;
        threadpool pool;

        for (odes::real_t z_dot = params.z_dot_min; z_dot < params.z_dot_max; z_dot += z_dot_step) {
            for (odes::real_t z = params.z_min; z < params.z_max; z += z_step) {
                pool.put([params, z, z_dot, skip_n_outs, &output_mutex, &tasks_done, total_tasks]() mutable {
                    params.z_start     = z;
                    params.z_dot_start = z_dot;

                    si::sitnikov_solver solver(params);

                    odes::integer_t counter = 0;
                    while (!solver.solved()) {
                        solver.step();
                        ++counter;
                        if (counter % skip_n_outs == 0) {
                            si::sitnikov_solution sol = solver.current();
                            {
                                std::unique_lock lk(output_mutex);
                                std::cout << std::fixed << std::setprecision(10) << sol.t << " " << sol.z << " "
                                          << sol.z_dot << std::endl;
                            }
                        }
                    }

                    {
                        std::unique_lock lk(output_mutex);
                        ++tasks_done;
                        odes::real_t done = static_cast<odes::real_t>(tasks_done) / total_tasks * 100.0;
                        std::cerr << std::fixed << std::setprecision(4) << "Done " << done << "%" << std::endl;
                    }
                });
            }
        }

        pool.join();

        return 0;
    } catch (const std::exception& ex) {
        std::cout << "Internal error: " << ex.what() << std::endl;
    }
}
