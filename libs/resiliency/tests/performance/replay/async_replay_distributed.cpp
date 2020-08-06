//  Copyright (c) 2019 National Technology & Engineering Solutions of Sandia,
//                     LLC (NTESS).
//  Copyright (c) 2018-2019 Hartmut Kaiser
//  Copyright (c) 2018-2019 Adrian Serio
//  Copyright (c) 2019-2020 Nikunj Gupta
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/actions_base/plain_action.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/include/runtime.hpp>
#include <hpx/modules/futures.hpp>
#include <hpx/modules/resiliency.hpp>
#include <hpx/modules/testing.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

int universal_ans(std::vector<hpx::id_type> f_locales, std::size_t size)
{
    // Pretending to do some useful work
    std::size_t start = hpx::util::high_resolution_clock::now();

    while ((hpx::util::high_resolution_clock::now() - start) < (size * 1e3)) {}

    // Check if the node is faulty
    for (const auto& locale : f_locales)
    {
        // Throw a runtime error in case the node is faulty
        if (locale == hpx::find_here())
            throw std::runtime_error("runtime error occured.");
    }

    return 42;
}

HPX_PLAIN_ACTION(universal_ans, universal_action);

bool validate(int ans)
{
    return ans == 42;
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    std::size_t f_nodes = vm["f-nodes"].as<std::size_t>();
    std::size_t size = vm["size"].as<std::size_t>();
    std::size_t num_tasks = vm["num-tasks"].as<std::size_t>();

    universal_action ac;
    std::vector<hpx::id_type> locales = hpx::find_all_localities();

    // Make sure that the number of faulty nodes are less than the number of
    // localities we work on.
    assert(f_nodes < locales.size());

    // List of faulty nodes
    std::vector<hpx::id_type> f_locales;
    std::vector<std::size_t> visited;

    // Mark nodes as faulty
    for (std::size_t i = 0; i < f_nodes; ++i)
    {
        std::size_t num = std::rand() % locales.size();
        while (visited.end() != std::find(visited.begin(), visited.end(), num))
        {
            num = std::rand() % locales.size();
        }

        f_locales.push_back(locales.at(num));
    }

    {
        hpx::util::high_resolution_timer t;

        std::vector<hpx::future<int>> tasks;
        for (std::size_t i = 0; i < num_tasks; ++i)
        {
            tasks.push_back(hpx::resiliency::experimental::async_replay(
                locales, ac, f_locales, size));

            std::rotate(locales.begin(), locales.begin() + 1, locales.end());
        }

        hpx::wait_all(tasks);

        double elapsed = t.elapsed();
        std::cout << "Replay: " << elapsed << std::endl;
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    // Configure application-specific options
    hpx::program_options::options_description desc_commandline;

    desc_commandline.add_options()("f-nodes",
        hpx::program_options::value<std::size_t>()->default_value(1),
        "Number of faulty nodes to be injected")("size",
        hpx::program_options::value<std::size_t>()->default_value(2000),
        "Grain size of a task")("num-tasks",
        hpx::program_options::value<std::size_t>()->default_value(1000000),
        "Number of tasks to invoke");

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}
