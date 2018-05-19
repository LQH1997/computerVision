#include "CImg.h"
using namespace cimg_library;
int main() {
  CImg<unsigned char> image;
  //load and display the origin bmp file
  image.load_bmp("1.bmp");
  image.display();

  //define the rgb parameter for each color
  unsigned char red[] = {255, 0, 0}, green[] = {0, 128, 0},
                blue[] = {0, 0, 255}, white[] = {255, 255,255},
                yellow[] = {255, 255, 0};

  int width = image._width;
  int height = image._height;
  
  //create a pic with the same height and width
  CImg<unsigned char> step2(width, height, 1, 3);
  
  //copy each pixel from the origin pic to the new pic
  cimg_forXY(image, x, y) {
      if(image(x,y,0) == image(x,y,1)  && image(x,y,1) == image(x,y,2)
        && image(x,y,1) >= 103) {
	//find the white pixel and change it to red
        step2(x, y, 0) = 255;
        step2(x, y, 1) = 0;
        step2(x, y, 2) = 0;
      } else if(image(x,y,0) == 0 && image(x,y,1) == 0 && image(x,y,2) == 0) {
	//modify black space to green
        step2(x, y, 0) = 0;
        step2(x, y, 1) = 128;
        step2(x, y, 2) = 0;
      } else {
	//copy the rest of the pixels
        step2(x, y, 0) = image(x, y, 0);
        step2(x, y, 1) = image(x, y, 1);
        step2(x, y, 2) = image(x, y, 2);
      }
  }

   step2.display();

  CImg<unsigned char> step3(step2);
  step3.draw_circle(50, 50, 30, blue);
  //draw a circle with radius 30 and located at (50,50)
  step3.display();

  CImg<unsigned char> step4(step3);
  //draw another circle with reaius 3 and located at (50,50)
  step4.draw_circle(50, 50, 3, yellow);
  step4.display();

  //copy the picture of each step and show all of them
//  CImg<unsigned char> all(width*4, height, 1, 3);
//  cimg_forXY(all, x, y) {
//    if(x >= 0 && x < width) {
//      all(x, y, 0) = image(x, y, 0);
//      all(x, y, 1) = image(x, y, 1);
//     all(x, y, 2) = image(x, y, 2);
//
//   } else if(x >= width && x < 2 * width) {
//      all(x, y, 0) = step2(x, y, 0);
//      all(x, y, 1) = step2(x, y, 1);
//      all(x, y, 2) = step2(x, y, 2);
//    } else if(x >= 2*width && x < 3* width) {
//      all(x, y, 0) = step3(x, y, 0);
//      all(x, y, 1) = step3(x, y, 1);
//      all(x, y, 2) = step3(x, y, 2);
//    } else if(x >= 3*width && x < 4*width) {
//      all(x, y, 0) = step4(x, y, 0);
//      all(x, y, 1) = step4(x, y, 1);
//      all(x, y, 2) = step4(x, y, 2);
//    }
//  }

//  all.display();

  return 0;
}
