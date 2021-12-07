#pragma once

#include "Libs/svg/svg.h"
#include "utils.h"

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <array>
#include <queue>
#include <random>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>


namespace FamilyTree {
    template<typename NodeId, size_t NParents>
    struct Node {
        // TODO: struct -> class
        // TODO: add getters and make fields private
        NodeId id;
        std::optional<std::array<NodeId, NParents>> parent_ids;

        explicit Node(const NodeId &id) : id(id) {}

        template<typename NodeIdIt>
        Node(const NodeId &id, NodeIdIt parent_ids_begin, NodeIdIt parents_ids_end);

        template<typename Container>
        Node(const NodeId &id, Container container);

        std::vector<NodeId> GetParents() const;
        static Node ParseFrom(const std::string& input);
    };

    template<typename NodeId, size_t NParents>
    bool operator ==(const Node<NodeId, NParents>& lhs,
                     const Node<NodeId, NParents>& rhs);

    template<typename NodeId, size_t NParents>
    bool operator !=(const Node<NodeId, NParents>& lhs,
                     const Node<NodeId, NParents>& rhs);

    template<typename NodeId, size_t NParents>
    std::ostream& operator <<(std::ostream& output,
                              const Node<NodeId, NParents>& node);


    template<typename NodeId, size_t NParents>
    class Tree {
    public: using Node = Node<NodeId, NParents>;
    private:
        std::unordered_map<NodeId, Node> nodes_;
        std::vector<NodeId> birth_order_;

        static std::string MakeString(const NodeId &node_id);
        // Returns string made from node_id using operator <<(ostream& NodeId)

    public:
        Tree() = default;
        template<typename NodeIt>
        Tree(NodeIt begin, NodeIt end);
        static Tree ParseFrom(const std::string& input);

        size_t GetSize() const { return nodes_.size(); }

        Tree &AddNode(const Node &new_node);
        // TODO: add node by rvalue
        // TODO: node emplacement

        const Node *GetNode(const NodeId &node_id) const;
        // nullptr - node with id node_id not found

        std::vector<Node> GetNodes() const;
        // Returning nodes in birth order

        std::unordered_set<NodeId> GetAncestors(const NodeId &node) const;
        std::unordered_set<NodeId> LowestCommonAncestors(const NodeId &node1, const NodeId &node2) const;
        // Return common ancestors (node is an ancestor of itself)
        // that doesn't have common ancestors (for node1 and node2) in offspring

        static Tree Merge(const Tree &lhs, const Tree &rhs);

        // Rendering constants
        static const size_t RENDER_WIDTH = 1500;
        static const size_t RENDER_HEIGHT = 740;
        static const size_t RENDER_PADDING = 50;
        static const size_t RENDER_NODE_RADIUS = 30;

    private:
        static Svg::Color GenerateDefaultColor();

        template<typename ColorIt>
        static Svg::Color InheritColor(ColorIt color_begin, ColorIt color_end);

        std::unordered_map<NodeId, Svg::Color> CalculateColors() const;
        std::vector<std::vector<NodeId>> DistributeNodesInLevels() const;
        std::unordered_map<NodeId, Svg::Point> CalculatePositions() const;

    public:
        Svg::Document RenderSvg() const;
    };

    template<typename NodeId, size_t NParents>
    bool operator ==(const Tree<NodeId, NParents>& lhs,
                     const Tree<NodeId, NParents>& rhs);

    template<typename NodeId, size_t NParents>
    bool operator !=(const Tree<NodeId, NParents>& lhs,
                     const Tree<NodeId, NParents>& rhs);

    template<typename NodeId, size_t NParents>
    std::ostream& operator <<(std::ostream& output,
                              const Tree<NodeId, NParents>& tree);
}


// Implementations
// TODO: split into several header files
// Node
namespace FamilyTree {
    template<typename NodeId, size_t NParents>
    template<typename NodeIdIt>
    Node<NodeId, NParents>::Node(const NodeId &id, NodeIdIt parent_ids_begin, NodeIdIt parents_ids_end):
            Node(id) {
        if (parent_ids_begin == parents_ids_end) {
            return;
        }
        parent_ids.emplace();
        for (size_t i = 0; i < NParents; ++i) {
            if (parent_ids_begin == parents_ids_end) {
                throw std::runtime_error(
                        "Too few parents - " + std::to_string(i) + " should be " + std::to_string(NParents));
            }
            (*parent_ids)[i] = *parent_ids_begin;
            ++parent_ids_begin;
        }
        if (parent_ids_begin != parents_ids_end) {
            throw std::runtime_error("Too much parents, should be " + std::to_string(NParents));
        }
    }


