#pragma once

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
        Node(const NodeId &id, NodeIdIt parent_ids_begin, NodeIdIt parents_ids_end):
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

        template<typename Container>
        Node(const NodeId &id, Container container):
                Node(id, std::begin(container), std::end(container)) {}

        std::vector<NodeId> GetParents() const {
            if (parent_ids) {
                return {parent_ids->begin(), parent_ids->end()};
            } else {
                return {};
            }
        }

        static Node ParseFrom(const std::string& input) {
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
    };


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
