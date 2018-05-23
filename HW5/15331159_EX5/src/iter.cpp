#include "iter.h"

iter::iter() {
  //do nothing
}

iter::iter(string str) {
  path = str;
  startSeg(str);
}

void iter::startSeg(string str) {
  input.load(str.c_str());
  height = input.height();
  width = input.width();
  delta = 0.5;
  countStep = 0;
  threshold = 128;
  toGrayScale();
  findIntensity();
  compute();
  doSegmentation();
  saveImg();
}

void iter::toGrayScale() {
  unsigned char r, g, b;
  unsigned char gray;
  grayScaled = CImg<unsigned char>(width, height, 1, 1);
  for(int i = 0; i < width; i++) {
    for(int j = 0; j < height; j++) {
      r = input(i, j, 0, 0);
      g = input(i, j, 0, 1);
      b = input(i, j, 0, 2);
      gray = round(0.3*((double)r) + 0.6*((double)g)+0.1*((double)b));
      grayScaled(i,j,0,0) = gray;
    }
  }
}

void iter::findIntensity() {
  for(int i = 0; i < 256; i++) {
    intensity[i] = 0;
  }

  cimg_forXY(grayScaled, x, y) {
    int temp = (int) grayScaled(x, y);
    intensity[temp] += 1;
  }
}

void iter::compute() {
  double diff = 10000;
  countStep = 0;
  while(diff > delta && countStep < 100) {
    double newThreshold = 0;
    c1MeanIntensity = 0;
    c2MeanIntensity = 0;
    int sum1 = 0;
    int sum2 = 0;
    for(int i = 0; i < 256; i++) {
      if(i <= threshold) {
        c1MeanIntensity += i * intensity[i];
        sum1 += intensity[i];
      } else {
        c2MeanIntensity += i * intensity[i];
        sum2 += intensity[i];
      }
    }
    c1MeanIntensity /= sum1;
    c2MeanIntensity /= sum2;
    newThreshold = (c1MeanIntensity + c2MeanIntensity) / 2;
    diff = abs(threshold - newThreshold);
    threshold = newThreshold;
    countStep += 1;
  }

}

void iter::doSegmentation() {
  output = CImg<unsigned char>(width, height, 1, 1);
  cimg_forXY(grayScaled, x, y) {
    if(grayScaled(x,y) <= threshold) {
      output(x,y) = 0;
    } else {
      output(x,y) = 255;
    }
  }
}

void iter::saveImg() {
  string head = "result_iter";
  string tail = path.replace(3,7,head);
  // tail = tail.replace(tail.find("jpg"),3,"bmp");
  cout << "iter: " << tail << endl;
  output.save(tail.c_str());
}
