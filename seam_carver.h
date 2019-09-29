// my_class.h
#ifndef SEAM_CARVER_H // include guard
#define SEAM_CARVER_H

#include <iostream> 
#include <vector> 
#include <opencv2/opencv.hpp>
using namespace cv;


void verticle_seam(Mat, std::vector<int>&);

void compress_horizontal(Mat, Mat&, int);

#endif /* MY_CLASS_H */