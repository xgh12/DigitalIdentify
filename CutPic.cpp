
/*****************************************************************************
* @author  : ����
* @date    : 2020  /  08   /  13
* @file    :
* @brief   : ͼƬ���зָ�����ַָ�
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
// @param   : ��ֵ��ͼ��BianryImage
// @param   : ѵ����ɵ�SVMģ��
// @return  : ����Row
// @brief   : �зָ�
// @generate����ͼ��CUT_WIN_ROI1.2.3...
*******************************************************************/
int Cutpic::CutRow(Mat& BinaryImage, Ptr<SVM> SVM_params)
{
    CvSize pSize;
    //CvRect RectLine;
    pSize.width = BinaryImage.cols;  // ��ȡͼ����
    pSize.height = BinaryImage.rows; // ��ȡͼ��߶�

    int* startIndex = new int[10];
    memset(startIndex, 0, 10 * sizeof(int));

    Mat ROI; // ������к��ͼ��
    char WinName[15]; // ��������

    int* rowwidth = new int[pSize.height];

    cout << endl;
    cout << "ԭͼ��ֵͼ�ߴ磺" << endl;
    cout << "��ֵͼͼ���Ϊ��" << pSize.width << endl;
    cout << "��ֵͼͼ���Ϊ��" << pSize.height << endl << endl;

    Mat structElement2 = getStructuringElement(MORPH_RECT, Size(6, 6));
    dilate(BinaryImage, BinaryImage, structElement2);
    imshow("����", BinaryImage);
    Mat structElement1 = getStructuringElement(MORPH_RECT, Size(1, 1));
    erode(BinaryImage, BinaryImage, structElement1);
    imshow("��ʴ", BinaryImage);

    //******************************��¼�������еİ�ɫ���ص�*****************************//
    // ��������rowwidth�����洢ÿ�а�ɫ������
    memset(rowwidth, 0, pSize.height * sizeof(int));

    double val;

    for (int i = 0; i < pSize.height; i++)
    {
        for (int j = 0; j < pSize.width; j++)
        {
            val = BinaryImage.at<uchar>(i, j);   // ��ȡͼ�����ص������ֵ

            if (val == 255.0) {
                // ͳ�ư�ɫ������
                rowwidth[i] = rowwidth[i] + 1;
            }
        }
    }

    //*********************************���ҷ���λ��*********************************//
    //���ҷ���λ��
    int* DivLoc = new int[10];
    memset(DivLoc, 0, 10 * sizeof(int));

    int RowNum = 0;
    int in = 0;
    for (int i = 1; i < pSize.height; i++)
    {
        if (rowwidth[i - 1] <= 10 && rowwidth[i] > 10) // ��¼�����ַ���ʱ������
        {
            startIndex[RowNum] = i;
        }
        //ǰһ�е� ��ɫ���ص� >= 10  �� �ڶ��е� ��ɫ���ص� < 10����ж�Ϊ���� 
        if (rowwidth[i - 1] >= 10 && rowwidth[i] < 10)
        {
            DivLoc[RowNum] = i;  // ���е�λ��
            RowNum++; // ���еĴ���
        }
    }

    //********************************�ָ���*********************************//
    //���зָ�ͼ��
    RowNum = 0;
    string  CutSavePath = "C:\\Users\\XGH\\Desktop\\����\\Row%d.png";
    size_t PathLenght = CutSavePath.length() + 3;

    while (DivLoc[RowNum]) {

        // LinePath����ͼ��Ĵ洢·����ÿ��ѭ�������ı�
        char* LinePath = (char*)malloc(PathLenght * sizeof(char));
        if (LinePath == NULL)
        {
            cout << "��ͼƬ·���ڴ����ʧ��" << endl;
            exit(1);
        }

        cout << "��ʼ�зָ�:" ;

        // ȷ�ϸ���Ȥ���򣬽��зָ�
        //ȷ�����Σ���5���رߣ�
        sprintf_s((char*)WinName, 15, "CUT_WIN_NAME-%d", RowNum + 1);
        if (RowNum == 0) {
            Rect rect(0, startIndex[RowNum], pSize.width, DivLoc[RowNum] - startIndex[RowNum]);
            ROI = BinaryImage(rect);
        }
        else {
            Rect rect(0, startIndex[RowNum], pSize.width, DivLoc[RowNum] - startIndex[RowNum]);
            ROI = BinaryImage(rect);
        }
        cout << "�зָ����" << endl << endl;

        //****************************************�ָ�����*************************************//
        CutNum(ROI, RowNum, SVM_params);  // ���г�ͼƬ

        //�������ͼ��
        sprintf_s(LinePath, PathLenght, "C:\\Users\\XGH\\Desktop\\����\\Row%d.png", RowNum + 1);
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
// @param   : �зָ��Ķ�ֵͼͼ��
// @param   : �зָ�������
// @param   : ѵ����ɵ�SVMģ��
// @return  : �ָ�����������
// @brief   : �ָ�����
// @generate: ����������ͼ��1-1��1-2...
*******************************************************************/
size_t Cutpic::CutNum(Mat& CutImage, int rowNum, Ptr<SVM> SVM_params)
{
    Mat out;
    /*Mat structElement2 = getStructuringElement(MORPH_RECT, Size(2, 2));
    dilate(CutImage, out, structElement2);
    imshow("�����δ���", out);*/

    medianBlur(CutImage, out, 3);//��ֵ�˲�
    Mat pretreatment = out.clone();
    
    //imshow("Ԥ������", pretreatment);

    //*************************************��������*************************************////
    vector<vector<Point>>contours;
    findContours(out, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); //��������������������ͼ�����Ϊ��ֵͼ
    Mat outline = CutImage.clone();
    drawContours(outline, contours, -1, Scalar(0, 0, 255), 2);// ������������ Scalar(B, G, R)

    //*********************************�޳�������Χ������*************************************///
    Mat result = CutImage.clone();
    vector<std::vector<cv::Point>>::const_iterator itc = contours.begin();

    int cmin = 10, cmax = 500;  //�޳�������Χ������
    while (itc != contours.end()) {
        
        if (itc->size() < cmin || itc->size() > cmax)
            itc = contours.erase(itc);
        else
            ++itc;
    }

    //*********************************����������������С��Χ����******************************///
    map<int, int> Area;
    parameter_type parameter;
    size_t digitalNum = contours.size();

    cout << "�ָ������������" << endl;
    for (int i = 0; i < digitalNum; i++)
    {
        Rect r = boundingRect(Mat(contours[i]));
        rectangle(outline, r, Scalar(255, 0, 0), 1);
        // ��¼ÿ�����������ͳ���
        parameter.x.push_back(r.x);
        parameter.y.push_back(r.y);
        parameter.width.push_back(r.width);
        parameter.height.push_back(r.height);
        Area.insert(pair<int, int>(r.area(), r.x));  // ��¼��������Ͷ�Ӧ��x����
        cout << r.area() << endl;
    }
    cout << endl;

    // �Ƚ���С���������������5����С���С���������������С����
    int pointIndex = -1;
    int minArea = Area.begin()->first;
    auto map_temp = Area.end();
    map_temp--;
    int maxArea = map_temp->first;
    cout << "С�����ж�";
    if (minArea * 5 < maxArea) {
        cout << "����ͼ���д���С����" << endl;
        pointIndex = Area.begin()->second;
    }
    else {
        pointIndex = -1;
        cout << "����ͼ��������" << endl << endl;
    }

    char rectangular[50];
    sprintf_s((char *)rectangular, 50, "�����ο�λ�á�%d", rowNum);
    imshow(rectangular, outline);

    //************************************* �и����λ�ľ���************************************///
    char rectnum[255];
    //char file[10];
    Mat pic;

    vector<map<int, Mat>> numberList;
    map<int, Mat> number;

    for (int i = 0; i < digitalNum; i++)
    {
        //sprintf_s(file, 10, "%d-%d", rowNum, i);
        sprintf_s(rectnum, 255, "C:\\Users\\XGH\\Desktop\\����\\%d-%d.png", rowNum, i);
        Rect r(parameter.x[i], parameter.y[i], parameter.width[i], parameter.height[i]);
        pic = pretreatment(r);//ѡ��roi
        resize(pic, pic, Size(32, 32));//���и��ͼƬ���ų�32*32
        number.insert(pair<int, Mat>(parameter.x[i], pic));

        imwrite(rectnum, pic);
        //imshow(file, pic);
    }

    // ����ʶ��
    Recognition recognition;
    recognition.identifyNum(number, pointIndex, SVM_params);

    number.erase(number.begin(), number.end()); // ���map�е�����
    Area.erase(Area.begin(), Area.end());
    numberList.clear();
    parameter.x.clear();
    parameter.y.clear();
    parameter.width.clear();
    parameter.height.clear();

    return digitalNum;
}

// ͶӰ���ָ�����
/*cout << "�ָ�����" << endl;
 CvSize pSize;
 CvRect RectLine;
 pSize.width = CutImage.cols;  // ��ȡͼ����
 pSize.height = CutImage.rows; // ��ȡͼ��߶�
 char WinName[18]; // ��������

 string  CutSavePath = "C:\\Users\\XGH\\Desktop\\����\\Num-%d-%d.png";
 int PathLenght = CutSavePath.length() + 3;
 int picID = 0;

 int perPixelValue;//ÿ�����ص�ֵ
 int* projectValArry = new int[pSize.width];//����һ�����ڴ���ÿ�а�ɫ���ظ���������
 memset(projectValArry, 0, pSize.width * sizeof(int));//�����ʼ������

 //����ÿһ�е�ͼ��Ҷ�ֵ������ÿһ��255��ֵ
 for (int col = 0; col < pSize.width; ++col)
 {
     for (int row = 0; row < pSize.height; ++row)
     {
         perPixelValue = CutImage.at<uchar>(row, col);
         if (perPixelValue == 255)//����Ǻڵװ���
         {
             projectValArry[col]++;
         }
     }
 }

 �½�һ��Mat���ڴ���ͶӰֱ��ͼ����������Ϊ��ɫ
 Mat verticalProjectionMat(pSize.height, pSize.width, CV_8UC1);
 for (int i = 0; i < pSize.height; i++)
 {
     for (int j = 0; j < pSize.width; j++)
     {
         perPixelValue = 255;  //��������Ϊ��ɫ��
         verticalProjectionMat.at<uchar>(i, j) = perPixelValue;
     }
 }

 ��ֱ��ͼ��������Ϊ��ɫ
 for (int i = 0; i < pSize.width; i++)
 {
     for (int j = 0; j < projectValArry[i]; j++)
     {
         perPixelValue = 0;  //ֱ��ͼ����Ϊ��ɫ
         verticalProjectionMat.at<uchar>(pSize.height - 1 - j, i) = perPixelValue;
     }
 }
 imshow("��ͶӰ��", verticalProjectionMat);
 //delete[] projectValArry;//��Ҫ����ɾ������ռ�

 vector<Mat> roiList;//���ڴ���ָ������ÿ���ַ�
 int startIndex = 0;//��¼�����ַ���������
 int endIndex = 0;//��¼����հ����������
 bool inBlock = false;//�Ƿ���������ַ�����
 Mat ROI;

 for (int i = 0; i < CutImage.cols; ++i)
 {
     if (!inBlock && projectValArry[i] != 0)//�����ַ�����
     {
         inBlock = true;
         startIndex = i;
         cout << "startIndex is " << startIndex << endl;
     }
     else if (projectValArry[i] == 0 && inBlock)//����հ�����
     {
         // LinePath����ͼ��Ĵ洢·����ÿ��ѭ�������ı�
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

         // ����ʶ��
         Recognition recognition;
         recognition.identifyNum(ROI, ++picID, SVM_params);

         //�������ͼ��
         sprintf_s(LinePath, PathLenght, "C:\\Users\\XGH\\Desktop\\����\\Num-%d-%d.png", rowNum, picID);
         imwrite(LinePath, ROI);
     }
 }

 return 1;*/