    template<typename NodeId, size_t NParents>
    template<typename Container>
    Node<NodeId, NParents>::Node(const NodeId &id, Container container):
            Node(id, std::begin(container), std::end(container)) {}


    template<typename NodeId, size_t NParents>
    std::vector<NodeId> Node<NodeId, NParents>::GetParents() const {
        if (parent_ids) {
            return {parent_ids->begin(), parent_ids->end()};
        } else {
            return {};
        }
    }


    template<typename NodeId, size_t NParents>
    Node<NodeId, NParents> Node<NodeId, NParents>::ParseFrom(const std::string& input) {
        if (input.empty()) {
            throw std::runtime_error("Can't parse Node from empty input");
        }
        std::stringstream input_stream(input);
        NodeId node_id;
        input_stream >> node_id;
        std::vector<NodeId> parent_ids;
        for (NodeId parent_id; input_stream >> parent_id; ) {
            parent_ids.push_back(std::move(parent_id));
        }
        return Node<NodeId, NParents>(node_id, parent_ids.begin(), parent_ids.end());
    }


    template<typename NodeId, size_t NParents>
    bool operator ==(const Node<NodeId, NParents>& lhs,
                     const Node<NodeId, NParents>& rhs) {
        auto l_parents = lhs.GetParents();
        auto r_parents = rhs.GetParents();
        return lhs.id == rhs.id && std::unordered_set(l_parents.begin(), l_parents.end())
                                == std::unordered_set(r_parents.begin(), r_parents.end());
    }


    template<typename NodeId, size_t NParents>
    bool operator !=(const Node<NodeId, NParents>& lhs,
                     const Node<NodeId, NParents>& rhs) {
        return !(lhs == rhs);
    }


    template<typename NodeId, size_t NParents>
    std::ostream& operator <<(std::ostream& output,
                              const Node<NodeId, NParents>& node) {
        output << node.id;
        for (const NodeId& parent_id : node.GetParents()) {
            output << " " << parent_id;
        }
        return output;
    }
}

// Tree
namespace FamilyTree {
    template<typename NodeId, size_t NParents>
    std::string Tree<NodeId, NParents>::MakeString(const NodeId &node_id) {
        std::stringstream ss;
        ss << node_id;
        return ss.str();
    }


    template<typename NodeId, size_t NParents>
    template<typename NodeIt>
    Tree<NodeId, NParents>::Tree(NodeIt begin, NodeIt end) {
        for (auto it = begin; it != end; ++it) {
            AddNode(*it);
        }
    }


    template<typename NodeId, size_t NParents>
    Tree<NodeId, NParents> &Tree<NodeId, NParents>::AddNode(const Node &new_node) {
        if (GetNode(new_node.id) != nullptr) {
            throw std::runtime_error("Node with given id already exists");
        }
        for (const NodeId &parent_id: new_node.GetParents()) {
            if (GetNode(parent_id) == nullptr) {
                throw std::runtime_error("Unknown parent id");
            }
        }
        nodes_.emplace(new_node.id, new_node);
        birth_order_.push_back(new_node.id);
        return *this;
    }


    template<typename NodeId, size_t NParents>
    const Node<NodeId, NParents> *Tree<NodeId, NParents>::GetNode(
            const NodeId &node_id) const {
        if (auto node_it = nodes_.find(node_id); node_it != nodes_.end()) {
            return &node_it->second;
        } else {
            return nullptr;
        }
    }


    template<typename NodeId, size_t NParents>
    std::vector<Node<NodeId, NParents>> Tree<NodeId, NParents>::GetNodes() const {
        std::vector<Node> nodes;
        for (const NodeId &node_id: birth_order_) {
            nodes.push_back(*GetNode(node_id));
        }
        return nodes;
    }


    template<typename NodeId, size_t NParents>
    Svg::Color Tree<NodeId, NParents>::GenerateDefaultColor() {
        static std::mt19937 rnd(time(nullptr) + 239);
        auto gen_rand_channel = []() -> int {
            return rnd() % 256;
        };
        return Svg::Rgb{
                .red = gen_rand_channel(),
                .green = gen_rand_channel(),
                .blue = gen_rand_channel(),
        };
    }


