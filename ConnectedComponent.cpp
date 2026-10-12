//Azhar Abdool
//Assignment 3
//

#include <iostream>
#include <vector>
#include <algorithm>
#include "ConnectedComponent.h"
#include "PGMimageProcessor.h"

namespace image_processing {
    int ConnectedComponent::IDcount = 0;

    // Constructor
    ConnectedComponent::ConnectedComponent() {
        id = IDcount;
        IDcount++;
        pixelCount = 0;
    }

    // Copy constructor
    ConnectedComponent::ConnectedComponent(const ConnectedComponent &rhs) : pixelCount(rhs.pixelCount), componentCoords(rhs.componentCoords), boundaryPixels(rhs.boundaryPixels) {
        id = IDcount; // Need to still have a unique ID
        IDcount++;
    }

    // Move Constructor
    ConnectedComponent::ConnectedComponent(ConnectedComponent &&rhs) noexcept : id(std::move(rhs.id)), pixelCount(rhs.pixelCount), componentCoords(std::move(rhs.componentCoords)), boundaryPixels(rhs.boundaryPixels) {}

    // Copy Assignment Operator
    ConnectedComponent &ConnectedComponent::operator=(const ConnectedComponent &rhs) {
        if (this != &rhs) {
            id = IDcount; // Needs a unique ID
            IDcount++;
            pixelCount = rhs.pixelCount;
            componentCoords = rhs.componentCoords;
            boundaryPixels = rhs.boundaryPixels;
        }
        return *this;
    }

    // Move Assignment Operator
    ConnectedComponent &ConnectedComponent::operator=(ConnectedComponent &&rhs) {
        if (this != &rhs) {
            id = std::move(rhs.id);
            pixelCount = rhs.pixelCount;
            componentCoords = std::move(rhs.componentCoords);
            boundaryPixels = rhs.boundaryPixels;
        }
        return *this;
    }

    void ConnectedComponent::addCoord(std::pair<int, int> coords) {
        componentCoords.push_back(coords);
        pixelCount++;
    }

    int ConnectedComponent::getSize() const {
        return pixelCount;
    }

    int ConnectedComponent::getID() const {
        return id;
    }

    const std::vector<std::pair<int, int>> &ConnectedComponent::getPairs() const {
        return componentCoords; // Returns all the coordinate pairs in the component
    }

    ConnectedComponent::~ConnectedComponent() {
        //default constructor
        //blank


    }

    bool ConnectedComponent::isBoundary(std::pair<int, int> p) {
        // Predicate function for boundary pixel counting
        // Checks if all adjacent pixels are part of the component - if not, pixel is a boundary pixel
        int x = p.first;
        int y = p.second;
        std::pair<int, int> adjPixel = p;
        adjPixel.first = x + 1;
        adjPixel.second = y;
        if (std::find(componentCoords.begin(), componentCoords.end(), adjPixel) == componentCoords.end()) {
            return true;
        }
        adjPixel.first = x - 1;
        adjPixel.second = y;
        if (std::find(componentCoords.begin(), componentCoords.end(), adjPixel) == componentCoords.end()) {
            return true;
        }
        adjPixel.first = x;
        adjPixel.second = y + 1;
        if (std::find(componentCoords.begin(), componentCoords.end(), adjPixel) == componentCoords.end()) {
            return true;
        }
        adjPixel.first = x;
        adjPixel.second = y - 1;
        if (std::find(componentCoords.begin(), componentCoords.end(), adjPixel) == componentCoords.end()) {
            return true;
        }

        return false;
    }

    void ConnectedComponent::countBoundaryPixels() {
        boundaryPixels = std::count_if(componentCoords.begin(), componentCoords.end(),
                                        [this](std::pair<int, int> point) { return isBoundary(point); });
    }

    int ConnectedComponent::getBoundaryPixels() const {
        return boundaryPixels;
    }
}
