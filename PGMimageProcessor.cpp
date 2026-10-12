//Azhar Abdool
//Assignment 3
//

#include "PGMimageProcessor.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <queue>
#include <memory>
#include <stdexcept>
#include <cctype>
#include "ConnectedComponent.h"


namespace image_processing {
    // Portfolio enhancement: validate P5 headers before allocating or reading pixels.
    PGMimageProcessor::PGMimageProcessor(const std::string & imgName) {
        std::ifstream in(imgName, std::ios::binary);
        if (!in) throw std::runtime_error("Cannot open input image");
        auto token = [&]() {
            std::string value;
            while (in >> value) {
                if (value[0] == '#') { std::getline(in, value); continue; }
                return value;
            }
            throw std::runtime_error("Incomplete PGM header");
        };
        if (token() != "P5") throw std::runtime_error("Only binary P5 PGM is supported");
        auto integer = [&]() {
            auto value = token();
            std::size_t consumed = 0;
            int result = std::stoi(value, &consumed);
            if (consumed != value.size()) throw std::runtime_error("Invalid PGM integer");
            return result;
        };
        int width = integer(), height = integer(), maximum = integer();
        if (width < 1 || height < 1 || width > 10000 || height > 10000 ||
            static_cast<long long>(width) * height > 50000000 || maximum != 255)
            throw std::runtime_error("Unsupported PGM dimensions or maximum value");
        char separator;
        if (!in.get(separator) || !std::isspace(static_cast<unsigned char>(separator)))
            throw std::runtime_error("Missing raster separator");
        if (separator == '\r' && in.peek() == '\n') in.get();
        int size = width * height;
        auto buffer = std::make_unique<unsigned char[]>(size);
        if (!in.read(reinterpret_cast<char *>(buffer.get()), size))
            throw std::runtime_error("Truncated PGM raster");
        imgWidth = width; imgHeight = height; imgMatrix = buffer.release();
    }

//Copy Constructor
PGMimageProcessor::PGMimageProcessor(const PGMimageProcessor &rhs) : components(rhs.components), imgWidth(rhs.imgWidth), imgHeight(rhs.imgHeight)
{

    int totalSize = PGMimageProcessor::imgWidth*PGMimageProcessor::imgHeight;//copy imgMatrix manually
    PGMimageProcessor::imgMatrix = new unsigned char[totalSize];
    for(int i =0; i< totalSize; ++i)
    {
        PGMimageProcessor::imgMatrix[i] = rhs.imgMatrix[i]; //Copy to new object
    }
}

//Move constructor
PGMimageProcessor::PGMimageProcessor(PGMimageProcessor && rhs) : components(std::move(rhs.components)), imgWidth(rhs.imgWidth), imgHeight(rhs.imgHeight)
{
    imgMatrix = rhs.imgMatrix; //copy over address
    rhs.imgMatrix = nullptr; //set old objects pointer to null
}

//Copy Assignment Operator
PGMimageProcessor & PGMimageProcessor::operator=(const PGMimageProcessor & rhs){
    if(this != &rhs)
    {
        PGMimageProcessor::components = rhs.components;
        PGMimageProcessor::imgWidth = rhs.imgWidth;
        PGMimageProcessor::imgHeight = rhs.imgHeight;
        int totalSize = rhs.imgWidth*rhs.imgHeight;
        if(PGMimageProcessor::imgMatrix) {delete [] PGMimageProcessor::imgMatrix;}
        PGMimageProcessor::imgMatrix = new unsigned char[totalSize]; //Remake char array in case image has a different size
        for(int i =0; i< totalSize; ++i)
        {
            PGMimageProcessor::imgMatrix[i] = rhs.imgMatrix[i]; //copy over imgMatrix
        }
    }
    return *this;
}

//Move Assignment Operator
PGMimageProcessor & PGMimageProcessor::operator=(PGMimageProcessor && rhs){
    if(this != &rhs)
    {
        PGMimageProcessor::components = std::move(rhs.components);
        PGMimageProcessor::imgWidth = rhs.imgWidth;
        PGMimageProcessor::imgHeight = rhs.imgHeight;
        if(PGMimageProcessor::imgMatrix) {delete [] PGMimageProcessor::imgMatrix;}
        imgMatrix = rhs.imgMatrix;
        rhs.imgMatrix = nullptr; //Sets old pointer to null
    }
    return *this;
}

//Destructor
PGMimageProcessor::~PGMimageProcessor(void) {
    if(PGMimageProcessor::imgMatrix)
    {
        delete [] PGMimageProcessor::imgMatrix;
    }
}



//extract components from the image
int PGMimageProcessor::extractComponents(unsigned char threshold, int minValidSize){
    // Zero is the submitted traversal's visited marker, so it cannot be foreground.
    if (threshold == 0) throw std::invalid_argument("Threshold must be at least one");

    for(int y=0; y<PGMimageProcessor::imgHeight; ++y) //Row number
    {

        for(int x=0; x<PGMimageProcessor::imgWidth; ++x) //Column number
        {

            //Iterate across pixels in image to find components
            if(PGMimageProcessor::imgMatrix[y*PGMimageProcessor::imgWidth + x] >= threshold )
            {

                std::queue< std::pair<int,int> > q;

                // Append the position of starting pixel of the component to the queue
                std::pair<int,int> point(x,y);
                q.push(point);

                auto comp = std::make_unique<ConnectedComponent>();

                comp->addCoord(point); //Add the starting coordinate to the component

                PGMimageProcessor::imgMatrix[y*PGMimageProcessor::imgWidth + x] = 0; //Set pixel to 0 to mark that it's been visited


                // While the queue is not empty
                while(q.size() > 0)
                {


                    std::pair<int,int> currPixel = q.front();
                    q.pop();

                    int posX = currPixel.first;
                    int posY = currPixel.second;

                    // Check if the adjacent
                    // pixels are valid
                    if(PGMimageProcessor::isValidPixel(threshold, posX + 1, posY))
                    {

                        PGMimageProcessor::imgMatrix[posY*PGMimageProcessor::imgWidth + (posX+1)] = 0; //Mark pixel as visited

                        point.first = posX + 1;
                        point.second = posY;
                        comp->addCoord(point); //Add point to component
                        q.push(point);//Add point to the queue
                    }

                    if(PGMimageProcessor::isValidPixel(threshold, posX - 1, posY))
                    {

                        PGMimageProcessor::imgMatrix[posY*PGMimageProcessor::imgWidth + (posX-1)] = 0;//Mark pixel as visited

                        point.first = posX - 1;
                        point.second = posY;
                        comp->addCoord(point);
                        q.push(point);//Add point to the queue
                    }

                    if(PGMimageProcessor::isValidPixel(threshold, posX, posY+1))
                    {

                        PGMimageProcessor::imgMatrix[(posY+1)*PGMimageProcessor::imgWidth + posX] = 0;//Mark pixel as visited

                        point.first = posX;
                        point.second = posY+1;
                        comp->addCoord(point);//Add point to component
                        q.push(point); //Add point to the queue
                    }

                    if(PGMimageProcessor::isValidPixel(threshold, posX, posY-1))
                    {

                        PGMimageProcessor::imgMatrix[(posY-1)*PGMimageProcessor::imgWidth + posX] = 0;//Mark pixel as visited

                        point.first = posX;
                        point.second = posY-1;
                        comp->addCoord(point);//Add point to component
                        q.push(point); //Add point to the queue
                    }
                }

                comp->countBoundaryPixels(); //Count the boundary pixels of the component - sets it internally
                PGMimageProcessor::components.insert(std::move(*comp)); //Add the component to the multiset of components

            }
            else{
                //Coord was below the threshold
                PGMimageProcessor::imgMatrix[y*PGMimageProcessor::imgWidth + x] = 0; //Mark coord as visited
            }
        }
        //Iterate across all the pixels in image to find components
    }
    int imgSize = PGMimageProcessor::imgHeight*PGMimageProcessor::imgWidth;
    return PGMimageProcessor::filterComponentsBySize(minValidSize, imgSize); //Returns number of components above the threshold found
}

bool PGMimageProcessor::isValidPixel(unsigned char threshold, int x, int y){
    //Helper function for extracting the components
    if( x<0 || x>=PGMimageProcessor::imgWidth || y<0 || y>=PGMimageProcessor::imgHeight ||
    PGMimageProcessor::imgMatrix[y*PGMimageProcessor::imgWidth + x] < threshold ){
        return false;
    }
    return true;
}

int PGMimageProcessor::filterComponentsBySize(int minSize, int maxSize){
    //Filters out components which are too small or too large
    for(auto i = PGMimageProcessor::components.begin();
        i != PGMimageProcessor::components.end();)
    {
        if(i->getSize() < minSize || i->getSize() > maxSize)
        {

            i = PGMimageProcessor::components.erase(i);
        }
        else ++i;
    }

    return PGMimageProcessor::components.size();

}
int PGMimageProcessor::getComponentCount(void) const{
    return PGMimageProcessor::components.size(); //Returns the number of components in the image
}


bool PGMimageProcessor::writeComponents(const std::string & outFileName){//Update imgMatrix
    if(PGMimageProcessor::imgMatrix) {

        for(std::multiset<ConnectedComponent>::iterator i = PGMimageProcessor::components.begin();
            i != PGMimageProcessor::components.end(); ++i)
        {
            //Iterating over the multiset to access each component
            const std::vector<std::pair<int,int>> & coords = i->getPairs();
            int pairCount = i->getSize();
            for(int a=0; a<pairCount;++a){ //Gets each coordiante within the component
                int x = coords[a].first;
                int y = coords[a].second;
                PGMimageProcessor::imgMatrix[y*PGMimageProcessor::imgWidth+x] = 255; //Recolour those coordiantes to white
            }
        }

        //Write the outputfile
        std::ofstream outFile(outFileName, std::ios::out | std::ios::binary);
        outFile << "P5" << "\n";
        outFile << "#Created by driver program" << "\n";
        outFile << PGMimageProcessor::imgWidth << " " << PGMimageProcessor::imgHeight << "\n";
        outFile << "255" << "\n";
        int totalSize = PGMimageProcessor::imgWidth*PGMimageProcessor::imgHeight;
        if(!outFile.write((char *)PGMimageProcessor::imgMatrix, totalSize)) //Writes new imgMatrix to a file
        {
            outFile.close();
            return false; //Return false if writing failed
        }
        outFile.close();
        return true; //writing successful
    }
    return true;
}

int PGMimageProcessor::getLargestSize() const {
    int largest = 0;
    for(std::multiset<ConnectedComponent>::const_iterator  i = PGMimageProcessor::components.begin();
        i != PGMimageProcessor::components.end(); ++i)
    {
        if(i->getSize() >largest)
        {
            largest = i->getSize();
        }
    }

    return largest;
}

int PGMimageProcessor::getSmallestSize() const {
    int smallest = PGMimageProcessor::imgWidth*PGMimageProcessor::imgHeight;
    for(std::multiset<ConnectedComponent>::const_iterator i = PGMimageProcessor::components.begin();
        i != PGMimageProcessor::components.end(); ++i)
    {
        if(i->getSize() < smallest)
        {
            smallest = i->getSize();
        }
    }

    return smallest;
}

void PGMimageProcessor::printComponentData(const ConnectedComponent & theComponent) const{
    std::cout << theComponent.getID() << ", " << theComponent.getSize() << ", " << theComponent.getBoundaryPixels() << std::endl;
}

void PGMimageProcessor::printAllComponents() {
    std::cout << "All Component Data: \n" << "[ID, Total Pixels, Boundary Pixels] \n";
    for(std::multiset<ConnectedComponent>::iterator i = PGMimageProcessor::components.begin();
        i != PGMimageProcessor::components.end(); ++i)
    {
        PGMimageProcessor::printComponentData(*i);
    }

    std::cout << "Total number of components = " << PGMimageProcessor::getComponentCount() << std::endl;
    std::cout << "Size of smallest component = " << PGMimageProcessor::getSmallestSize() << std::endl;
    std::cout << "Size of largest component = " << PGMimageProcessor::getLargestSize() << std::endl;

}


bool PGMimageProcessor::compareComponents::operator()(const ConnectedComponent & lhs, const ConnectedComponent & rhs) const
{
    return lhs.getSize() < rhs.getSize();
}

int PGMimageProcessor::getImgHeight(){
    return PGMimageProcessor::imgHeight;
}

int PGMimageProcessor::getImgWidth(){
    return PGMimageProcessor::imgWidth;
}

int PGMimageProcessor::getImgSize(){
    return PGMimageProcessor::imgWidth*PGMimageProcessor::imgHeight;
}

unsigned char * PGMimageProcessor::getImgMatrix(){
    return PGMimageProcessor::imgMatrix;
}

int PGMimageProcessor::getTotalBoundaryPixels(){
    int total = 0;
    for(std::multiset<ConnectedComponent>::iterator i = PGMimageProcessor::components.begin();
        i != PGMimageProcessor::components.end(); ++i)
    {
        total += i->getBoundaryPixels();
    }
    return total;
}

} //namespace
