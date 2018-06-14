#include <iostream>
// #define cimg_use_jpeg
#include "CImg.h"
#include "ostu.h"
#include "iter.h"

using namespace std;
using namespace cimg_library;

int main() {
  cout << "aaa" << endl;
  string filePath;
  string head = "../dataset/";
  cout << "aaa" << endl;
  ostu ostu1;
  cout << "aaa" << endl;
  for(int i = 1; i <=100; i++) {
    cout << "aaa" << endl;
    string temp = to_string(i);
    cout << "aaa" << endl;
    temp = head+temp;
    cout << "aaa" << endl;
    filePath = temp + ".JPG";
    cout << "aaa" << endl;
    ostu ostu1(filePath);
    cout << "aaa" << endl;
}

  for(int i = 1; i <= 100; i++) {
    string temp = to_string(i);
    temp = head+temp;
    filePath = temp + ".JPG";
    iter it(filePath);
  }
  return 0;
}
