//Azhar Abdool
//Assignment 3
//

#ifndef ASSIGNMENT_3_PGMIMAGEPROCESSOR_H
#define ASSIGNMENT_3_PGMIMAGEPROCESSOR_H

#include <vector>
#include <set>
#include <string>
#include "ConnectedComponent.h"

namespace image_processing {

// Class representing a PGM image processor
class PGMimageProcessor {
    // Comparison functor for sorting components
    struct compareComponents {
        bool operator()(const ConnectedComponent &lhs, const ConnectedComponent &rhs) const;
    };

    std::multiset<ConnectedComponent, compareComponents> componentSet; // Multiset to store components
    std::multiset<ConnectedComponent, compareComponents> components;   // Multiset to store filtered components
    int imgWidth, imgHeight;    // Width and height of the image
    unsigned char *imgMatrix;  // Pointer to the image matrix

public:
    PGMimageProcessor(const std::string &imgName);       // Constructor
    PGMimageProcessor(const PGMimageProcessor &rhs);     // Copy constructor
    PGMimageProcessor(PGMimageProcessor &&rhs);          // Move constructor
    PGMimageProcessor &operator=(const PGMimageProcessor &rhs);   // Copy assignment operator
    PGMimageProcessor &operator=(PGMimageProcessor &&rhs);        // Move assignment operator
    ~PGMimageProcessor(void);                               // Destructor

    bool writeComponents(const std::string &outFileName);  // Write components to a file
    int extractComponents(unsigned char threshold, int minValidSize); // Extract components from the image
    bool isValidPixel(unsigned char threshold, int x, int y);  // Check if a pixel is valid
    int filterComponentsBySize(int minSize, int maxSize);   // Filter components by size
    int getComponentCount(void) const;                      // Get the number of components
    int getLargestSize(void) const;                         // Get the size of the largest component
    int getSmallestSize(void) const;                        // Get the size of the smallest component
    void printComponentData(const ConnectedComponent &theComponent) const; // Print data of a specific component
    void printAllComponents();                              // Print data of all components
    int getImgHeight();                                     // Get the height of the image
    int getImgWidth();                                      // Get the width of the image
    int getImgSize();                                       // Get the size of the image
    unsigned char *getImgMatrix();                          // Get the image matrix
    int getTotalBoundaryPixels();                           // Get the total boundary pixels of all components
};

} // namespace

#endif // ASSIGNMENT_3_PGMIMAGEPROCESSOR_H
