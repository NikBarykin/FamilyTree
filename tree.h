#pragma once


#include "svg.h"
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


template<typename NodeId, size_t NParents>
class FamilyTree {
// We assume that NodeId is a light type,
// which means we shouldn't worry about resources its copying takes
public:
    struct Node {
        NodeId id;
        std::optional<std::array<NodeId, NParents>> parent_ids;

        std::vector<NodeId> GetParents() const;
//        static Node ParseFrom()
    };
private:
    std::unordered_map<NodeId, std::vector<NodeId>> children_;
    std::unordered_map<NodeId, Node> nodes_;
public:

    FamilyTree& AddNode(const Node& new_node);
    const Node* GetNode(const NodeId& node_id) const;
    std::vector<NodeId> GetChildren(const NodeId& node_id) const;
    //    template<typename NodeIt>
//    FamilyTree(NodeIt begin, NodeIt end);
    // Walkers
    template<typename NodeVisitor>
    void TraverseBreastFirst(NodeVisitor node_visitor) const;
    template<typename NodeVisitor>
    void TraverseAncestorsBreastFirst(const NodeId& start_node,
                                      NodeVisitor node_visitor) const;
    // main methods
    std::unordered_set<NodeId> GetAncestors(const NodeId& node) const;
//    std::unordered_set<NodeId> DiscardAncestors(const std::unordered_set<NodeId>& nodes) const;
    std::unordered_set<NodeId> LowestCommonAncestors(const NodeId& node1, const NodeId& node2) const;

    // Rendering
    static const size_t RENDER_WIDTH = 1500;
    static const size_t RENDER_HEIGHT = 660;
    static const size_t RENDER_PADDING = 100;
    static const size_t RENDER_NODE_RADIUS = 30;

private:
    static Svg::Color GenerateDefaultColor();
    template<typename ColorIt>
    static Svg::Color InheritColor(ColorIt color_begin, ColorIt color_end);

    std::unordered_map<NodeId, Svg::Color> CalculateColors() const;
    std::unordered_map<NodeId, Svg::Point> CalculatePositions() const;
public:
    Svg::Document RenderSvg() const;

    // side methods
//    static FamilyTree Merge(const FamilyTree& lhs, const FamilyTree& rhs);
//    friend bool operator == (const FamilyTree& lhs, const FamilyTree& rhs);

//    void PrintTo(std::ostream& output) const;
//    void SaveTo(const std::string& filename) const;
//    static FamilyTree ParseFrom(std::istream& input);
//    static FamilyTree OpenFrom(const std::string& filename);
};


// Implementations
template<typename NodeId, size_t NParents>
std::vector<NodeId> FamilyTree<NodeId, NParents>::Node::GetParents() const {
    if (parent_ids) {
        return {parent_ids->begin(), parent_ids->end()};
    } else {
        return {};
    }
}


template<typename NodeId, size_t NParents>
FamilyTree<NodeId, NParents>& FamilyTree<NodeId, NParents>::AddNode(const Node& new_node) {
    if (GetNode(new_node.id) != nullptr) {
        throw std::runtime_error("Node with given id already exists");
    }
    for (const NodeId& parent_id : new_node.GetParents()) {
        if (GetNode(parent_id) == nullptr) {
            throw std::runtime_error("Unknown parent id");
        }
    }
    for (const NodeId& parent_id : new_node.GetParents()) {
        children_[parent_id].push_back(new_node.id);
    }
    nodes_[new_node.id] = std::move(new_node);
    return *this;
}

template<typename NodeId, size_t NParents>
const typename FamilyTree<NodeId, NParents>::Node* FamilyTree<NodeId, NParents>::GetNode(
        const NodeId &node_id) const {
    if (auto node_it = nodes_.find(node_id); node_it != nodes_.end()) {
        return &node_it->second;
    } else {
        return nullptr;
    }
}


template<typename NodeId, size_t NParents>
std::vector<NodeId> FamilyTree<NodeId, NParents>::GetChildren(const NodeId &node_id) const {
    auto it = children_.find(node_id);
    return it != children_.end() ? it->second : std::vector<NodeId>{};
}


template<typename NodeId, size_t NParents>
template<typename NodeVisitor>
void FamilyTree<NodeId, NParents>::TraverseBreastFirst(NodeVisitor node_visitor) const {
    std::queue<NodeId> node_order;
    std::unordered_set<NodeId> considered_nodes;
    for (const auto& [node_id, node] : nodes_) {
        if (!node.parent_ids) {
            node_order.push(node_id);
            considered_nodes.insert(node_id);
        }
    }
    while (!node_order.empty()) {
        NodeId node_id = node_order.front();
        node_order.pop();
        node_visitor(*GetNode(node_id));
        for (const NodeId &child_id: GetChildren(node_id)) {
            if (!considered_nodes.count(child_id)) {
                node_order.push(child_id);
                considered_nodes.insert(child_id);
            }
        }
    }
}


template<typename NodeId, size_t NParents>
template<typename NodeVisitor>
void FamilyTree<NodeId, NParents>::TraverseAncestorsBreastFirst(
        const NodeId &start_node, NodeVisitor node_visitor) const {
    std::queue<NodeId> node_order;
    node_order.push(start_node);
    std::unordered_set<NodeId> considered_nodes = {start_node};
    while (!node_order.empty()) {
        NodeId node_id = node_order.front();
        node_order.pop();
        node_visitor(*GetNode(node_id));
        for (const NodeId &parent: GetNode(node_id)->GetParents()) {
            if (!considered_nodes.count(parent)) {
                node_order.push(parent);
                considered_nodes.insert(parent);
            }
        }
    }
}


