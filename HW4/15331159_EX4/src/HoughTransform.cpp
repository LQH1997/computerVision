#include <map>
#include <set>
#include "HoughTransform.h"
#include <string>
using namespace std;

#define PI 3.14159

int con=4;

HoughTransform::HoughTransform(char* filename, int method) {
    this->filename = filename;
    this->method = method;
    origin.load_bmp(filename);
    int h = origin.height();
    int w = origin.width();
    if(method == 1) {
      canny can(origin, 3, 40, 120);
      edge = can.returenResult();
      findLine();
    } else{
      for(int temp = 1; temp < 10; temp++) {
        if(temp > 1 && temp % 2 != 0) continue;
        if(h/temp <= 401 && w/temp <= 401) {
          con = temp;
          break;
        }
      }
      vector<vector<double> > RGBFilter = createFilter(3, 3, 1);
      CImg<unsigned char> RGBTemp = useFilter(origin, RGBFilter);
      // RGBTemp.display("RGB");
      // cout << "start" << endl;
      // canny temp(origin, 3, 40, 120);
      // cout << "temp" << endl;
      // CImg<unsigned char> a = temp.returnGuassianResult();
      CImg<unsigned char> small(RGBTemp.width()/con+1, RGBTemp.height()/con+1,1,3);
      cimg_forXY(RGBTemp, x, y) {
        if(x % con == 0 && y % con == 0) {
          small(x/con, y/con, 0) = RGBTemp(x, y, 0);
          small(x/con, y/con, 1) = RGBTemp(x, y, 1);
          small(x/con, y/con, 2) = RGBTemp(x, y, 2);
        }
      }
      // small.display();
      // small.display();
      // cout << "aaa" << endl;
       canny can(small, 3, 40, 120);
      edge = can.returenResult();
      findCircle(small);
    }
}

CImg<unsigned char> HoughTransform::findLine() {
    CImg<unsigned char> img = edge;
    lineOutput = origin;
    // img.display("edge");

    double thete;
    int r;
    int count = 0;

    int width = img.width();
    int height = img.height();
    int maxLength = (int)sqrt(width*width + height*height);
    houghGraph = CImg<unsigned char>(180, maxLength,1,1,0);

  //the voting progress
    cimg_forXY(img, x, y) {
            if (x < 10 || x > width-10 || y < 10 || y > height-10) continue;
            if (img(x, y) == 255) {
                for (int i = 0; i < 180; i++) {
                    thete = PI*i/180;
                    r = (int)(x*cos(thete)+y*sin(thete));
                    if (r < -maxLength || r > maxLength)
                        continue;
                    if(r >=0 && r < maxLength)
                      houghGraph(i,r,0) = houghGraph(i,r)+1;
                    hough h(i, r);
                    houghSpace[h]++;
                }
            }
        }
        // houghGraph.display();

//find the points with maximun value
    for (auto & x : houghSpace) {
        sortHough[x.second] = x.first;
    }

    count = 0;

    for (auto it = sortHough.rbegin(); it != sortHough.rend(); ++it) {
        bool duplicate = false;
        for (auto x : line) {
          //check whether the line is duplicated
          //if it's duplicated,we ignore this line
            if (abs(abs(it->second.r) - abs(x.r)) < 10 &&
            (abs(it->second.i - x.i) < 3 || abs(it->second.i + x.i - 180) < 3)) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) continue;
//if it's not duplicated,we use the point in the polar system to draw the line
//on the origin picture
        line.insert(it->second);
        r = it->second.r;
        thete = PI*(it->second.i)/180;
//draw red lines for the edges
        cimg_forXY(lineOutput, x, y) {
                int cur_r = int(x*cos(thete) + y*sin(thete));
                if (abs(cur_r - r) < 2) {
                  if(lineOutput(x, y, 0) == 255 &&lineOutput(x, y, 1) == 0 && lineOutput(x, y, 2) == 0) {
                    bool tempDup = false;
                    for(int qq = 0; qq < intersetcions.size(); qq++) {
                      if((abs(intersetcions[qq].x-x) < 10) && (abs(intersetcions[qq].y - y) < 10)) {
                        tempDup = true;
                        break;
                      }
                  }
                  if(!tempDup) intersetcions.push_back(point(x, y));
                  }
                    lineOutput(x, y, 0) = 255;
                    lineOutput(x, y, 1) = 0;
                    lineOutput(x, y, 2) = 0;;
                }
            }
        count++;
        if (count == 4) break;
    }

    //draw blue circles at the intersetcion points
    unsigned char blue[] ={0, 0, 255};
    for(int q = 0; q < intersetcions.size(); q++) {
      lineOutput.draw_circle(intersetcions[q].x, intersetcions[q].y, 10, blue);
    }
    // lineOutput.display("line");

    string temp = (char*)"answer_";
    // temp = temp+filename;
    string temp1 = filename;
    temp1.insert(9, temp);
    lineOutput.save(temp1.c_str());


    return lineOutput;

}

