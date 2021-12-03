#include "test_tree.h"
#include "user_interface.h"

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>

using namespace std;


int main(int argc, char* argv[]) {
    TestAll();
    if (argc == 1) {
        RunInteraction();
    } else if (argc == 2) {
        RunInteraction(argv[1]);
    } else {
        cout << "Too many command line arguments, should be 1 or 0!" << endl;
    }
    return 0;
}
