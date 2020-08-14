
/*****************************************************************************
* @author  : 徐广辉
* @date    :  2020 / 08 / 13
* @file    : 
* @brief   : 数字识别
*----------------------------------------------------------------------------*
*                           Change History
*----------------------------------------------------------------------------*
* Date        | Version   | Author         | Description
*----------------------------------------------------------------------------*
*             |           |                |
*****************************************************************************/
#include "DigitalRecognition.h"

/*******************************************************************
// @data    : 2020/8/13
// @param   : key为数字矩形对应的x坐标,  value为图像矩阵的map集合
// @param   : 小数点是否存在, 不存在为-1, 存在为对应的x坐标
// @param   : 训练完成的SVM模型
// @return  : NULL
// @brief   : 数字识别及结果拼接
*******************************************************************/
void Recognition::identifyNum(map<int, Mat> number, int pointExit , Ptr<SVM> SVM_params)
{
	map<int, Mat>::iterator iter;
	Mat dealimage;
	int index = 0, findPoint = 0;
	// 遍历map, 获取单个数字的map, 进行识别
	for (iter = number.begin(); iter != number.end(); iter++) 
	{
		if (findPoint == 0 && pointExit >= 0) {
			if (pointExit == (iter->first)) {
				findPoint = 1;
				continue;
			}
			index++;
		}
		dealimage = iter->second;
		//输入图像取特征点
		Mat trainTempImg = Mat::zeros(Size(128, 128), CV_8UC1);
		resize(dealimage, trainTempImg, trainTempImg.size());
		HOGDescriptor* hog = new HOGDescriptor(Size(128, 128), Size(16, 16), Size(8, 8), Size(8, 8), 9);
		vector<float>descriptors;//结果数组         
		hog->compute(trainTempImg, descriptors, Size(1, 1), Size(0, 0));
		//cout << "HOG描述子向量维数    " << descriptors.size() << endl;
		Mat SVMtrainMat = Mat(1, descriptors.size(), CV_32FC1);

		size_t number1 = descriptors.size();

		//将计算好的HOG描述子复制到样本特征矩阵SVMtrainMat  
		for (int i = 0; i < number1; i++)
		{
			//把一幅图像的HOG描述子向量依次存入data_mat矩阵的同一列
			//因为输入图像只有一个，即SVMtrainMat只有一列，则为0
			SVMtrainMat.at<float>(0, i) = descriptors[i]; 
		}

		SVMtrainMat.convertTo(SVMtrainMat, CV_32FC1);//更改图片数据的类型，必要，不然会出错
		int ret = (int)SVM_params->predict(SVMtrainMat);//检测结果  

		digital.push_back(ret);
		//imshow(pattern, dealimage);
	}

	//*************************************拼接结果************************************//
	size_t digitalLength = digital.size(); // 获取数据长度
	size_t temp = digitalLength;
	size_t firstData = 0, pointLocal = 0;
	float pointData = 0.000f, finalData =  0.000f;

	for (auto it : digital)
	{
		firstData += it * pow(10, --digitalLength);
	}
	cout << "数字部分识别的结果：" << firstData << endl;

	if(index != 0)
	{
		pointLocal = temp - index;  // 小数点位置计算
		cout << "小数点位置:" <<  pointLocal << endl;
		pointData = firstData / pow(10, pointLocal);
		finalData = pointData;
	}
	else 
	{
		finalData = (float)firstData;
	}

	cout << "最终结果:" << finalData << endl;
	digital.clear();
}

