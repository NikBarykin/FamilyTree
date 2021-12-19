#include "test_tree.h"
#include "Libs/test_runner.h"
#include "tree.h"

using namespace std;
using namespace FamilyTree;


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
        }
        {
            auto tree2 = TreeT::ParseFrom("100");
            ASSERT_EQUAL(tree2.GetNodes(), vector<NodeT>{NodeT(100)});
        }
    }

    void TestFamilyTreeAncestorFunctional() {
        using NodeT = Node<char, 3>;
        auto tree = Tree<char, 3>::ParseFrom(R"(A
B
C
D
E A B C
F A B C
G A B C
H A B C
I E F D
J F G H
K I G H
L I G D
M A K L)");
        // GetAncestors
        unordered_set<char> expected_i_ancestors{'I', 'A', 'B', 'C', 'E', 'F', 'D'};
        ASSERT_EQUAL(tree.GetAncestors('I'), expected_i_ancestors);
        unordered_set<char> expected_h_ancestors{'H', 'A', 'B', 'C'};
        ASSERT_EQUAL(tree.GetAncestors('H'), expected_h_ancestors);
        unordered_set<char> expected_m_ancestors{'A', 'B', 'C', 'D', 'E', 'F',
                                                 'G', 'H', 'I', 'K', 'L', 'M'};
        ASSERT_EQUAL(tree.GetAncestors('M'), expected_m_ancestors);
        unordered_set<char> expected_b_ancestors{'B'};
        ASSERT_EQUAL(tree.GetAncestors('B'), expected_b_ancestors);
        // LowestCommonAncestors
        unordered_set<char> expected_j_d_ancestors = {};
        ASSERT_EQUAL(tree.LowestCommonAncestors('J', 'D'), expected_j_d_ancestors);
        unordered_set<char> expected_k_m_ancestors = {'K'};
        ASSERT_EQUAL(tree.LowestCommonAncestors('K', 'M'), expected_k_m_ancestors);
        unordered_set<char> expected_k_l_ancestors = {'I', 'G'};
        ASSERT_EQUAL(tree.LowestCommonAncestors('K', 'L'), expected_k_l_ancestors);
        unordered_set<char> expected_j_e_ancestors = {'A', 'B', 'C'};
        ASSERT_EQUAL(tree.LowestCommonAncestors('J', 'E'), expected_j_e_ancestors);
        unordered_set<char> expected_m_m_ancestors = {'M'};
        ASSERT_EQUAL(tree.LowestCommonAncestors('M', 'M'), expected_m_m_ancestors);
        unordered_set<char> expected_l_j_ancestors = {'F', 'G'};
        ASSERT_EQUAL(tree.LowestCommonAncestors('L', 'J'), expected_l_j_ancestors);
        unordered_set<char> expected_m_j_ancestors = {'F', 'G', 'H'};
        ASSERT_EQUAL(tree.LowestCommonAncestors('M', 'J'), expected_m_j_ancestors);
        unordered_set<char> expected_e_d_ancestors = {};
        ASSERT_EQUAL(tree.LowestCommonAncestors('E', 'D'), expected_e_d_ancestors);
    }

    void TestFamilyTreeMerge() {
        {
            using TreeT = Tree<int, 2>;
            auto tree1 = TreeT::ParseFrom(R"(1
2
4 1 2
3
5 4 3)");
            auto tree2 = TreeT::ParseFrom(R"(2
6
1
4 2 1
7 6 2
8 4 7)");
            auto expected_merge12 = TreeT::ParseFrom(R"(6
2
1
4 2 1
7 6 2
3
5 4 3
8 7 4)");
            ASSERT_EQUAL(TreeT::Merge(tree1, tree2), expected_merge12);
            auto tree3 = TreeT::ParseFrom(R"(100
200
300 100 200)");
            auto expected_merge_13 = TreeT::ParseFrom(R"(1
2
4 1 2
3
5 4 3
200
100
300 200 100)");
            ASSERT_EQUAL(TreeT::Merge(tree1, tree3), expected_merge_13);
            auto expected_merge_123 = TreeT::ParseFrom(R"(6
2
1
4 2 1
7 6 2
3
5 4 3
8 7 4
100
200
300 100 200)");
            ASSERT_EQUAL(TreeT::Merge(tree1, TreeT::Merge(tree2, tree3)), expected_merge_123);
        }
        {
            using TreeT = Tree<string, 2>;
            auto tree1 = TreeT::ParseFrom(R"(Biba
Boba
Aboba
Bingus Biba Boba
)");
            auto tree2 = TreeT::ParseFrom(R"(Biba
Boba
Aboba
Bingus Boba Aboba)");
            ASSERT_THROWS(TreeT::Merge(tree1, tree2), runtime_error);
            auto tree3 = TreeT::ParseFrom(R"(Biba
Boba
Aboba
Bingus)");
            ASSERT_THROWS(TreeT::Merge(tree1, tree3), runtime_error);
        }
    }


    struct MyNodeId1 {
        int x, y;
    };

    struct MyNodeId1Hash {
        size_t operator()(const MyNodeId1& node_id) const {
            return static_cast<size_t>(node_id.x) * static_cast<size_t>(node_id.y);
        }
    };

    bool operator ==(const MyNodeId1& lhs, const MyNodeId1& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    ostream& operator <<(ostream& output, const MyNodeId1& node_id) {
        output << node_id.x << "," << node_id.y;
        return output;
    }

    istream& operator >>(istream& input, MyNodeId1& node_id) {
        input >> node_id.x;
        input.ignore();
        input >> node_id.y;
        return input;
    }

    void TestFamilyTreeCustomNodeId() {
        {
            using TreeT = Tree<MyNodeId1, 2, MyNodeId1Hash>;
            TreeT tree = TreeT::ParseFrom(R"(1,1
2,2
1,2 1,1 2,2)");
            ostringstream oss;
            oss << tree;
            TreeT parsed_tree = TreeT::ParseFrom(oss.str());
            ASSERT_EQUAL(tree, parsed_tree);
            TreeT tree2 = TreeT::ParseFrom(R"(1,1
2,2
3,3 1,1 2,2)");
            TreeT tree3 = TreeT::Merge(tree, tree2);
            TreeT expected_tree3 = TreeT::ParseFrom(R"(1,1
2,2
1,2 2,2 1,1
3,3 2,2 1,1)");
            ASSERT_EQUAL(tree3, expected_tree3);
            unordered_set<MyNodeId1, MyNodeId1Hash> expected_common_ancestors;
            auto common_ancestors = tree3.LowestCommonAncestors(
                    MyNodeId1{1, 2}, MyNodeId1{3, 3});
            ASSERT_EQUAL(expected_common_ancestors, common_ancestors);
        }
    }
}


void TestAll() {
    TestRunner tr;
    RUN_TEST(tr, TestFamilyTreeNode);
    RUN_TEST(tr, TestFamilyTreeCreation);
    RUN_TEST(tr, TestFamilyTreeGetters);
    RUN_TEST(tr, TestFamilyTreeAncestorFunctional);
    RUN_TEST(tr, TestFamilyTreeMerge);
}
