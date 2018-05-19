#include <iostream>
#include "canny.h"
#include "HoughTransform.h"
#include "sharp.h"

using namespace std;


int main() {
  // int method = 2;
    // char file[2][6][30] = {"dataset1/1.bmp","dataset1/2.bmp","dataset1/3.bmp","dataset1/4.bmp",
    // "dataset1/5.bmp","dataset1/6.bmp","dataset2/1.bmp","dataset2/2.bmp","dataset2/3.bmp",
    // "dataset2/4.bmp","dataset2/5.bmp","dataset2/6.bmp"};
    // for(int q = 0; q < 2; q++) {
    //     for(int i = 0; i < 6; i++) {
    //       if(q == 1 && i == 5) break;
    //     HoughTransform houghTransform(file[q][i], q+1);
    //   }
    //     // houghTransform.findLine();
    //   }
      // cout << "aaaaa" << endl;
    int method = 1;
    char file[6][30]={"dataset/1.bmp","dataset/2.bmp","dataset/3.bmp","dataset/4.bmp",
    "dataset/5.bmp","dataset/6.bmp"};
    for(int i = 0; i < 6; i++) {
      HoughTransform houghTransform(file[i], 1);
      vector<point> corners = houghTransform.returnIntersections();
      sharp shp(file[i], houghTransform.returnOriginImage(), houghTransform.returnIntersections());

      // for(int q = 0; q < corners.size(); q++) {
      //   cout << corners[q].x << " " << corners[q].y << endl;
      //
    }


    return 0;
}
