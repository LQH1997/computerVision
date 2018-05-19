#include <iostream>
#include <cmath>
#include <vector>
#include "canny.h"
#include "CImg.h"

#define PI 3.14

using namespace std;
using namespace cimg_library;

canny::canny(string filename, int size, int sigma, int lowThr, int highThr) {
    this->lowThreshold = lowThr;
    this->highThreshold = highThr;
    const char* file = filename.c_str();
    inputImg.load_bmp(file);

        //construct a size*size gaussian Kernel
        vector<vector<double> > filter = createGaussianKernel(size, size, sigma);

        //convert the origin image to a greyScaled Image
        grayscaled = toGrayScale(inputImg._width, inputImg._height);

        //Gaussian filter
        gaussianFiltered = gaussianFilter(grayscaled, filter);

        //Sobel Filter
        sobelFiltered = sobel(); //Sobel

        //Nonemaxmun Suppression
        non = nonMaxSupp();

        //threshold
        thres = hysteresis(non);

        inputImg.display();
        grayscaled.display("greyScaled");
        gaussianFiltered.display("gaussian");
        xGradientFiltered.display("x");
        yGradientFiltered.display("y");
        sobelFiltered.display("sobel");
        non.display("nonmaxmiun suppression");
        thres.display("dest Img");
}

CImg<unsigned char> canny::toGrayScale(int width, int height) {
    grayscaled = CImg<unsigned char>(width, height, 1, 1, 0);


    cimg_forXY(inputImg, x, y) {
        grayscaled(x,y) = inputImg(x, y, 0) * 0.299 + inputImg(x, y, 1) * 0.587 + inputImg(x, y, 2) * 0.114;
    }
    return grayscaled;
}

//Generate size*size Gaussian Kernel
vector<vector<double> > canny::createGaussianKernel(int row, int column, double sigmaIn) {
    vector<vector<double> > filter;

    for (int i = 0; i < row; i++) {
        vector<double> col;
        for (int j = 0; j < column; j++) {
            col.push_back(-1);
        }
        filter.push_back(col);
    }

    float coordSum = 0;
    float constant = 2.0 * sigmaIn * sigmaIn;

    float sum = 0.0;

    //calculate each value in the kernel

    for(int i = 0; i < row; i++) {
      for(int j = 0; j < column; j++) {
        filter[i][j] = (exp(-((i-row/2)*(i-row/2) + (j-column/2)*(j-column/2))/constant))/(PI * constant);
        sum += filter[i][j];
      }
    }

    // normalize to make sure the sum equal to 1
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            filter[i][j] /= sum;
          }
        }

    return filter;
}

//Gaussian
CImg<unsigned char> canny::gaussianFilter(CImg<unsigned char> img_in, vector<vector<double> > fiilter) {
    int size = (int)fiilter.size() / 2;
    CImg<unsigned char> filteredImg = CImg<unsigned char>(img_in._width - 2 * size, img_in._height - 2 * size, 1,1,0);

    for (int i = size; i < img_in._width - size; i++) {
        for (int j = size; j < img_in._height - size; j++) {
            double sum = 0;

            for (int x = 0; x < fiilter.size(); x++)
                for (int y = 0; y < fiilter.size(); y++) {
                    sum += fiilter[x][y] * (double)(img_in(i + x - size, j + y - size));
                }

            filteredImg(i - size, j - size) = sum;
        }

    }
    return filteredImg;
}

//sobel
CImg<unsigned char> canny::sobel() {
    //Sobel at X axis
    double x1[] = { -1.0, 0, 1.0 };
    double x2[] = { -2.0, 0, 2.0 };
    double x3[] = { -1.0, 0, 1.0 };

    vector<vector<double> > xFilter(3);
    xFilter[0].assign(x1, x1 + 3);
    xFilter[1].assign(x2, x2 + 3);
    xFilter[2].assign(x3, x3 + 3);

    //Sobel at Y axis
    double y1[] = { 1.0, 2.0, 1.0 };
    double y2[] = { 0, 0, 0 };
    double y3[] = { -1.0, -2.0, -1.0 };

    vector<vector<double> > yFilter(3);
    yFilter[0].assign(y1, y1 + 3);
    yFilter[1].assign(y2, y2 + 3);
    yFilter[2].assign(y3, y3 + 3);

    //Limit Size
    int size = (int)xFilter.size() / 2;

    CImg<unsigned char> filteredImg = CImg<unsigned char>(gaussianFiltered._width - 2 * size, gaussianFiltered._height - 2 * size, 1,1,0);
    xGradientFiltered = CImg<unsigned char>(gaussianFiltered._width - 2 * size, gaussianFiltered._height - 2 * size, 1,1,0);
    yGradientFiltered = CImg<unsigned char>(gaussianFiltered._width - 2 * size, gaussianFiltered._height - 2 * size, 1,1,0);

    angles = CImg<unsigned char>(gaussianFiltered._width - 2 * size, gaussianFiltered._height - 2 * size, 1,1,0);

    for (int i = size; i < gaussianFiltered._width - size; i++) {
        for (int j = size; j < gaussianFiltered._height - size; j++) {
            double sumX = 0;
            double sumY = 0;

            for (int x = 0; x < xFilter.size(); x++) {
                for (int y = 0; y < xFilter.size(); y++) {
                    sumX += xFilter[x][y] * (double)(gaussianFiltered(i + x - size, j + y - size)); //Sobel_X Filter Value
                    sumY += yFilter[x][y] * (double)(gaussianFiltered(i + x - size, j + y - size)); //Sobel_Y Filter Value
                }
              }

            double sq = sqrt(sumX*sumX + sumY*sumY);

            sq > 255 ? sq = 255 : sq = sq;

            filteredImg(i - size, j - size) = sq;
            xGradientFiltered(i - size, j - size) = sumX > 255? 255:sumX;
            yGradientFiltered(i - size, j - size) = sumY > 255? 255:sumY;
            if (sumX == 0) {
                angles(i - size, j - size) = 90;
              }
            else {
                angles(i - size, j - size) = atan(sumY / sumX);
              }
        }
    }

    return filteredImg;

}

