#include <iostream>
// #define cimg_use_jpeg
#include "CImg.h"
#include "ostu.h"
#include "iter.h"

using namespace std;
using namespace cimg_library;

int main() {
  string filePath;
  string head = "../dataset/";
  ostu ostu1;
  for(int i = 1; i <=100; i++) {
    string temp = to_string(i);
    temp = head+temp;
    filePath = temp + ".JPG";
    ostu ostu1(filePath);
}

  for(int i = 1; i <= 100; i++) {
    string temp = to_string(i);
    temp = head+temp;
    filePath = temp + ".JPG";
    iter it(filePath);
  }
  return 0;
}
