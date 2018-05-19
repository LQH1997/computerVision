#ifndef _CANNY_
#define _CANNY_

#include <vector>
#include <iostream>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

class canny {
private:
    CImg<unsigned char> inputImg;
    CImg<unsigned char> grayscaled;
    CImg<unsigned char> gaussianFiltered;
    CImg<unsigned char> xGradientFiltered; //Gradient X axis
    CImg<unsigned char> yGradientFiltered; //Gradient Y axis
    CImg<unsigned char> sobelFiltered;
    CImg<unsigned char> angles;
    CImg<unsigned char> non; //NoneMaximun Suppression
    CImg<unsigned char> thres;

public:
    int lowThreshold;
    int highThreshold;
    canny(string path, int size, int sigma, int lowThreshold, int highThreshold);

    //convert the image to a greyScaled image
    CImg<unsigned char> toGrayScale(int width, int height);

    //creata A Gaussian Kernel with the demand size;
    vector<vector<double> > createGaussianKernel(int, int, double);

    //use the GaussianKernel created above to filte
    CImg<unsigned char> gaussianFilter(CImg<unsigned char>, vector<vector<double> >);

    //using sobel to filte
    CImg<unsigned char> sobel();

    //NoneMaximun Suppression
    CImg<unsigned char> nonMaxSupp();

    //Hysteresis Thresholding
    CImg<unsigned char> hysteresis(CImg<unsigned char>);
};


#endif
