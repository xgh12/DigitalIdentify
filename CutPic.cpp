
/*****************************************************************************
* @author  : 徐广辉
* @date    : 2020  /  08   /  13
* @file    :
* @brief   : 图片的行分割和数字分割
*----------------------------------------------------------------------------*
*                           Change History
*----------------------------------------------------------------------------*
* Date        | Version   | Author         | Description
*----------------------------------------------------------------------------*
*             |           |                |
*****************************************************************************/
#include "CutPic.h"


/******************************************************************
// @data    : 2020/8/5
// @param   : 二值化图像BianryImage
// @param   : 训练完成的SVM模型
// @return  : 行数Row
// @brief   : 行分割
// @generate：行图像CUT_WIN_ROI1.2.3...
*******************************************************************/
int Cutpic::CutRow(Mat& BinaryImage, Ptr<SVM> SVM_params)
{
    CvSize pSize;
    //CvRect RectLine;
    pSize.width = BinaryImage.cols;  // 获取图像宽度
    pSize.height = BinaryImage.rows; // 获取图像高度

    int* startIndex = new int[10];
    memset(startIndex, 0, 10 * sizeof(int));

    Mat ROI; // 保存剪切后的图像
    char WinName[15]; // 窗口名称

    int* rowwidth = new int[pSize.height];

    cout << endl;
    cout << "原图二值图尺寸：" << endl;
    cout << "二值图图像宽为：" << pSize.width << endl;
    cout << "二值图图像高为：" << pSize.height << endl << endl;

    Mat structElement2 = getStructuringElement(MORPH_RECT, Size(6, 6));
    dilate(BinaryImage, BinaryImage, structElement2);
    imshow("膨胀", BinaryImage);
    Mat structElement1 = getStructuringElement(MORPH_RECT, Size(1, 1));
    erode(BinaryImage, BinaryImage, structElement1);
    imshow("腐蚀", BinaryImage);

    //******************************记录行像素中的白色像素点*****************************//
    // 建立数组rowwidth用来存储每行白色像素数
    memset(rowwidth, 0, pSize.height * sizeof(int));

    double val;

    for (int i = 0; i < pSize.height; i++)
    {
        for (int j = 0; j < pSize.width; j++)
        {
            val = BinaryImage.at<uchar>(i, j);   // 获取图像像素点的像素值

            if (val == 255.0) {
                // 统计白色的像素
                rowwidth[i] = rowwidth[i] + 1;
            }
        }
    }

    //*********************************查找分行位置*********************************//
    //查找分行位置
    int* DivLoc = new int[10];
    memset(DivLoc, 0, 10 * sizeof(int));

    int RowNum = 0;
    int in = 0;
    for (int i = 1; i < pSize.height; i++)
    {
        if (rowwidth[i - 1] <= 10 && rowwidth[i] > 10) // 记录进入字符区时的坐标
        {
            startIndex[RowNum] = i;
        }
        //前一行的 白色像素点 >= 10  且 第二行的 白色像素点 < 10则可判断为换行 
        if (rowwidth[i - 1] >= 10 && rowwidth[i] < 10)
        {
            DivLoc[RowNum] = i;  // 换行的位置
            RowNum++; // 换行的次数
        }
    }

    //********************************分割行*********************************//
    //按行分割图像
    RowNum = 0;
    string  CutSavePath = "C:\\Users\\XGH\\Desktop\\数字\\Row%d.png";
    size_t PathLenght = CutSavePath.length() + 3;

    while (DivLoc[RowNum]) {

        // LinePath是行图像的存储路径，每次循环发生改变
        char* LinePath = (char*)malloc(PathLenght * sizeof(char));
        if (LinePath == NULL)
        {
            cout << "行图片路径内存分配失败" << endl;
            exit(1);
        }

        cout << "开始行分割:" ;

        // 确认感兴趣区域，进行分割
        //确定矩形（留5像素边）
        sprintf_s((char*)WinName, 15, "CUT_WIN_NAME-%d", RowNum + 1);
        if (RowNum == 0) {
            Rect rect(0, startIndex[RowNum], pSize.width, DivLoc[RowNum] - startIndex[RowNum]);
            ROI = BinaryImage(rect);
        }
        else {
            Rect rect(0, startIndex[RowNum], pSize.width, DivLoc[RowNum] - startIndex[RowNum]);
            ROI = BinaryImage(rect);
        }
        cout << "行分割完成" << endl << endl;

        //****************************************分割数字*************************************//
        CutNum(ROI, RowNum, SVM_params);  // 剪切出图片

        //保存分列图像
        sprintf_s(LinePath, PathLenght, "C:\\Users\\XGH\\Desktop\\数字\\Row%d.png", RowNum + 1);
        imwrite(LinePath, ROI);
        imshow(WinName, ROI);

        free(LinePath);
        LinePath = NULL;

        RowNum++;
    }

    return RowNum;
}

