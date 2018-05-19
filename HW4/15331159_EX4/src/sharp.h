#ifndef _SHARP_H
#define _SHARP_H

#include "HoughTransform.h"
#include "canny.h"
#include <vector>
#include <iostream>

class sharp {
  public:
    sharp(char* filename, CImg<unsigned char> inputImg, vector<point> inputCorners);
    void rotate();
    void sortPoints();
    int sqr(int x1, int x2, int y1, int y2);
    unsigned char bilinearInt(double x, double xy, int channel);
    void crop(CImg<unsigned char> tempImg);
    CImg<unsigned char> returnSharpedImage();

  private:
    int height;
    int width;
    char* filename;
    CImg<unsigned char> sharpedImage;
    CImg<unsigned char> inputImg;
    CImg<unsigned char> returnOriginImage();
    vector<point> corners;
    vector<point> rotatedCorners;
    double theta;
};



#endif
