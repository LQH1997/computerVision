

#ifndef _CANNY_H
#define _CANNY_H


#include "CImg.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace cimg_library;

class canny {
private:
    CImg<unsigned char> img; //Original Image
    CImg<unsigned char> grayscaled; // Grayscale
    CImg<unsigned char> gFiltered; // Gradient
    CImg<unsigned char> sFiltered; //Sobel Filtered
    CImg<float> angles; //Angle Map
    CImg<unsigned char> non; // Non-maxima supp.
    CImg<unsigned char> thres; //Double threshold and final
public:

    canny(CImg<unsigned char> inputImg, int size = 3, int low = 20, int high = 40); //Constructor
    CImg<unsigned char> toGrayScale();
    vector<vector<double> > createFilter(int, int, double); //Creates a gaussian filter
    CImg<unsigned char> useFilter(CImg<unsigned char>, vector<vector<double> >); //Use some filter
    CImg<unsigned char> sobel(); //Sobel filtering
    CImg<unsigned char> nonMaxSupp(); //Non-maxima supp.
    CImg<unsigned char> threshold(CImg<unsigned char>, int, int); //Double threshold and finalize picture
    CImg<unsigned char> returenResult();
    CImg<unsigned char> returnGuassianResult();
};

#endif
