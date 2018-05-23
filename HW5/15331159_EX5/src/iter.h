#ifndef _ITER_H
#define _ITER_H

#include "CImg.h"
#include <iostream>
#include <string>
#include <cmath>

using namespace std;
using namespace cimg_library;

class iter {
  public:
    iter(string str);
    iter();
    void startSeg(string str);
    void findIntensity();
    void toGrayScale();
    void compute();
    void doSegmentation();
    void saveImg();
  private:
    int height;
    int width;
    int countStep;

    double delta;
    double threshold;

    int intensity[255];
    double c1MeanIntensity;
    double c2MeanIntensity;

    string path;

    CImg<unsigned char> input;
    CImg<unsigned char> grayScaled;
    CImg<unsigned char> output;
};

#endif
