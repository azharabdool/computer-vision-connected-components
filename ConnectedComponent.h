//Azhar Abdool
//Assignment 3
//

#ifndef ASSIGNMENT_3_CONNECTEDCOMPONENT_H
#define ASSIGNMENT_3_CONNECTEDCOMPONENT_H

#include <vector>

namespace image_processing {

// Class representing a connected component in an image
class ConnectedComponent {
    int id;                               // ID of the component
    int pixelCount;                       // Total number of pixels in the component
    std::vector<std::pair<int, int>> componentCoords;  // Coordinates of pixels in the component
    static int IDcount;                   // Static counter for assigning unique IDs
    int boundaryPixels;                  // Number of boundary pixels in the component

public:
    ConnectedComponent();                                        // Constructor
    ConnectedComponent(const ConnectedComponent &rhs);            // Copy constructor
    ConnectedComponent(ConnectedComponent &&rhs) noexcept;        // Move constructor
    ConnectedComponent &operator=(const ConnectedComponent &rhs); // Copy assignment operator
    ConnectedComponent &operator=(ConnectedComponent &&rhs);      // Move assignment operator
    ~ConnectedComponent();                                       // Destructor

    void addCoord(std::pair<int, int> coords);                   // Add coordinate to the component
    int getSize() const;                                         // Get the size of the component
    int getID() const;                                           // Get the ID of the component
    const std::vector<std::pair<int, int>> &getPairs() const;    // Get the vector of coordinate pairs
    bool isBoundary(std::pair<int, int> p);                      // Check if a pixel is on the boundary
    void countBoundaryPixels();                                  // Count the number of boundary pixels
    int getBoundaryPixels() const;                               // Get the number of boundary pixels
};

} // namespace

#endif // ASSIGNMENT_3_CONNECTEDCOMPONENT_H
