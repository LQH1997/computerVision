#ifndef _OSTU_H
#define _OSTU_H
#include "CImg.h"
#include <iostream>
#include <string>
#include <vector>

#define PI 3.14

using namespace std;
using namespace cimg_library;

class ostu {
  public:
    ostu(string str);
    ostu();
    void startSeg(string str);
    CImg<unsigned char> toGrayScale();
    void findIntensity();
    void normalizeIntensity();
    void Otsu();
    void doSegmentation();
    void createGaussianKernel(int row, int col, double sigma);
    void gaussianFilter();
    void saveImg();
  private:
    int height;
    int width;

    double lowThreshold;
    double highThreshold;
    double c1Prob;
    double c2Prob;
    double c3Prob;

    double c1MeanIntensity;
    double c2MeanIntensity;
    double c3MeanIntensity;
    double globalIntensity;

    double globalVar;
    double betweenClassVar;
    double m;
    double yita;

    double bestLowThreshold;
    double bestHighThreshold;
    double bestVar;

    CImg<unsigned char> input;
    CImg<unsigned char> output;

    CImg<unsigned char> filtedImg;

    CImg<unsigned char> grayScaled;
    int intensity[256];
    double intensity_norm[256];

    vector<vector<double>> filter;
    // char* fileP;

    string path;
};


#endif