/*******************************************************************
// @data    : 2020/8/13
// @param   : 行分割后的二值图图像
// @param   : 行分割后的行数
// @param   : 训练完成的SVM模型
// @return  : 分割后的数字数量
// @brief   : 分割数字
// @generate: 单独的数字图像1-1、1-2...
*******************************************************************/
size_t Cutpic::CutNum(Mat& CutImage, int rowNum, Ptr<SVM> SVM_params)
{
    Mat out;
    /*Mat structElement2 = getStructuringElement(MORPH_RECT, Size(2, 2));
    dilate(CutImage, out, structElement2);
    imshow("【二次处理】", out);*/

    medianBlur(CutImage, out, 3);//中值滤波
    Mat pretreatment = out.clone();
    
    //imshow("预处理结果", pretreatment);

    //*************************************轮廓查找*************************************////
    vector<vector<Point>>contours;
    findContours(out, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); //查找所有外轮廓，输入图像必须为二值图
    Mat outline = CutImage.clone();
    drawContours(outline, contours, -1, Scalar(0, 0, 255), 2);// 画出所有轮廓 Scalar(B, G, R)

    //*********************************剔除超过范围的轮廓*************************************///
    Mat result = CutImage.clone();
    vector<std::vector<cv::Point>>::const_iterator itc = contours.begin();

    int cmin = 10, cmax = 500;  //剔除超过范围的轮廓
    while (itc != contours.end()) {
        
        if (itc->size() < cmin || itc->size() > cmax)
            itc = contours.erase(itc);
        else
            ++itc;
    }

    //*********************************画出各个轮廓的最小包围矩形******************************///
    map<int, int> Area;
    parameter_type parameter;
    size_t digitalNum = contours.size();

    cout << "分割后的数字面积：" << endl;
    for (int i = 0; i < digitalNum; i++)
    {
        Rect r = boundingRect(Mat(contours[i]));
        rectangle(outline, r, Scalar(255, 0, 0), 1);
        // 记录每个矩阵的坐标和长宽
        parameter.x.push_back(r.x);
        parameter.y.push_back(r.y);
        parameter.width.push_back(r.width);
        parameter.height.push_back(r.height);
        Area.insert(pair<int, int>(r.area(), r.x));  // 记录矩阵面积和对应的x坐标
        cout << r.area() << endl;
    }
    cout << endl;

    // 比较最小面积和最大面积，当5倍最小面积小于最大面积，则存在小数点
    int pointIndex = -1;
    int minArea = Area.begin()->first;
    auto map_temp = Area.end();
    map_temp--;
    int maxArea = map_temp->first;
    cout << "小数点判断";
    if (minArea * 5 < maxArea) {
        cout << "数字图像中存在小数点" << endl;
        pointIndex = Area.begin()->second;
    }
    else {
        pointIndex = -1;
        cout << "数字图像是整数" << endl << endl;
    }

    char rectangular[50];
    sprintf_s((char *)rectangular, 50, "【矩形框定位置】%d", rowNum);
    imshow(rectangular, outline);

    //************************************* 切割出定位的矩形************************************///
    char rectnum[255];
    //char file[10];
    Mat pic;

    vector<map<int, Mat>> numberList;
    map<int, Mat> number;

    for (int i = 0; i < digitalNum; i++)
    {
        //sprintf_s(file, 10, "%d-%d", rowNum, i);
        sprintf_s(rectnum, 255, "C:\\Users\\XGH\\Desktop\\数字\\%d-%d.png", rowNum, i);
        Rect r(parameter.x[i], parameter.y[i], parameter.width[i], parameter.height[i]);
        pic = pretreatment(r);//选定roi
        resize(pic, pic, Size(32, 32));//把切割的图片缩放成32*32
        number.insert(pair<int, Mat>(parameter.x[i], pic));

        imwrite(rectnum, pic);
        //imshow(file, pic);
    }

    // 数字识别
    Recognition recognition;
    recognition.identifyNum(number, pointIndex, SVM_params);

    number.erase(number.begin(), number.end()); // 清空map中的数据
    Area.erase(Area.begin(), Area.end());
    numberList.clear();
    parameter.x.clear();
    parameter.y.clear();
    parameter.width.clear();
    parameter.height.clear();

    return digitalNum;
}

