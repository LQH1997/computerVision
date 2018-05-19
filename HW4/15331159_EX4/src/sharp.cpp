#include "sharp.h"
#include <iostream>
#include <vector>
#include "CImg.h"
#include <cmath>
#include <string>
using namespace std;
sharp::sharp(char* filename, CImg<unsigned char> inputImg, vector<point>inputCorners) {
  this->filename = filename;
  this->inputImg = inputImg;
  this->corners = inputCorners;
  sortPoints();
  rotate();
  string temp1 = filename;
  string temp = (char*) "answer_";
  temp1.insert(8, temp);
  sharpedImage.save(temp1.c_str());
}

void sharp::sortPoints() {
  for(int i = 0; i < 4; i++) {
    for(int q = 0; q < 3; q++) {
      if(corners[q].y > corners[q+1].y) {
        int tempX = corners[q+1].x;
        int tempY = corners[q+1].y;
        corners[q+1].x = corners[q].x;
        corners[q+1].y = corners[q].y;
        corners[q].x = tempX;
        corners[q].y = tempY;
      }
    }
  }
  if(corners[0].x > corners[1].x) {
    int tempX = corners[1].x;
    int tempY = corners[1].y;
    corners[1].x = corners[0].x;
    corners[1].y = corners[0].y;
    corners[0].x = tempX;
    corners[0].y = tempY;
  }
  if(corners[2].x > corners[3].x) {
    int tempX = corners[2].x;
    int tempY = corners[2].y;
    corners[2].x = corners[3].x;
    corners[2].y = corners[3].y;
    corners[3].x = tempX;
    corners[3].y = tempY;
  }
  double yDis = corners[1].y-corners[0].y;
  double xDis = corners[1].x - corners[0].x;
  theta = yDis / xDis;
  cout << "y distance: " << yDis << endl;
  cout << "x distamce: " << xDis << endl;
  cout << "theta: " << theta << endl;

}

void sharp::rotate() {
  // theta = 1;
  double angle = atan(this->theta);
  for(int q = 0; q < 4; q++) {
    double x1 = corners[q].x;
    double y1 = corners[q].y;
    rotatedCorners.push_back(point(x1*cos(-angle)-y1*sin(-angle), x1*sin(-angle)+y1*cos(-angle)));
  }

  cout << "angle: " << angle << endl;

  CImg<unsigned char> tempImg(inputImg._width*2, inputImg._height*2, 1, 3, 0);
  cimg_forXY(tempImg, x, y) {
    double x1, y1;
    x1 = x*cos(angle)-y*sin(angle);
    y1 = x*sin(angle)+y*cos(angle);
    if(x1 < 0 || x1 > inputImg._width || y1 < 0 || y1 > inputImg._height) continue;
    else {
      tempImg(x,y,0) = bilinearInt(x1, y1, 0);
      tempImg(x,y,1) = bilinearInt(x1, y1, 1);
      tempImg(x,y,2) = bilinearInt(x1, y1, 2);
    }
  }
  // tempImg.display("rotate");
  crop(tempImg);
}

void sharp::crop(CImg<unsigned char> tempImg) {
  int minX =100000, minY = 1000000, maxX = -10, maxY = -10;
  for(int i = 0; i < 4; i++) {
    if(rotatedCorners[i].x <minX) minX = rotatedCorners[i].x;
    if(rotatedCorners[i].x > maxX) maxX = rotatedCorners[i].x;
    if(rotatedCorners[i].y < minY) minY = rotatedCorners[i].y;
    if(rotatedCorners[i].y > maxY) maxY = rotatedCorners[i].y;
  }
  // int minX1 = x
  minX -= 10;
  minY -= 10;
  maxX += 10;
  maxY += 10;
  cout << "aaa" << endl;
  CImg<unsigned char> sharpedImage(maxX-minX+20, maxY-minY+20, 1, 3, 0);
  cimg_forXY(sharpedImage, x, y) {
      sharpedImage(x, y, 0) = tempImg(x+minX, y+minY, 0);
      sharpedImage(x, y, 1) = tempImg(x+minX, y+minY, 1);
      sharpedImage(x, y, 2) = tempImg(x+minX, y+minY, 2);
  }
  sharpedImage.display("final");
}

unsigned char sharp::bilinearInt(double x, double y, int channel) {
  int x0 = x / 1;
  int x1 = (x+1) / 1;
  int y0 = y / 1;
  int y1 = (y+1) / 1;
  int a = x - x0;
  int b = y - y0;
  double result = a*b*inputImg(x0, y0, channel) + a*(1-b)*inputImg(x1, y0, channel)
                 +(1-a)*b*inputImg(x0, y1, channel) + (1-a)*(1-b)*inputImg(x1, y1, channel);
  return (unsigned char) result;
}

CImg<unsigned char> sharp::returnSharpedImage() {
  return sharpedImage;
}
