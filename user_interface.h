#pragma once

#include <iostream>


void RunInteraction(const std::string& start_filename = "",
                    std::istream& command_stream = std::cin, std::ostream& output = std::cout);
