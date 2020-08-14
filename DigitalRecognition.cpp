
/*****************************************************************************
* @author  : ����
* @date    :  2020 / 08 / 13
* @file    : 
* @brief   : ����ʶ��
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
// @param   : keyΪ���־��ζ�Ӧ��x����,  valueΪͼ������map����
// @param   : С�����Ƿ����, ������Ϊ-1, ����Ϊ��Ӧ��x����
// @param   : ѵ����ɵ�SVMģ��
// @return  : NULL
// @brief   : ����ʶ�𼰽��ƴ��
*******************************************************************/
void Recognition::identifyNum(map<int, Mat> number, int pointExit , Ptr<SVM> SVM_params)
{
	map<int, Mat>::iterator iter;
	Mat dealimage;
	int index = 0, findPoint = 0;
	// ����map, ��ȡ�������ֵ�map, ����ʶ��
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
		//����ͼ��ȡ������
		Mat trainTempImg = Mat::zeros(Size(128, 128), CV_8UC1);
		resize(dealimage, trainTempImg, trainTempImg.size());
		HOGDescriptor* hog = new HOGDescriptor(Size(128, 128), Size(16, 16), Size(8, 8), Size(8, 8), 9);
		vector<float>descriptors;//�������         
		hog->compute(trainTempImg, descriptors, Size(1, 1), Size(0, 0));
		//cout << "HOG����������ά��    " << descriptors.size() << endl;
		Mat SVMtrainMat = Mat(1, descriptors.size(), CV_32FC1);

		size_t number1 = descriptors.size();

		//������õ�HOG�����Ӹ��Ƶ�������������SVMtrainMat  
		for (int i = 0; i < number1; i++)
		{
			//��һ��ͼ���HOG�������������δ���data_mat�����ͬһ��
			//��Ϊ����ͼ��ֻ��һ������SVMtrainMatֻ��һ�У���Ϊ0
			SVMtrainMat.at<float>(0, i) = descriptors[i]; 
		}

		SVMtrainMat.convertTo(SVMtrainMat, CV_32FC1);//����ͼƬ���ݵ����ͣ���Ҫ����Ȼ�����
		int ret = (int)SVM_params->predict(SVMtrainMat);//�����  

		digital.push_back(ret);
		//imshow(pattern, dealimage);
	}

	//*************************************ƴ�ӽ��************************************//
	size_t digitalLength = digital.size(); // ��ȡ���ݳ���
	size_t temp = digitalLength;
	size_t firstData = 0, pointLocal = 0;
	float pointData = 0.000f, finalData =  0.000f;

	for (auto it : digital)
	{
		firstData += it * pow(10, --digitalLength);
	}
	cout << "���ֲ���ʶ��Ľ����" << firstData << endl;

	if(index != 0)
	{
		pointLocal = temp - index;  // С����λ�ü���
		cout << "С����λ��:" <<  pointLocal << endl;
		pointData = firstData / pow(10, pointLocal);
		finalData = pointData;
	}
	else 
	{
		finalData = (float)firstData;
	}

	cout << "���ս��:" << finalData << endl;
	digital.clear();
}