CImg<unsigned char> HoughTransform::findCircle(CImg<unsigned char> smallOrigin) {
  CImg<unsigned char> img = edge;
  circleOutput = smallOrigin;
  // img.display("edge");
  int width = origin.width();
  int height = origin.height();
  double theta;

  // cout << "in loop" << endl;

  // CImg<unsigned char> small(width/con+1, height/con+1, 1, 1);
  // CImg<unsigned char> smallOrigin(width/con+1, height/con+1, 1, 3);
  // cimg_forXY(img, x, y) {
  //   if(x % con == 0 && y % con == 0) {
  //     // small(x/con, y/con) = img(x, y);
  //     smallOrigin(x/con, y/con, 0) = origin(x, y, 0);
  //     smallOrigin(x/con, y/con, 1) = origin(x, y, 1);
  //     smallOrigin(x/con, y/con, 2) = origin(x, y, 2);
  //   }
  // }
  // small.display();
  // smallOrigin.display();
  int maxRadius = width/con/2 < height/con/2 ? width/con/2-10: height/con/2-10;
  cimg_forXY(img,x, y) {
    // cout << x << " " << y << endl;
    if (x < 15 || x > width-15 || y < 15 || y > height-15) continue;
    if(img(x,y) == 255) {
      for(int radius = 25; radius < maxRadius; radius++) { //try each radius
        for(int angle = 0; angle <= 360; angle++) {
          double theta = angle * PI /180;
          int temp1 = x - radius * cos(theta);
          int temp2 = y - radius * sin(theta);
          circle c(temp1, temp2, radius);
          circleHoughSpace[c]++;
        }
      }
    }
  }

// cout << "construction finished" << endl;

// map<circle, int> tempSpace = circleHoughSpace;
// for(auto x = circleHoughSpace.rbegin(); x != circleHoughSpace.rend(); ++x) {
//   for(auto y = tempSpace.rbegin(); y != tempSpace.rend(); ++y){
// //     if(abs(x->first.a - y->first.a < 2) && abs(x->first.b - y->first.b) < 2) {
// //       y->second += x->second;
//       circle c1(y->first.a-1, y->first.b, y->first.r);
//       circle c2(y->first.a, y->first.b-1, y->first.r);
//       circle c3(y->first.a+1, y->first.b, y->first.r);
//       circle c4(y->first.a, y->first.b+1, y->first.r);
//       circle c5(y->first.a+1, y->first.b+1, y->first.r);
//       circle c6(y->first.a+1, y->first.b-1, y->first.r);
//       circle c7(y->first.a-1, y->first.b-1, y->first.r);
//       circle c8(y->first.a-1, y->first.b+1, y->first.r);
//       // circle c4(y->first.a, y->first.b+1, y->first.r);
//       y->second += circleHoughSpace[c1]+circleHoughSpace[c2]+circleHoughSpace[c3]+circleHoughSpace[c4]
//       +circleHoughSpace[c5]+circleHoughSpace[c6]+circleHoughSpace[c7]+circleHoughSpace[c8];
//     }
//   }
//   cout << "111" << endl;
// }


  for(auto & x : circleHoughSpace) {
    sortCircleHoughSpace[x.second] = x.first;
  }

  // map<int, circle> tempSortCircleHoughSpace;
  // for(auto it1 = sortCircleHoughSpace.rbegin(); it1 != sortCircleHoughSpace.rend(); ++it1) {
  //   for(auto it2 = tempSortCircleHoughSpace.rbegin(); it2 != tempSortCircleHoughSpace.rend(); ++it2) {
  //     if(abs(it1 -> second.a - it2 -> second.a) <= 2 && abs(it1 -> second.b - it2 -> second.b) <= 2) {
  //       it2->first += it1->first;
  //     }
  //     else {
  //       tempSortCircleHoughSpace.insert(it1->first, it1->second);
  //     }
  //   }
  // }
  // sortCircleHoughSpace = tempSortCircleHoughSpace;

// map<circle, int> tempSpace;
// for(auto it1=)
// cout << "sort finished" << endl;
  int threshold = sortCircleHoughSpace.rbegin()->first/5*3;
  // cout << "max: " <<  sortCircleHoughSpace.rbegin()->first << endl;
  // cout << "threshold: " <<threshold<<endl;
  for(auto it = sortCircleHoughSpace.rbegin(); it != sortCircleHoughSpace.rend(); ++it) {
    if(it->first >= threshold) {
      bool duplicate = false;
      for(auto x : originCircle) {
        if(abs(x.a-it->second.a) <= 35 &&
        abs(x.b - it->second.b) <= 35 ) {
          if(abs(x.r - it->second.r) > 35) {
            duplicate = false;
            break;
          }
          duplicate = true;
          break;
        }
      }
      if(duplicate) {
        continue;
      } else {
        originCircle.insert(it->second);
      }
    }
  }

// cout << "finding finished" << endl;
// cout << originCircle.size() << endl;
  unsigned char red[] = {255, 0, 0};
  unsigned char yellow[] = {255,255,0};
  for(auto x : originCircle) {
    smallOrigin.draw_circle(x.a, x.b, x.r, red, 1, 5);
  }

  // smallOrigin.display();
  // char* fore = "";
  // char* temp = strcat(fore, filename);
  string a = "number:";
  char num;
  num='0'+originCircle.size();
  // cout << num << endl;
  a = a+num;
  smallOrigin.draw_text(10,10,a.c_str(),red,yellow,1,25);
   string temp = (char*)"answer_";
  // temp = temp+filename;
  string temp1 = filename;
  temp1.insert(9, temp);
  smallOrigin.save(temp1.c_str());
  return circleOutput;
}

