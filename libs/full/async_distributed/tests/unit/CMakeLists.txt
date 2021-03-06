# Copyright (c) 2007-2017 Hartmut Kaiser
# Copyright (c) 2011-2012 Bryce Adelstein-Lelbach
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(tests
    apply_colocated
    apply_local
    apply_local_executor
    apply_remote
    apply_remote_client
    async_cb_colocated
    async_cb_remote
    async_cb_remote_client
    async_colocated
    async_continue
    async_continue_cb
    async_continue_cb_colocated
    async_continue_colocated
    async_local
    async_local_executor
    async_remote
    async_remote_client
    async_unwrap_result
    remote_dataflow
    sync_remote
)

set(apply_colocated_PARAMETERS LOCALITIES 2)
set(apply_remote_PARAMETERS LOCALITIES 2)
set(apply_remote_client_PARAMETERS LOCALITIES 2)
set(apply_local_PARAMETERS THREADS_PER_LOCALITY 4)
set(apply_local_executor_PARAMETERS THREADS_PER_LOCALITY 4)
set(async_cb_colocated_PARAMETERS LOCALITIES 2)
set(async_local_PARAMETERS THREADS_PER_LOCALITY 4)
set(async_local_executor_PARAMETERS THREADS_PER_LOCALITY 4)

set(async_continue_PARAMETERS LOCALITIES 2)
set(async_continue_cb_PARAMETERS LOCALITIES 2)
set(async_continue_cb_colocated_PARAMETERS LOCALITIES 2)
set(async_remote_PARAMETERS LOCALITIES 2)
set(async_remote_client_PARAMETERS LOCALITIES 2)
set(async_cb_remote_PARAMETERS LOCALITIES 2)
set(async_cb_remote_client_PARAMETERS LOCALITIES 2)

set(remote_dataflow_PARAMETERS THREADS_PER_LOCALITY 4)
set(remote_dataflow_PARAMETERS LOCALITIES 2)

foreach(test ${tests})
  set(sources ${test}.cpp)

  source_group("Source Files" FILES ${sources})

  set(folder_name "Tests/Unit/Modules/Async")

  add_hpx_executable(
    ${test}_test INTERNAL_FLAGS
    SOURCES ${sources} ${${test}_FLAGS}
    EXCLUDE_FROM_ALL
    FOLDER ${folder_name}
  )

  add_hpx_unit_test("modules.async_distributed" ${test} ${${test}_PARAMETERS})
endforeach()
