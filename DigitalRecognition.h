#pragma once
#ifndef DIGITALRECOGNITION_H
#define DIGITALRECOGNITION_H

#include <stdio.h>  
#include <time.h>  
#include <math.h>
#include <opencv2/opencv.hpp>  
#include <iostream> 
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>  
#include <io.h> //�����ļ���غ���
#include <cstring>
#include "CutPic.h"

using namespace std;
using namespace cv;
using namespace ml;

class Recognition
{

public:
	void identifyNum(map<int, Mat> number, int pointExit, Ptr<SVM> SVM_params);

private:
	//Ptr<SVM>SVM_params;
	ostringstream oss;//����ַ���������
	vector<int> digital; // ����ʶ��������
};

#endif // !DIGITALRECOGNITION_H
