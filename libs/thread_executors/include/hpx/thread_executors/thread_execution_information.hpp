//  Copyright (c) 2017-2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/async_base/traits/is_launch_policy.hpp>
#include <hpx/runtime/get_os_thread_count.hpp>
#include <hpx/threading_base/scheduler_mode.hpp>
#include <hpx/topology/topology.hpp>

#include <hpx/execution/executors/execution_information.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace threads {
    template <typename Executor, typename Parameters>
    typename std::enable_if<hpx::traits::is_threads_executor<Executor>::value,
        std::size_t>::type
    processing_units_count(Parameters&&, Executor&& exec)
    {
        return hpx::get_os_thread_count(exec);
    }

    template <typename Executor>
    typename std::enable_if<hpx::traits::is_threads_executor<Executor>::value,
        bool>::type
    has_pending_closures(Executor&& exec)
    {
        return exec.num_pending_closures() ? true : false;
    }

    template <typename Executor>
    typename std::enable_if<hpx::traits::is_threads_executor<Executor>::value,
        threads::mask_cref_type>::type
    get_pu_mask(
        Executor&& exec, threads::topology& topo, std::size_t thread_num)
    {
        return exec.get_pu_mask(topo, thread_num);
    }

    template <typename Executor, typename Mode>
    typename std::enable_if<
        hpx::traits::is_threads_executor<Executor>::value>::type
    set_scheduler_mode(Executor&& exec, Mode mode)
    {
        exec.set_scheduler_mode(
            static_cast<threads::policies::scheduler_mode>(mode));
    }
}}    // namespace hpx::threads