    template<typename NodeId, size_t NParents>
    template<typename ColorIt>
    Svg::Color Tree<NodeId, NParents>::InheritColor(ColorIt color_begin, ColorIt color_end) {
        int red_sum = 0, green_sum = 0, blue_sum = 0, n_colors = 0;
        auto add_color = [&red_sum, &green_sum, &blue_sum, &n_colors](Svg::Color color) {
            if (!std::holds_alternative<Svg::Rgb>(color)) {
                throw std::runtime_error("Can't merge non rgb color");
            }
            Svg::Rgb rgb_color = std::get<Svg::Rgb>(color);
            red_sum += rgb_color.red;
            green_sum += rgb_color.green;
            blue_sum += rgb_color.blue;
            ++n_colors;
        };
        for (ColorIt color_it = color_begin; color_it != color_end; ++color_it) {
            add_color(*color_it);
        }
        add_color(GenerateDefaultColor());
        return Svg::Rgb{
                .red = red_sum / n_colors,
                .green = green_sum / n_colors,
                .blue = blue_sum / n_colors,
        };
    }


    template<typename NodeId, size_t NParents>
    std::unordered_map<NodeId, Svg::Color> Tree<NodeId, NParents>::CalculateColors() const {
        std::unordered_map<NodeId, Svg::Color> colors;
        for (const Node &node: GetNodes()) {
            if (!node.parent_ids) {
                colors[node.id] = GenerateDefaultColor();
            } else {
                std::array<Svg::Color, NParents> parent_colors;
                for (size_t parent_i = 0; parent_i < NParents; ++parent_i) {
                    const NodeId &parent_id = (*node.parent_ids)[parent_i];
                    parent_colors[parent_i] = colors[parent_id];
                }
                colors[node.id] = InheritColor(begin(parent_colors), end(parent_colors));
            }
        }
        return colors;
    }


    template<typename NodeId, size_t NParents>
    std::vector<std::vector<NodeId>> Tree<NodeId, NParents>::DistributeNodesInLevels() const {
        std::vector<std::vector<NodeId>> levels;
        std::unordered_map<NodeId, size_t> level_by_node;
        auto nodes = GetNodes();
        std::reverse(nodes.begin(), nodes.end());
        for (const Node &node : nodes) {
            size_t node_level = level_by_node[node.id];
            for (const NodeId &parent_id : node.GetParents()) {
                level_by_node[parent_id] = std::max(level_by_node[parent_id], node_level + 1);
            }
            if (node_level >= levels.size()) {
                levels.emplace_back();
            }
            levels[node_level].push_back(node.id);
        }
        std::reverse(levels.begin(), levels.end());
        return levels;
    }


    template<typename NodeId, size_t NParents>
    std::unordered_map<NodeId, Svg::Point> Tree<NodeId, NParents>::CalculatePositions() const {
        auto levels = DistributeNodesInLevels();
        std::unordered_map<NodeId, Svg::Point> positions;
        double level_y = levels.size() > 1 ? RENDER_PADDING : RENDER_HEIGHT / 2.0;
        for (size_t level = 0; level < levels.size(); ++level) {
            if (level) {
                level_y += (RENDER_HEIGHT - RENDER_PADDING * 2) / (levels.size() - 1);
            }
            double x = RENDER_PADDING;
            for (size_t node_i = 0; node_i < levels[level].size(); ++node_i) {
                x += (RENDER_WIDTH - RENDER_PADDING * 2) / (levels[level].size() + 1);
                const NodeId &node_id = levels[level][node_i];
                positions[node_id] = Svg::Point{x, level_y};
            }
        }
        return positions;
    }


    template<typename NodeId, size_t NParents>
    Svg::Document Tree<NodeId, NParents>::RenderSvg() const {
        Svg::Document tree_doc;
        auto colors = CalculateColors();
        auto positions = CalculatePositions();
        for (const Node &node: GetNodes()) {
            Svg::Point node_pos = positions[node.id];
            for (const NodeId &parent_id: node.GetParents()) {
                tree_doc.Add(Svg::Polyline{}.AddPoint(positions[parent_id])
                                     .AddPoint(node_pos)
                                     .SetStrokeColor(colors[parent_id]));
            }
            tree_doc.Add(Svg::Circle{}.SetRadius(RENDER_NODE_RADIUS)
                                 .SetCenter(node_pos)
                                 .SetStrokeColor("black")
                                 .SetFillColor(colors[node.id]));
            tree_doc.Add(Svg::Text{}.SetData(MakeString(node.id))
                                 .SetPoint({node_pos.x + RENDER_NODE_RADIUS, node_pos.y})
                                 .SetStrokeColor("black")
                                 .SetFillColor("black")
                                 .SetFontSize(RENDER_NODE_RADIUS));
        };
        return tree_doc;
    }