template<typename NodeId, size_t NParents>
Svg::Color FamilyTree<NodeId, NParents>::GenerateDefaultColor() {
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
Svg::Color FamilyTree<NodeId, NParents>::InheritColor(ColorIt color_begin, ColorIt color_end) {
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
std::unordered_map<NodeId, Svg::Color> FamilyTree<NodeId, NParents>::CalculateColors() const {
    std::unordered_map<NodeId, Svg::Color>  colors;
    auto color_calculator = [& colors](const Node& node) {
        if (!node.parent_ids) {
             colors[node.id] = GenerateDefaultColor();
        } else {
            std::array<Svg::Color, NParents> parent_colors;
            for (size_t parent_i = 0; parent_i < NParents; ++parent_i) {
                const NodeId& parent_id = (*node.parent_ids)[parent_i];
                parent_colors[parent_i] = colors[parent_id];
            }
             colors[node.id] = InheritColor(begin(parent_colors), end(parent_colors));
        }
    };
    TraverseBreastFirst(color_calculator);
    return colors;
}


template<typename NodeId, size_t NParents>
std::unordered_map<NodeId, Svg::Point> FamilyTree<NodeId, NParents>::CalculatePositions() const {
    std::vector<std::vector<NodeId>> nodes_by_level;
    // TODO: create separate function for level calculation
    std::unordered_map<NodeId, size_t> levels;
    auto node_processor = [&nodes_by_level, &levels](const Node& node) {
        size_t node_level = 0;
        for (const NodeId& parent_id : node.GetParents()) {
            node_level = std::max(node_level, levels[parent_id] + 1);
        }
        if (node_level >= nodes_by_level.size()) {
            nodes_by_level.emplace_back();
        }
        nodes_by_level[node_level].push_back(node.id);
        levels[node.id] = node_level;
    };
    TraverseBreastFirst(node_processor);
    std::unordered_map<NodeId, Svg::Point> positions;
    auto create_even_distribution = [](double min_val, double max_val, size_t n_points) -> std::vector<double> {
        std::vector<double> points(n_points);
        if (n_points == 1) {
            points[0] = (min_val + max_val) / 2.0;
        } else {
            points[0] = min_val;
            for (size_t i = 1; i < n_points; ++i) {
                points[i] = points[i - 1] + (max_val - min_val) / (n_points - 1);
            }
        }
        return points;
    };
    auto y_distribution = create_even_distribution(RENDER_PADDING, RENDER_HEIGHT - RENDER_PADDING,
                                                   nodes_by_level.size());
    for (size_t level = 0; level < nodes_by_level.size(); ++level) {
        auto x_distribution = create_even_distribution(RENDER_PADDING, RENDER_WIDTH - RENDER_PADDING,
                                                       nodes_by_level[level].size());
        for (size_t node_i = 0; node_i < nodes_by_level[level].size(); ++node_i) {
            const NodeId& node_id = nodes_by_level[level][node_i];
            positions[node_id] = Svg::Point{.x = x_distribution[node_i],
                                            .y = y_distribution[level]};
        }
    }
    return positions;
}


template<typename NodeId, size_t NParents>
Svg::Document FamilyTree<NodeId, NParents>::RenderSvg() const {
    Svg::Document tree_doc;
    auto colors = CalculateColors();
    auto positions = CalculatePositions();
    auto node_renderer = [&tree_doc, &colors, &positions](const Node& node) {
        Svg::Point node_pos = positions[node.id];
        for (const NodeId& parent_id : node.GetParents()) {
            tree_doc.Add(Svg::Polyline{}.AddPoint(positions[parent_id])
                                        .AddPoint(node_pos)
                                        .SetStrokeColor(colors[parent_id]));
        }
        tree_doc.Add(Svg::Circle{}.SetRadius(RENDER_NODE_RADIUS)
                                  .SetCenter(node_pos)
                                  .SetStrokeColor("black")
                                  .SetFillColor(colors[node.id]));
        tree_doc.Add(Svg::Text{}.SetData(node.id)
                                .SetPoint({node_pos.x + RENDER_NODE_RADIUS, node_pos.y})
                                .SetStrokeColor("black")
                                .SetFillColor("black")
                                .SetFontSize(RENDER_NODE_RADIUS));
    };
    TraverseBreastFirst(node_renderer);
    return tree_doc;
}


template<typename NodeId, size_t NParents>
std::unordered_set<NodeId> FamilyTree<NodeId, NParents>::GetAncestors(const NodeId &node) const {
    std::unordered_set<NodeId> result;
    auto node_visitor = [&result] (const Node& node) {
        result.insert(node.id);
    };
    TraverseAncestorsBreastFirst(node, node_visitor);
    return result;
}


template<typename NodeId, size_t NParents>
std::unordered_set<NodeId> FamilyTree<NodeId, NParents>::LowestCommonAncestors(
        const NodeId &node1, const NodeId &node2) const {
    auto ancestors1 = GetAncestors(node1);
    auto ancestors2 = GetAncestors(node2);
    auto common_ancestors = UnorderedSetIntersection(ancestors1, ancestors2);
    auto lowest_common_ancestors = common_ancestors;
    for (const NodeId& node : common_ancestors) {
        for (const NodeId& parent : GetNode(node)->GetParents()) {
            lowest_common_ancestors.erase(parent);
        }
    }
    return lowest_common_ancestors;
}
