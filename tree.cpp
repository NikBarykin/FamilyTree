#include "tree.h"
#include "utils.h"

#include <iostream>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <fstream>
#include <random>
#include <ctime>
using namespace std;


namespace FamilyTree {
    BirthInfo BirthInfo::ParseFrom(const std::string &birth_info_str) {
        // Valid BirthInfo string representation looks like
        // name: mother_name, father_name
        auto tokens = Split(birth_info_str);
        // TODO: exception handling
//        bool check1 = tokens.size() == 3;
//        bool check2 = tokens[0].size() >= 2 && tokens[0].back() == ':';
//        bool check3 = tokens[1].size() >= 2 && tokens[1].back() == ',';
//        bool check4 = !tokens[2].empty();
//        if (!check1 || !check2 || !check3 || !check4) {
//            throw
//        }
        tokens[0].pop_back();
        tokens[1].pop_back();
        return BirthInfo{.name = tokens[0],
                         .mother_name = tokens[1],
                         .father_name = tokens[2]};
    }

    Tree::Tree() :
            adam_(make_unique<Node>("Adam", nullptr, nullptr)),
            eve_(make_unique<Node>("Eve", nullptr, nullptr)) {
        nodes_["Adam"] = adam_.get();
        nodes_["Eve"] = eve_.get();
    }

//    template<typename BirthInfoIt>
//    Tree::Tree(BirthInfoIt birth_info_begin, BirthInfoIt birth_info_end): Tree() {
//        for (BirthInfoIt birth_info_it = birth_info_begin; birth_info_it != birth_info_end; ++birth_info_it) {
//            // TODO: exception handling
//            Birth(*birth_info_it);
//        }
//    }