// 投影法分割数字
/*cout << "分割数字" << endl;
 CvSize pSize;
 CvRect RectLine;
 pSize.width = CutImage.cols;  // 获取图像宽度
 pSize.height = CutImage.rows; // 获取图像高度
 char WinName[18]; // 窗口名称

 string  CutSavePath = "C:\\Users\\XGH\\Desktop\\数字\\Num-%d-%d.png";
 int PathLenght = CutSavePath.length() + 3;
 int picID = 0;

 int perPixelValue;//每个像素的值
 int* projectValArry = new int[pSize.width];//创建一个用于储存每列白色像素个数的数组
 memset(projectValArry, 0, pSize.width * sizeof(int));//必须初始化数组

 //遍历每一列的图像灰度值，查找每一行255的值
 for (int col = 0; col < pSize.width; ++col)
 {
     for (int row = 0; row < pSize.height; ++row)
     {
         perPixelValue = CutImage.at<uchar>(row, col);
         if (perPixelValue == 255)//如果是黑底白字
         {
             projectValArry[col]++;
         }
     }
 }

 新建一个Mat用于储存投影直方图并将背景置为白色
 Mat verticalProjectionMat(pSize.height, pSize.width, CV_8UC1);
 for (int i = 0; i < pSize.height; i++)
 {
     for (int j = 0; j < pSize.width; j++)
     {
         perPixelValue = 255;  //背景设置为白色。
         verticalProjectionMat.at<uchar>(i, j) = perPixelValue;
     }
 }

 将直方图的曲线设为黑色
 for (int i = 0; i < pSize.width; i++)
 {
     for (int j = 0; j < projectValArry[i]; j++)
     {
         perPixelValue = 0;  //直方图设置为黑色
         verticalProjectionMat.at<uchar>(pSize.height - 1 - j, i) = perPixelValue;
     }
 }
 imshow("【投影】", verticalProjectionMat);
 //delete[] projectValArry;//不要忘了删除数组空间

 vector<Mat> roiList;//用于储存分割出来的每个字符
 int startIndex = 0;//记录进入字符区的索引
 int endIndex = 0;//记录进入空白区域的索引
 bool inBlock = false;//是否遍历到了字符区内
 Mat ROI;

 for (int i = 0; i < CutImage.cols; ++i)
 {
     if (!inBlock && projectValArry[i] != 0)//进入字符区了
     {
         inBlock = true;
         startIndex = i;
         cout << "startIndex is " << startIndex << endl;
     }
     else if (projectValArry[i] == 0 && inBlock)//进入空白区了
     {
         // LinePath是行图像的存储路径，每次循环发生改变
         char* LinePath = (char*)malloc(PathLenght * sizeof(char));

         endIndex = i;
         cout << "endIndex is " << endIndex << endl;
         inBlock = false;
         Mat roiImg = CutImage(Range(0, CutImage.rows), Range(startIndex, endIndex + 1));
         roiList.push_back(roiImg);

         sprintf_s((char*)WinName, 18, "CUT_COL_NAME-%d", i + 1);
         Rect rect(startIndex, 0, (endIndex - startIndex), pSize.height - 5);
         ROI = CutImage(rect);
         resize(ROI, ROI, Size(32, 32));

         // 数字识别
         Recognition recognition;
         recognition.identifyNum(ROI, ++picID, SVM_params);

         //保存分行图像
         sprintf_s(LinePath, PathLenght, "C:\\Users\\XGH\\Desktop\\数字\\Num-%d-%d.png", rowNum, picID);
         imwrite(LinePath, ROI);
     }
 }

 return 1;*/