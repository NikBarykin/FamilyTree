#include "tree.h"
#include "utils.h"
#include "C:/Dev/Cpp/Libs/svg/svg.h"

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>
using namespace std;


//void MakeLower(string& str) {
//    transform(begin(str), end(str), begin(str),
//              [](unsigned char c){ return std::tolower(c); });
//}
//
//
//vector<string> ParseCommand(const string& command) {
//    stringstream command_stream(command);
//    vector<string> result;
//    for (string token; command_stream >> token; ) {
////        MakeLower(token);
//        result.push_back(move(token));
//    }
//    return result;
//}


void Run(istream& command_stream = cin, ostream& output = cout,
         const string& start_filename = "") {
    FamilyTree::Tree family_tree;
    if (!start_filename.empty()) {
        family_tree = FamilyTree::Tree::OpenFrom(start_filename);
    }
    for (string command; getline(command_stream, command); ) {
        vector<string> tokens = Split(command);
        if (tokens.empty()) {
            continue;
        }
        if (tokens[0] == "Exit") {
            break;
        } else if (tokens[0] == "Birth") {
            family_tree.Birth({tokens[1], tokens[2], tokens[3]});
        } else if (tokens[0] == "Open") {
            family_tree = FamilyTree::Tree::OpenFrom(tokens[1]);
        } else if (tokens[0] == "Save") {
            family_tree.SaveTo(tokens[1]);
        } else if (tokens[0] == "Print") {
            family_tree.PrintTo(output);
        } else if (tokens[0] == "Render") {
            ofstream f_output(tokens[1]);
            auto svg_doc = family_tree.RenderSvg();
            svg_doc.Render(f_output);
        } else if (tokens[0] == "Help") {
            // TODO: help command handling
        } else {
            output << "Unknown command" << endl;
        }
    }
}


int main(int argc, char *argv[]) {
    // TODO: add unit tests
    // TODO: add comments
    // TODO: add == operation
    // TODO: add merge
    // TODO: change mother and father order (father should be first c:)
    if (argc > 2) {
        cout << "Too many command-line arguments: " << argc - 1 << endl;
        return 0;
    }
    string start_filename;
    if (argc == 2) {
        start_filename = argv[1];
    }
    Run(cin, cout, start_filename);
//    vector<FamilyTree::BirthInfo> birth = {{"Paul", "Eve", "Adam"},
//                                           {"Monica", "Eve", "Adam"},
//                                           {"Ivan", "Eve", "Adam"},
//                                           {"Alex", "Monica", "Paul"}};
//    FamilyTree::Tree family_tree(begin(birth), end(birth));
//    family_tree.RenderTo("test.svg");

    return 0;
}
