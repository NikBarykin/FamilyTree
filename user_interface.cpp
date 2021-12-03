#include "user_interface.h"
#include "tree.h"

using namespace std;


FamilyTree::Tree<string, 2> OpenFrom(const string& filename) {
    return FamilyTree::Tree<string, 2>::ParseFrom(ReadEverythingFromFile(filename));
}


void RunInteraction(const string& start_filename, istream& command_stream, ostream& output) {
    using Tree = FamilyTree::Tree<string, 2>;
    Tree family_tree;
    if (!start_filename.empty()) {
        family_tree = OpenFrom(start_filename);
    }
    for (string command; getline(command_stream, command); ) {
        vector<string> tokens = Split(command);
        if (tokens.empty()) {
            continue;
        }
        string command_name = MakeLower(tokens[0]);
        vector<string> arguments(make_move_iterator(tokens.begin() + 1),
                                 make_move_iterator(tokens.end()));
        if (command_name == "exit") {
            break;
        } else if (command_name == "add" || command_name == "addnode") {
            family_tree.AddNode(Tree::Node(arguments[0], arguments.begin() + 1, arguments.end()));
        } else if (command_name == "open") {
            family_tree = OpenFrom(arguments[0]);
        } else if (command_name == "save") {
            ofstream f_output(arguments[0]);
            f_output << family_tree;
        } else if (command_name == "print") {
            output << family_tree;
        } else if (command_name == "render") {
            auto svg_doc = family_tree.RenderSvg();
            if (arguments.empty()) {
                svg_doc.Render(output);
            } else {
                ofstream f_output(arguments[0]);
                svg_doc.Render(f_output);
            }
        } else if (command_name == "lowestcommonancestors" || command_name == "lca") {
            auto common_ancestors = family_tree.LowestCommonAncestors(arguments[0], arguments[1]);
            if (common_ancestors.empty()) {
                output << "No common ancestors";
            } else {
                PrintSequenceWithDelimiter(output, begin(common_ancestors), end(common_ancestors));
            }
            output << endl;
        } else if (command_name == "merge") {
          Tree other_tree = OpenFrom(arguments[0]);
          family_tree = Tree::Merge(family_tree, other_tree);
        } else if (command_name == "help") {
            // TODO: help command handling
        } else {
            output << "Unknown command" << endl;
        }
    }
}
