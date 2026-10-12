#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include "PGMimageProcessor.h"

int main(int argc, char *argv[]) {
    if (argc < 2 || (argc == 2 && std::strcmp(argv[1], "--help") == 0)) {
        std::cout << "Usage: components [-t 1..255] [-s min max] [-p] [-w output.pgm] input.pgm\n";
        return argc < 2 ? 2 : 0;
    }
    try {
        int threshold = 128, minimum = 3, maximum = std::numeric_limits<int>::max();
        bool print = false;
        std::string output;
        auto number = [](const char *value) {
            std::size_t consumed = 0;
            int result = std::stoi(value, &consumed);
            if (consumed != std::strlen(value)) throw std::invalid_argument("Invalid numeric option");
            return result;
        };
        for (int i = 1; i < argc - 1; ++i) {
            std::string option = argv[i];
            if (option == "-t") {
                if (i + 1 >= argc - 1) throw std::invalid_argument("Missing threshold");
                threshold = number(argv[++i]);
            } else if (option == "-s") {
                if (i + 2 >= argc - 1) throw std::invalid_argument("Missing component-size bounds");
                minimum = number(argv[++i]);
                maximum = number(argv[++i]);
            } else if (option == "-w") {
                if (i + 1 >= argc - 1) throw std::invalid_argument("Missing output path");
                output = argv[++i];
            } else if (option == "-p") print = true;
            else throw std::invalid_argument("Unknown option: " + option);
        }
        if (threshold < 1 || threshold > 255 || minimum < 1 || maximum < minimum)
            throw std::invalid_argument("Threshold or component-size bounds are out of range");
        image_processing::PGMimageProcessor image(argv[argc - 1]);
        image.extractComponents(static_cast<unsigned char>(threshold), minimum);
        image.filterComponentsBySize(minimum, maximum);
        if (print) image.printAllComponents();
        if (!output.empty() && !image.writeComponents(output)) throw std::runtime_error("Could not write output");
        std::cout << "Done!\n";
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 2;
    }
}
