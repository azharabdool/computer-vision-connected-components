// New portfolio verification on an authored fixture, not submitted coursework tests.
#include "PGMimageProcessor.h"
#include <iostream>
#include <stdexcept>

int main() {
    int checks = 0;
    auto require = [&checks](bool condition) {
        if (!condition) throw std::runtime_error("Synthetic component check failed");
        ++checks;
    };
    try {
        image_processing::PGMimageProcessor image("examples/synthetic-input.pgm");
        require(image.getImgWidth() == 144 && image.getImgHeight() == 96);
        require(image.extractComponents(128, 20) == 2);
        require(image.getLargestSize() == 625);
        require(image.getSmallestSize() == 600);
        require(image.filterComponentsBySize(20, 610) == 1);
        require(image.getLargestSize() == 600);
        std::cout << checks << " synthetic checks passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
