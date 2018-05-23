#include "ostu.h"
#include <string>
ostu::ostu() {
  //do nothing
}

ostu::ostu(string str) {
  // fileP = str;
  // string temp = str;
  path = str;
  startSeg(str);
  // while(threshold)
}

void ostu::startSeg(string str) {
  input.load(str.c_str());
  height = input.height();
  width = input.width();
  lowThreshold = 0;
  highThreshold = 1;
  bestLowThreshold = 0;
  bestHighThreshold = 1;
  bestVar = -100000;
  // input.display();
  toGrayScale();
  createGaussianKernel(5 , 5,  1);
  gaussianFilter();
  findIntensity();
  normalizeIntensity();
  for(int lowTh = 0; lowTh < 255; lowTh++) {
    lowThreshold = lowTh;
    for(int highTh = lowTh + 1; highTh < 255; highTh++) {
      highThreshold = highTh;
      Otsu();
    // threshold += 1;
    }
  }
  doSegmentation();
  saveImg();
}

CImg<unsigned char> ostu::toGrayScale() {
  // int width = input.width();
  // int height = input.height();
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
  // grayScaled.display();
}

//Generate size*size Gaussian Kernel
void ostu::createGaussianKernel(int row, int column, double sigmaIn) {
    // vector<vector<double> > filter;

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

}

//Gaussian
void ostu::gaussianFilter() {
    int size = (int)filter.size() / 2;
    filtedImg = CImg<unsigned char>(width - 2 * size, height - 2 * size, 1,1,0);

    for (int i = size; i < width - size; i++) {
        for (int j = size; j < height - size; j++) {
            double sum = 0;

            for (int x = 0; x < filter.size(); x++)
                for (int y = 0; y < filter.size(); y++) {
                    sum += filter[x][y] * (double)(grayScaled(i + x - size, j + y - size));
                }

            filtedImg(i - size, j - size) = sum;
        }
    }
    // filtedImg.display();
}

void ostu::findIntensity() {
  for(int i = 0; i < 256; i++) {
    intensity[i] = 0;
  }
  cimg_forXY(filtedImg, x, y) {
    int temp = (int) filtedImg(x,y);
    intensity[temp] += 1;
  }
}

void ostu::normalizeIntensity() {
  for(int i = 0; i < 256; i++) {
    intensity_norm[i] = 0;
  }
  for(int i = 0; i < 256; i++) {
    intensity_norm[i] = (double)intensity[i]/(double)(height*width);
  }
}

void ostu::Otsu() {
  c1Prob = 0;
  c2Prob = 0;
  c3Prob = 0;
  for(int i = 0; i < 256; i++) {
    if(i <= lowThreshold) {
      c1Prob += intensity_norm[i];
    } else if(i <= highThreshold){
      c2Prob += intensity_norm[i];
    } else {
      c3Prob += intensity_norm[i];
    }
  }

  c1MeanIntensity = 0;
  c2MeanIntensity = 0;
  c3MeanIntensity = 0;
  globalIntensity = 0;
  for(int i = 0; i < 256; i++) {
    globalIntensity += i*intensity_norm[i];
    if(i <= lowThreshold) {
      c1MeanIntensity += i*intensity_norm[i];
    } else if(i <= highThreshold){
      c2MeanIntensity += i*intensity_norm[i];
    } else {
      c3MeanIntensity += i*intensity_norm[i];
    }
  }
  m = c1MeanIntensity;
  c1MeanIntensity /= c1Prob;
  c2MeanIntensity /= c2Prob;
  c3MeanIntensity /= c3Prob;

  globalVar = 0;
  for(int i = 0; i < 256; i++) {
    globalVar += (i - globalIntensity)*(i - globalIntensity)*intensity_norm[i];
  }

  betweenClassVar = c1Prob*(c1MeanIntensity-globalIntensity)*(c1MeanIntensity-globalIntensity)
                  + c2Prob*(c2MeanIntensity-globalIntensity)*(c2MeanIntensity-globalIntensity)
                  + c3Prob*(c3MeanIntensity-globalIntensity)*(c3MeanIntensity-globalIntensity);

  yita = betweenClassVar/globalVar;
  if(betweenClassVar > bestVar) {
    bestVar = betweenClassVar;
    bestLowThreshold = lowThreshold;
    bestHighThreshold  = highThreshold;
  }
}

void ostu::doSegmentation() {
  output = CImg<unsigned char>(width, height, 1, 1);
  cimg_forXY(filtedImg, x, y) {
    if(filtedImg(x,y) <= bestLowThreshold) {
      output(x,y) = 0;
    } else if(filtedImg(x, y) <= bestHighThreshold){
      output(x,y) = 128;
    } else {
      output(x,y) = 255;
    }
  }
}

void ostu::saveImg() {
  string head = "result_OSTU";
  string tail = path.replace(3,7,head);
  // tail = tail.replace(tail.find("jpg"),3,"bmp");
  cout << "ostu: " << tail << endl;
  output.save(tail.c_str());
}
