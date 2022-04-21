#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv) {
  Mat src = imread(argv[argc - 1]);
  cout << "W: " << src.cols << ", H: " << src.rows << endl;
  Mat dst;
  cvtColor(src, dst, COLOR_BGR2GRAY);
  imwrite("gray.png", dst);
  return 0;
}