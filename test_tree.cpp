#include "test_tree.h"
#include "test_runner.h"
#include "tree.h"

using namespace std;
using namespace FamilyTree;

//
//template<typename NodeId, size_t NParents>
//using Node = typename FamilyTree<NodeId, NParents>::Node;
//

namespace {

    void TestFamilyTreeNode() {
        {
            using NodeT = Node<int, 2>;
            NodeT node1(1);
            ASSERT_EQUAL(node1.id, 1);
            ASSERT(!node1.parent_ids);
            ASSERT(node1.GetParents().empty());
            vector<int> empty_parents;
            NodeT node2(2, begin(empty_parents), end(empty_parents));
            ASSERT_EQUAL(node2.id, 2);
            ASSERT(!node2.parent_ids);
            ASSERT(node2.GetParents().empty());
            NodeT node3(3, empty_parents);
            ASSERT_EQUAL(node3.id, 3);
            ASSERT(!node3.parent_ids);
            ASSERT(node3.GetParents().empty());
            NodeT node4 = NodeT::ParseFrom("1");
            ASSERT_EQUAL(node1, node4);
            NodeT node5(5, vector<int>{1, 2});
            ASSERT(node5.parent_ids);
            ASSERT_EQUAL(node5.GetParents(), (vector<int>{1, 2}));
            ASSERT_EQUAL(node5, NodeT::ParseFrom("5 2 1"));
            ASSERT(node5 != NodeT::ParseFrom("2 1 5"));
            stringstream ss;
            ss << node1 << endl << node5;
            ASSERT_EQUAL(ss.str(), "1\n5 1 2");
            ASSERT(node1 != node5);
            ASSERT_THROWS(NodeT::ParseFrom("1 2"), runtime_error);
            ASSERT_THROWS(NodeT::ParseFrom("1 2 3 4"), runtime_error);
            ASSERT_THROWS(NodeT::ParseFrom(""), runtime_error);
        }
        {
            auto node = Node<string, 4>::ParseFrom("Ivan Oleg Pavel Maria Vasiliy");
            ASSERT_EQUAL(node.id, "Ivan");
            ASSERT_EQUAL(node.GetParents(), (vector<string>{"Oleg", "Pavel", "Maria", "Vasiliy"}));
        }
    }

    void TestFamilyTreeCreation() {
        {
            using TreeT = Tree<double, 3>;
            using NodeT = Node<double, 3>;
            vector<NodeT> nodes = {NodeT::ParseFrom("1.0"),
                                  NodeT::ParseFrom("2.0"),
                                  NodeT::ParseFrom("3.0"),
                                  NodeT::ParseFrom("4.0 1.0 2.0 3.0")};
            TreeT tree1(begin(nodes), end(nodes));
            TreeT tree2;
            for (NodeT node: nodes) {
                tree2.AddNode(node);
            }
            stringstream tree1_stream;
            tree1_stream << tree1;
            TreeT tree3 = TreeT::ParseFrom(tree1_stream.str());
            TreeT tree4 = TreeT::ParseFrom(R"(1.0
2.0
3.0
4.0 2.0 3.0 1.0)");
            ASSERT_EQUAL(tree1, tree2);
            ASSERT_EQUAL(tree2, tree3);
            ASSERT_EQUAL(tree3, tree4);
        }
        {
            using TreeT = Tree<char, 5>;
            auto tree1 = TreeT::ParseFrom(R"(A
B
C
D
E
F A B C D E
G A B C D E)");
            auto tree2 = TreeT::ParseFrom(R"(E
A
C
B
D
G B A C D E
F A B E D C)");
            ASSERT_EQUAL(tree1, tree2);
            auto tree3 = TreeT::ParseFrom("A");
            ASSERT(tree1 != tree3);
        }
        {
            Tree<string, 2> simple_tree;
            using NodeT = Node<string, 2>;
            simple_tree.AddNode(NodeT::ParseFrom("1"));
            auto old_simple_tree = simple_tree;
            ASSERT_THROWS(simple_tree.AddNode(NodeT::ParseFrom("1")), runtime_error);
            ASSERT_EQUAL(old_simple_tree, simple_tree);
            simple_tree.AddNode(NodeT::ParseFrom("2"))
                       .AddNode(NodeT::ParseFrom("biba"));
            ASSERT_THROWS(simple_tree.AddNode(NodeT::ParseFrom("boba biba amogus")), runtime_error);
            auto expected_simple_tree = Tree<string, 2>::ParseFrom(R"(1
2
biba)");
            ASSERT_EQUAL(simple_tree, expected_simple_tree);
        }
    }


    void TestFamilyTreeGetters() {
        using TreeT = Tree<size_t, 2>;
        using NodeT = Node<size_t, 2>;
        {
            auto tree1 = TreeT::ParseFrom(R"(0
1
2 0 1
3 2 1
4 3 1
5 4 3
6 4 3
7 3 4
8 6 7
9 0 8)");
            ASSERT_EQUAL(tree1.GetSize(), 10);
            ASSERT(tree1.GetNode(123) == nullptr);
            ASSERT_EQUAL(*tree1.GetNode(9), NodeT::ParseFrom("9 8 0"));
            ASSERT_EQUAL(*tree1.GetNode(4), NodeT::ParseFrom("4 3 1"));
            unordered_set<size_t> expected_children_4{5, 6, 7};
            ASSERT_EQUAL(tree1.GetChildren(4), expected_children_4);
            unordered_set<size_t> expected_children_0{9, 2};
            ASSERT_EQUAL(tree1.GetChildren(0), expected_children_0);
            ASSERT_EQUAL(tree1.GetChildren(9), unordered_set<size_t>{});
            unordered_map<size_t, size_t> order;
            auto node_enumerator = [&order](const NodeT& node) {
                order.emplace(node.id, order.size());
            };
            tree1.TraverseBreadthFirst(node_enumerator);
            ASSERT(order[0] <= 1);
            ASSERT(order[1] <= 1);
            ASSERT(order[2] <= 5);
            ASSERT(order[3] <= 5);
            ASSERT(order[4] <= 5);
            ASSERT(order[9] <= 5);
            ASSERT(order[5] <= 8);
            ASSERT(order[6] <= 8);
            ASSERT(order[7] <= 8);
            ASSERT(order[8] == 9);
        }
        {
            auto tree2 = TreeT::ParseFrom("100");
            ASSERT_EQUAL(tree2.GetNodes(), vector<NodeT>{NodeT(100)});
            auto tree3 = TreeT::ParseFrom(R"(1
2
3 1 2)");
            ASSERT_EQUAL(tree3.GetNodesBreadthFirst().back(), NodeT::ParseFrom("3 2 1"));
        }
    }
}


void TestAll() {
    TestRunner tr;
    RUN_TEST(tr, TestFamilyTreeNode);
    RUN_TEST(tr, TestFamilyTreeCreation);
    RUN_TEST(tr, TestFamilyTreeGetters);
}