vector<vector<double> > HoughTransform::createFilter(int row, int column, double sigmaIn)
{
    vector<vector<double> > filter;

    for (int i = 0; i < row; i++)
    {
        vector<double> col;
        for (int j = 0; j < column; j++)
        {
            col.push_back(-1);
        }
        filter.push_back(col);
    }

    float coordSum = 0;
    float constant = 2.0 * sigmaIn * sigmaIn;

    // Sum is for normalization
    float sum = 0.0;

    for (int x = - row/2; x <= row/2; x++)
    {
        for (int y = -column/2; y <= column/2; y++)
        {
            coordSum = (x*x + y*y);
            filter[x + row/2][y + column/2] = (exp(-(coordSum) / constant)) / (M_PI * constant);
            sum += filter[x + row/2][y + column/2];
        }
    }

    // Normalize the Filter
    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            filter[i][j] /= sum;

    return filter;

}

CImg<unsigned char>HoughTransform::useFilter(CImg<unsigned char> img_in, vector<vector<double> > filterIn)
{
    int size = (int)filterIn.size()/2;

    int width = img_in.width();
    int height = img_in.height();
    int depth = img_in.depth();

    //New grayscale images.
    CImg<unsigned char> filteredImg = CImg<unsigned char>(width, height, depth, 3);

    for (int i = size; i < width - size; i++)
    {
        for (int j = size; j < height - size; j++)
        {
            double sum[3] = {0};

            for (int x = 0; x < filterIn.size(); x++)
                for (int y = 0; y < filterIn.size(); y++)
                {
                    sum[0] += filterIn[x][y] * (double)(img_in(i + x - size, j + y - size, 0));
                    sum[1] += filterIn[x][y] * (double)(img_in(i + x - size, j + y - size, 1));
                    sum[2] += filterIn[x][y] * (double)(img_in(i + x - size, j + y - size, 2));
                }

            filteredImg(i-size, j-size,0) = sum[0];
            filteredImg(i-size, j-size,1) = sum[1];
            filteredImg(i-size, j-size,2) = sum[2];
        }

    }
    return filteredImg;
}

vector<point>  HoughTransform::returnIntersections() {
  return intersetcions;
}

CImg<unsigned char> HoughTransform::returnOriginImage() {
  return origin;
}