    template<typename NodeId, size_t NParents>
    std::unordered_set<NodeId> Tree<NodeId, NParents>::GetAncestors(const NodeId &node) const {
        std::unordered_set<NodeId> ancestors;
        std::queue<NodeId> node_order;
        node_order.push(node);
        std::unordered_set<NodeId> considered_nodes = {node};
        while (!node_order.empty()) {
            NodeId node_id = node_order.front();
            node_order.pop();
            ancestors.insert(node_id);
            for (const NodeId &parent: GetNode(node_id)->GetParents()) {
                if (!considered_nodes.count(parent)) {
                    node_order.push(parent);
                    considered_nodes.insert(parent);
                }
            }
        }
        return ancestors;
    }


    template<typename NodeId, size_t NParents>
    std::unordered_set<NodeId> Tree<NodeId, NParents>::LowestCommonAncestors(
            const NodeId &node1, const NodeId &node2) const {
        auto ancestors1 = GetAncestors(node1);
        auto ancestors2 = GetAncestors(node2);
        auto common_ancestors = UnorderedSetIntersection(ancestors1, ancestors2);
        auto lowest_common_ancestors = common_ancestors;
        for (const NodeId &node: common_ancestors) {
            for (const NodeId &parent: GetNode(node)->GetParents()) {
                lowest_common_ancestors.erase(parent);
            }
        }
        return lowest_common_ancestors;
    }


    template<typename NodeId, size_t NParents>
    Tree<NodeId, NParents> Tree<NodeId, NParents>::Merge(
            const Tree<NodeId, NParents> &lhs, const Tree<NodeId, NParents> &rhs) {
        Tree<NodeId, NParents> resulting_tree;
        for (const Node &node: lhs.GetNodes()) {
            if (rhs.GetNode(node.id) && node != *rhs.GetNode(node.id)) {
                throw std::runtime_error("Both trees have node " + MakeString(node.id) +
                                         " versions that cannot be merged");
            }
            resulting_tree.AddNode(node);
        }
        for (const Node &node: rhs.GetNodes()) {
            if (!lhs.GetNode(node.id)) {
                resulting_tree.AddNode(node);
            }
        }
        return resulting_tree;
    }


    template<typename NodeId, size_t NParents>
    Tree<NodeId, NParents> Tree<NodeId, NParents>::ParseFrom(const std::string &input) {
        std::stringstream input_stream(input);
        std::vector<Node> nodes;
        for (std::string line; std::getline(input_stream, line);) {
            if (line.empty()) {
                continue;
            }
            nodes.push_back(Node::ParseFrom(line));
        }
        return Tree<NodeId, NParents>(nodes.begin(), nodes.end());
    }


    template<typename NodeId, size_t NParents>
    bool operator==(const Tree<NodeId, NParents> &lhs,
                    const Tree<NodeId, NParents> &rhs) {
        if (lhs.GetSize() != rhs.GetSize()) {
            return false;
        }
        for (const Node<NodeId, NParents> &node: lhs.GetNodes()) {
            auto r_node_ptr = rhs.GetNode(node.id);
            if (!r_node_ptr || *r_node_ptr != node) {
                return false;
            }
        }
        return true;
    }


    template<typename NodeId, size_t NParents>
    bool operator!=(const Tree<NodeId, NParents> &lhs,
                    const Tree<NodeId, NParents> &rhs) {
        return !(lhs == rhs);
    }


    template<typename NodeId, size_t NParents>
    std::ostream &operator<<(std::ostream &output,
                             const Tree<NodeId, NParents> &tree) {
        for (const Node<NodeId, NParents>& node : tree.GetNodes()) {
            output << node.id;
            for (const NodeId &parent_id: node.GetParents()) {
                output << " " << parent_id;
            }
            output << std::endl;
        };
        return output;
    }
}
