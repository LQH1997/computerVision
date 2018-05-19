#include <iostream>
#include "canny.h"
#include "HoughTransform.h"

using namespace std;


int main() {
  // int method = 2;
    char file[2][6][30] = {"dataset1/1.bmp","dataset1/2.bmp","dataset1/3.bmp","dataset1/4.bmp",
    "dataset1/5.bmp","dataset1/6.bmp","dataset2/1.bmp","dataset2/2.bmp","dataset2/3.bmp",
    "dataset2/4.bmp","dataset2/5.bmp","dataset2/6.bmp"};
    for(int q = 0; q < 2; q++) {
        for(int i = 0; i < 6; i++) {
          if(q == 1 && i == 5) break;
        HoughTransform houghTransform(file[q][i], q+1);
      }
        // houghTransform.findLine();
      }


    return 0;
}