//NoneMaximun Suppression
CImg<unsigned char> canny::nonMaxSupp() {
    CImg<unsigned char> nonMaxSupped = CImg<unsigned char>(sobelFiltered._width - 2, sobelFiltered._height - 2, 1,1,0);
    for (int i = 1; i< sobelFiltered._width - 1; i++) {
        for (int j = 1; j<sobelFiltered._height - 1; j++) {
            float Tangent = angles(i, j);

            nonMaxSupped(i - 1, j - 1) = sobelFiltered(i, j);

            if (((-22.5 < Tangent) && (Tangent <= 22.5)) || ((157.5 < Tangent) && (Tangent <= -157.5))) {

                if ((sobelFiltered(i, j) < sobelFiltered(i, j + 1)) || (sobelFiltered(i, j) < sobelFiltered(i, j - 1)))
                    nonMaxSupped(i - 1, j - 1) = 0;
            }

            if (((-112.5 < Tangent) && (Tangent <= -67.5)) || ((67.5 < Tangent) && (Tangent <= 112.5))) {
                if ((sobelFiltered(i, j) < sobelFiltered(i + 1, j)) || (sobelFiltered(i, j) < sobelFiltered(i - 1, j)))
                    nonMaxSupped(i - 1, j - 1) = 0;
            }


            if (((-67.5 < Tangent) && (Tangent <= -22.5)) || ((112.5 < Tangent) && (Tangent <= 157.5))) {
                if ((sobelFiltered(i, j) < sobelFiltered(i - 1, j + 1)) || (sobelFiltered(i, j) < sobelFiltered(i + 1, j - 1)))
                    nonMaxSupped(i - 1, j - 1) = 0;
            }


            if (((-157.5 < Tangent) && (Tangent <= -112.5)) || ((22.5 < Tangent) && (Tangent <= 67.5))) {
                if ((sobelFiltered(i, j) < sobelFiltered(i + 1, j + 1)) || (sobelFiltered(i, j) < sobelFiltered(i - 1, j - 1)))
                    nonMaxSupped(i - 1, j - 1) = 0;
            }
        }
    }
    return nonMaxSupped;
}

//hysteresis Thresholding
CImg<unsigned char> canny::hysteresis(CImg<unsigned char> noneSuppedPic) {

    //the maximun value should not exceed 255
    int low = lowThreshold;
    int high = highThreshold;
    low = low > 255 ? 255: low;
    high = high > 255 ? 255 : low;

    CImg<unsigned char> answer = CImg<unsigned char>(noneSuppedPic._width, noneSuppedPic._height, 1,1,0);

    for (int i = 0; i<noneSuppedPic._width; i++) {
        for (int j = 0; j<noneSuppedPic._height; j++) {
            answer(i, j) = noneSuppedPic(i, j);
            //above the high threshold is considered as edge
            if (answer(i, j) > high) {
                answer(i, j) = 255;
              }
            //below the low threshold is considered not as edge
            else if (answer(i, j) < low) {
                answer(i, j) = 0;
              }
            else {
                bool anyHigh = false;
                bool anyBetween = false;
                for (int x = i - 1; x < i + 2; x++) {
                    for (int y = j - 1; y<j + 2; y++) {
                        if (x <= 0 || y <= 0 || answer._width || y > answer._height) //out of bound
                            continue;
                        else {
                            if (answer(x, y) > high) {
                                answer(i, j) = 255;
                                anyHigh = true;
                                break;
                            }
                            else if (answer(x, y) <= high && answer(x, y) >= low)
                                anyBetween = true;
                        }
                    }
                    if (anyHigh)
                        break;
                }
                if (!anyHigh && anyBetween) {
                    for (int x = i - 2; x < i + 3; x++) {
                        for (int y = j - 1; y<j + 3; y++) {
                            if (x < 0 || y < 0 || x > answer._width || y > answer._height) //Out of bounds
                                continue;
                            else {
                                if (answer(x, y) > high) {
                                    answer(i, j) = 255;
                                    anyHigh = true;
                                    break;
                                }
                            }
                        }
                        if (anyHigh)
                            break;
                    }
                  }
                if (!anyHigh)
                    answer(i, j) = 0;
            }
        }
    }
    return answer;
}
