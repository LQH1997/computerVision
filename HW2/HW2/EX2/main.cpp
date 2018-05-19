#include <string>
#include <vector>
#include <iostream>
#include "CImg.h"
#include "canny.h"

using namespace cimg_library;
using namespace std;

int main() {

    string filePath[4] = {"testData/lena.bmp", "testData/bigben.bmp", "testData/s.bmp", "testData/twows.bmp"};
    for(int i = 0; i < 4; i++) {
      //four parameters are filePath of the picture, the size of the gaussianKernel,
      //sigme, lowThreshold, highThreshold, respectively.
      canny test(filePath[i], 5, 5, 40, 160);
    }
    return 0;
}
