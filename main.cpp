/*****************************************************************************
* @author  : 徐广辉                                                            
* @date    : 2020 / 08 / 05                                                  
* @file    :                                                                 
* @brief   : 仪表文字识别主函数                                             
*----------------------------------------------------------------------------*
*                           Change History                                   
*----------------------------------------------------------------------------*
* Date        | Version   | Author         | Description                     
*----------------------------------------------------------------------------*
*             |           |                |                                 
*****************************************************************************/
#include "main.h"

int main()
{
    //*************************************加载SVM模型*******************************//
    Ptr<SVM>SVM_params;
    cout << "加载XML文件中..." << endl;
    //加载XML文件
    SVM_params = SVM::load("C:\\Users\\XGH\\Desktop\\模板匹配样本\\基于机器学习\\字符识别svm.xml");
    cout << "加载完成" << endl;

    //*************************************处理原图*********************************//
    //源图像：
    Mat SrcImage1, SrcImage2;
    //二值图：
    Mat BinaryImage;
    //灰度图：
    Mat GrayImage1, GrayImage2, GrayImage3;
    //源图片加载地址
    const char* SrcImageName1 = "C:\\Users\\XGH\\Desktop\\数字\\600.png";
    //图片储存地址
    const char* SaveBinaryPath = "C:\\Users\\XGH\\Desktop\\数字\\二值图.png";
    //图像窗口名称
    const char* BinaryWindowName = "二值图";
    //读取原图
    SrcImage1 = imread(SrcImageName1);
    imshow("【原图】", SrcImage1);

    // 根据图像大小重新调整图片尺寸, 更改图片大小为了分割图片时更准确 
    if (SrcImage1.cols <= 250 && SrcImage1.rows <= 130) {

        cout << "调整图像大小" << endl;

        resize(SrcImage1, SrcImage1, Size(2 * SrcImage1.cols, 2 * SrcImage1.rows));
        ////变成灰度图
        //cvtColor(SrcImage1, GrayImage1, CV_BGR2GRAY);
        //meanStdDev(GrayImage1, GrayImage2, GrayImage3);


        //double mean, standards;
        //mean = GrayImage2.at<double>(0, 0);
        //standards = GrayImage3.at<double>(0, 0);
        //cout << "灰度均值是：" << mean << endl;
        //cout << "的标准差是：" << standards << endl;

        ////创建二值图
        //threshold(GrayImage1, BinaryImage, mean, 255, THRESH_BINARY_INV);
    }
    //else {
        // 变成灰度图
        cvtColor(SrcImage1, GrayImage1, CV_BGR2GRAY);

        meanStdDev(GrayImage1, GrayImage2, GrayImage3);  // 计算像素均值作为阈值

        double mean, standards;
        mean = GrayImage2.at<double>(0, 0);
        standards = GrayImage3.at<double>(0, 0);
        cout << "灰度均值是：" << mean << endl;
        cout << "的标准差是：" << standards << endl;

        //创建二值图
        threshold(GrayImage1, BinaryImage, mean, 255, THRESH_BINARY);
    //}   
    //显示
    imshow("二值图", BinaryImage);
    imwrite(SaveBinaryPath, BinaryImage);

    //***************************************分割数字**************************************//
    Cutpic cutpic;
    int num = cutpic.CutRow(BinaryImage, SVM_params);

    //保存
    imwrite(SaveBinaryPath, BinaryImage);

    cout << endl << "数字识别结束" << endl;
    //等待按键事件
    waitKey();
    return 0;
}
