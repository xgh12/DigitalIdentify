#pragma once
#ifndef CUTPIC_H
#define CUTPIC_H

#include <stdio.h>  
#include <time.h>  
#include <opencv2/opencv.hpp>  
#include <iostream> 
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>  
#include <io.h> //查找文件相关函数
#include <cstring>
#include <map>
#include "DigitalRecognition.h"

using namespace std;
using namespace cv;
using namespace ml;

class Cutpic {

public:

	int CutRow(Mat& BinaryImage, Ptr<SVM> SVM_params);
	size_t CutNum(Mat& CutImage, int rowNum, Ptr<SVM> SVM_params);

private:
	struct parameter_type {
		vector<int> x;
		vector<int> y;
		vector<int> width;
		vector<int> height;
	};

};

#endif // !CUTPIC_H
