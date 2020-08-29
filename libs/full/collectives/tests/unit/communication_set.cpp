//  Copyright (c) 2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>
#include <hpx/modules/async_distributed.hpp>
#include <hpx/modules/collectives.hpp>
#include <hpx/modules/format.hpp>
#include <hpx/modules/pack_traversal.hpp>
#include <hpx/modules/testing.hpp>

#include <hpx/collectives/detail/communication_set_node.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace test {

    struct communication_set_tag
    {
    };

    struct get_connected_to
    {
    };
}    // namespace test

namespace hpx { namespace traits {

    template <typename Communicator>
    struct communication_operation<Communicator, test::communication_set_tag>
      : std::enable_shared_from_this<
            communication_operation<Communicator, test::communication_set_tag>>
    {
        communication_operation(Communicator& comm)
          : communicator_(comm)
        {
        }

        template <typename Result>
        Result get(std::size_t which, test::get_connected_to)
        {
            return communicator_.connect_to_;
        }

        Communicator& communicator_;
    };
}}    // namespace hpx::traits

namespace test {

    using get_connected_to_action =
        typename hpx::lcos::detail::communication_set_node::template get_action<
            communication_set_tag, std::size_t, get_connected_to>;
}    // namespace test

///////////////////////////////////////////////////////////////////////////////
// Every node is connected to another one
//
//               /                     \
//              0                       8
//             / \                     / \
//            /   \                   /   \
//           /     \                 /     \
//          /       \               /       \
//         /         \             /         \
//        0           4           8           2
//       / \         / \         / \         / \
//      /   \       /   \       /   \       /   \
//     0     2     4     6     8     0     2     4    <-- communicator nodes
//    / \   / \   / \   / \   / \   / \   / \   / \
//   0   1 2   3 4   5 6   7 8   9 0   1 2   3 4   5  <-- participants
//
// clang-format off
std::size_t connected_nodes[] = {
    0,  0,  0,  4,  0,  8,  8, 12,   //  0.. 7
    0, 16, 16, 20, 16, 24, 24, 28,   //  8..15
    0, 32, 32, 36, 32, 40, 40, 44,   // 16..23
   32, 48, 48, 52, 48, 56, 56, 60,   // 24..31
};
// clang-format on

///////////////////////////////////////////////////////////////////////////////
constexpr char const* communication_set_basename = "/test/communication_set";

void test_communication_set(std::size_t size, std::size_t arity)
{
    // create nodes of communication set
    std::string basename =
        hpx::util::format("{}_{}_{}", communication_set_basename, size, arity);

    std::vector<hpx::future<hpx::id_type>> nodes;
    nodes.reserve(size);
    for (std::size_t i = 0; i != size; ++i)
    {
        nodes.push_back(hpx::lcos::create_communication_set(
            basename.c_str(), size, i, arity));
    }

    // verify that all leaf-nodes are connected to the proper parent
    std::vector<hpx::id_type> node_ids = hpx::util::unwrap(nodes);
    for (std::size_t i = 0; i < size; i += arity)
    {
        for (std::size_t j = 1; j != arity && i + j < size; ++j)
        {
            HPX_TEST(node_ids[i] == node_ids[i + j]);
        }
    }

    // verify how created nodes are connected
    for (std::size_t i = 0; i != size; ++i)
    {
        hpx::future<std::size_t> f = hpx::async(test::get_connected_to_action{},
            node_ids[i], i, test::get_connected_to{});
        HPX_TEST_EQ(f.get(), connected_nodes[i / arity]);
    }
}

int hpx_main(int argc, char* argv[])
{
    for (std::size_t size = 0; size != 64; ++size)
    {
        test_communication_set(size + 1, 2);
        test_communication_set(size + 1, 4);
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    HPX_TEST_EQ(hpx::init(argc, argv), 0);
    return hpx::util::report_errors();
}