    const Node *Tree::NodeByName(const std::string &name) const {
        // nullptr means that there is no node with such name
        if (auto it = nodes_.find(name); it != nodes_.end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }

    Node *Tree::NodeByName(const std::string &name) {
        // -//-
        if (auto it = nodes_.find(name); it != nodes_.end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }

    Tree& Tree::Birth(const BirthInfo &birth_info) {
        // TODO: invalid birth_info handling:
        //  mother == nullptr || father == nullptr || child is already born
        Node *mother = NodeByName(birth_info.mother_name);
        Node *father = NodeByName(birth_info.father_name);
        shared_ptr<Node> child = make_shared<Node>(birth_info.name, mother, father);
        nodes_[child->name] = child.get();
        birth_order_.push_back(child->name);
        mother->children.push_back(child);
        father->children.push_back(child);
        return *this;
    }

//    std::vector<const Node *> Tree::NodesBF() const {
//        unordered_set<const Node*> used = {adam_.get(), eve_.get()};
//        vector<const Node *> order;
//        order.push_back(adam_.get());
//        order.push_back(eve_.get());
//        for (size_t i = 0; i != order.size(); ++i) {
//            for (auto child_ptr : order[i]->children) {
//                if (!used.count(child_ptr.get())) {
//                    order.push_back(child_ptr.get());
//                    used.insert(child_ptr.get());
//                }
//            }
//        }
//        return order;
//    }

//    void Tree::Print(std::ostream &output) const {
//        for (auto node_ptr : NodesBF()) {
//            if (node_ptr != adam_.get() && node_ptr != eve_.get()) {
//                output << node_ptr->name << ": " << node_ptr->mother->name
//                       << ", " << node_ptr->father->name << endl;
//            }
//        }
//    }
//
//    void Tree::SaveTo(const std::string &filename) const {
//        ofstream output(filename);
//        auto save_walker = [&output](const Node* node_ptr) {
//            if (node_ptr->name == "Adam" || node_ptr->name == "Eve") {
//                return;
//            }
//            output << node_ptr->name << ": " << node_ptr->mother->name
//                   << ", " << node_ptr->father->name << endl;
//        };
//        Walk(save_walker);
//        for (auto[name, node_ptr]: nodes_) {
//            if (name == adam_->name || name == eve_->name) {
//                continue;
//            }
//            output << name << ": " << node_ptr->mother->name
//                   << ", " << node_ptr->father->name << endl;
//        }
//    }
    void Tree::PrintTo(std::ostream &output) const {
        for (const string& name : birth_order_) {
            if (name != adam_->name && name != eve_->name) {
                auto node_ptr = NodeByName(name);
                output << name << ": " << node_ptr->mother->name
                       << ", " << node_ptr->father->name << endl;
            }
        }
    }

    void Tree::SaveTo(const std::string &filename) const {
        ofstream output(filename);
        PrintTo(output);
    }

    Tree Tree::ParseFrom(std::istream &input) {
        Tree result;
        for (string birth_info_str; getline(input, birth_info_str); ) {
            // TODO: add exception handling
            result.Birth(BirthInfo::ParseFrom(birth_info_str));
        }
        return result;
    }

    Tree Tree::OpenFrom(const std::string &filename) {
        ifstream input(filename);
        return ParseFrom(input);
    }

    const Node * Tree::LowestCommonAncestor(const std::string &person1_name, const std::string &person2_name) const {
        // TODO: implement
        return nullptr;
    }

    unordered_map<string, Svg::Color> Tree::CalculateColors() const {
        unordered_map<string, Svg::Color> result;
//        result[adam_->name] = RandColor(228 + time(0));
        result[adam_->name] = Svg::Rgb{255, 0, 0};
//        result[eve_->name] = RandColor(1337 + time(0));
        result[eve_->name] = Svg::Rgb{0, 255, 255};
        for (const string& name : birth_order_) {
            string mother_name = NodeByName(name)->mother->name;
            string father_name = NodeByName(name)->father->name;
            vector<Svg::Rgb> rgb_cols = {get<Svg::Rgb>(result[father_name]),
                                         get<Svg::Rgb>(result[mother_name]),
                                         get<Svg::Rgb>(RandColor(hash<string>()(name) + time(0)))};
            result[name] = MergeRgb(begin(rgb_cols), end(rgb_cols));
//            result[name] = MergeRgb(get<Svg::Rgb>(result[father_name]),
//                                    get<Svg::Rgb>(result[mother_name]));
//            result[name] = MergeRgb(get<Svg::Rgb>(result[name]),
//                                get<Svg::Rgb>(RandColor(hash<string>()(name) + time(0))));
            //            Svg::Rgb mother_c = get<Svg::Rgb>(result[mother_name]);
//            Svg::Rgb father_c = get<Svg::Rgb>(result[father_name]);
//            double merge_coeff = Rand01(hash<string>()(name) + time(nullptr));
//            result[name] = Svg::Rgb{static_cast<int>(mother_c.red * merge_coeff + father_c.red * (1.0 - merge_coeff)),
//                                    static_cast<int>(mother_c.green * merge_coeff + father_c.green * (1.0 - merge_coeff)),
//                                    static_cast<int>(mother_c.blue * merge_coeff + father_c.blue * (1.0 - merge_coeff))};
        }
        return result;
    }

    std::unordered_map<std::string, Svg::Point> Tree::CalculatePositions() const {
        unordered_map<string, size_t> levels;
        vector<size_t> level_sizes;
        levels[eve_->name] = 0;
        levels[adam_->name] = 0;
        level_sizes.push_back(2);
        for (const string& name : birth_order_) {
            const string& mother_name = NodeByName(name)->mother->name;
            const string& father_name = NodeByName(name)->father->name;
            size_t level = max(levels[mother_name], levels[father_name]) + 1;
            levels[name] = level;
            if (level == level_sizes.size()) {
                level_sizes.push_back(0);
            }
            ++level_sizes[level];
        }
        vector<size_t> level_shifts(level_sizes.size(), 0);
        unordered_map<string, Svg::Point> positions;
        for (const auto& [name, level] : levels) {
            size_t y;
            if (level_sizes.size() == 1) {
                y = RENDER_HEIGHT / 2;
            } else {
                size_t y_delta = (RENDER_HEIGHT - 2 * RENDER_PADDING) / (level_sizes.size() - 1);
                y = level * y_delta + RENDER_PADDING;
            }
            size_t x;
            if (level_sizes[level] == 1) {
                x = RENDER_WIDTH / 2;
            } else {
                x = level_shifts[level] + RENDER_PADDING;
                level_shifts[level] += (RENDER_WIDTH - 2 * RENDER_PADDING) / (level_sizes[level] - 1);
            }
            positions[name] = Svg::Point{static_cast<double>(x), static_cast<double>(y)};
        }
        return positions;
    }

    Svg::Document Tree::RenderSvg() const {
        Svg::Document result;
        unordered_map<string, Svg::Color> colors = CalculateColors();
//        cerr << colors[eve_->name] << " " << colors[adam_->name] << endl;
        unordered_map<string, Svg::Point> positions = CalculatePositions();
        // birth_order_ - without Adam and Eve (they don't have parents)
        for (const string& name : birth_order_) {
            const string& mother_name = NodeByName(name)->mother->name;
            result.Add(Svg::Polyline{}.AddPoint(positions[mother_name])
                               .AddPoint(positions[name])
                               .SetStrokeColor(colors[mother_name]));
            const string& father_name = NodeByName(name)->father->name;
            result.Add(Svg::Polyline{}.AddPoint(positions[father_name])
                               .AddPoint(positions[name])
//                               .SetStrokeColor("black")
                               .SetStrokeColor(colors[father_name]));
        }
        for (const auto& [name, pos] : positions) {
            result.Add(Svg::Circle{}.SetRadius(RENDER_NODE_RADIUS)
                               .SetCenter(pos)
                               .SetStrokeColor("black")
                               .SetFillColor(colors[name]));
            result.Add(Svg::Text{}.SetData(name)
                               .SetPoint({pos.x + RENDER_NODE_RADIUS, pos.y})
                               .SetStrokeColor("black")
                               .SetFillColor("black")
                               .SetFontSize(RENDER_NODE_RADIUS));
        }
        return result;
    }

    void Tree::RenderTo(const std::string &filename) {
        ofstream output(filename);
        RenderSvg().Render(output);
    }
}
