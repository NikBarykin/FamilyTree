#pragma once

#include "C:/Dev/Cpp/Libs/svg/svg.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

// Oh god, I am too lazy to implement appropriate user interface...
namespace FamilyTree {
    // TODO: add template PersonId = std::string
    struct Node {
        std::string name;
        Node *mother, *father;
        std::vector<std::shared_ptr<Node>> children;

        Node(std::string name, Node* mother, Node* father):
                name(move(name)), mother(mother), father(father) {}
    };

    struct BirthInfo {
        std::string name, mother_name, father_name;
        static BirthInfo ParseFrom(const std::string& birth_info_str);
    };

//    struct
    class Tree {
        // TODO: improve by using ids instead of names
    private:
        // TODO: destroy eve and adam
        std::unique_ptr<Node> adam_, eve_;
        std::unordered_map<std::string, Node*> nodes_;
        std::vector<std::string> birth_order_;
    public:
        Tree();
        template<typename BirthInfoIt>
        Tree(BirthInfoIt birth_info_begin, BirthInfoIt birth_info_end): Tree() {
            for (BirthInfoIt birth_info_it = birth_info_begin; birth_info_it != birth_info_end; ++birth_info_it) {
                // TODO: exception handling
                Birth(*birth_info_it);
            }
        }
        Tree& Birth(const BirthInfo& birth_info);
        const Node* NodeByName(const std::string& name) const;
        Node* NodeByName(const std::string& name);
//        std::vector<const Node*> NodesBF() const;
//        void Print(std::ostream& output) const;
//        // Traverse family tree using BFS algorithm and invoke visitor with every node
//        template<typename Visitor>
//        void Walk(Visitor visitor);
//        template<typename Visitor>
//        void Walk(Visitor visitor) c
        void PrintTo(std::ostream& output) const;
        void SaveTo(const std::string& filename) const;
        static Tree ParseFrom(std::istream& input);
        static Tree OpenFrom(const std::string& filename);

        const Node* LowestCommonAncestor(const std::string& person1_name,
                                         const std::string& person2_name) const;

        static const size_t RENDER_WIDTH = 1500;
        static const size_t RENDER_HEIGHT = 660;
        static const size_t RENDER_PADDING = 100;
        static const size_t RENDER_NODE_RADIUS = 30;

    private:
        std::unordered_map<std::string, Svg::Color> CalculateColors() const;
        std::unordered_map<std::string, Svg::Point> CalculatePositions() const;
    public:
        Svg::Document RenderSvg() const;
        void RenderTo(const std::string& filename);
    };
}
//
//class FamilyTree {
//private:
//    struct Node {
//        std::string name;
//        Node *mother, *father;
//        std::vector<std::shared_ptr<Node>> children;
//
//        Node(std::string name, Node* mother, Node* father):
//        name(move(name)), mother(mother), father(father) {}
//    };
//
//
//    std::unique_ptr<Node> adam_, eve_;
//    std::unordered_map<std::string, Node*> nodes_;
//public:
//
//    FamilyTree(std::vector<std::pair<std::string, std::pair<std::string, std::string>>>);
//    void Birth(const std::string& mother_name,
//               const std::string& father_name,
//               const std::string& child_name);
//    // Traverse family tree using BFS algorithm and invoke visitor with every node
//    template<typename Visitor>
//    void Walk(Visitor visitor);
//    void SaveTo(const std::string& filename) const;
//    static FamilyTree OpenFrom(const std::string& filename);
//};
