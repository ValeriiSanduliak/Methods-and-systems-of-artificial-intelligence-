#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_LabVs.h"
#include"imageCropWidget.h"
class LabVs : public QMainWindow
{
    Q_OBJECT

public:
    LabVs(QWidget *parent = nullptr);
    ~LabVs();
    

// Slots for buttons
    void on_buttonTransform_clicked(); // Slot for image transformation

    void on_buttonNext1_clicked();
    void on_buttonCrop_clicked();
    void on_buttonSegmentation_clicked();

    void on_buttonShowData_clicked();
    
    void on_buttonSelectImages1_clicked(); // Slot for image selection
    void on_buttonSelectImages2_clicked();
    void on_buttonSelectImages3_clicked();
    void on_buttonSelectImages4_clicked();


    void on_buttonTransform4_clicked();
    void on_buttonSegmentation4_clicked();
    void on_buttonCompare_clicked();
    void on_buttonCompareD_clicked();


    void on_buttonTransform_All_clicked();




protected:
    void convertImagesToGrayscale(QVector<QImage>& imagesLocal);
    void convertGrayPixelsToBlack(int threshold, QVector<QImage>& imagesLocal);


    QImage autoCropImage(const QImage& image);


    void SegmentImage(QVector<QImage>& imagesLocal, QMap<int, QVector<int>>& imageSegmentationDataLocal);

    void normalizeData();

    void normalizeData(QMap<int, QVector<int>>& imageSegmentationDataLocal, QMap<int, QVector<double>> &dataS, QMap<int, QVector<double>> &dataM);
    void LoadImage(QVector<QImage> &imagesLocal);
    void displayImagesInLoop(QVector<QImage>& imagesLocal);

    void FindMaxMinValue(QMap<int, QVector<double>>& data, QVector<double>& max, QVector<double>& min);
   

    double calculateEuclideanDistance(const QVector<double>& vec1, const QVector<double>& vec2);
    QVector<double> calculateColumnMeans(const QMap<int, QVector<double>>& data);

private:
    Ui::LabVsClass ui;
    QVector<QImage> images;



    // 3 classes of images
    QVector<QImage> imagesClass1;
    QVector<QImage> imagesClass2;
    QVector<QImage> imagesClass3;

    QVector<QImage> imagesCompare;

    // 3 classes of images
    QMap<int, QVector<int>> imageSegmentationDataClass1;
    QMap<int, QVector<int>> imageSegmentationDataClass2;
    QMap<int, QVector<int>> imageSegmentationDataClass3;

    QMap<int, QVector<int>> imageSegmentationDataCompare;


    int currentImageIndex;
    ImageCropWidget* imageCropWidget;
    QMap<int, QVector<int>> imageSegmentationData;



    QMap<int, QVector<double>> Class1SanduliakS1;
    QMap<int, QVector<double>> Class1SanduliakM1;

	QMap<int, QVector<double>> Class2SanduliakS2;
    QMap<int, QVector<double>> Class2SanduliakM2;

    QMap<int, QVector<double>> Class3SanduliakS3;
    QMap<int, QVector<double>> Class3SanduliakM3;


    QMap<int, QVector<double>> CompareSanduliakS1;
    QMap<int, QVector<double>> CompareSanduliakM1;

    QVector<double> SanduliakS1MAX;
    QVector<double> SanduliakS1MIN;


    QVector<double> SanduliakS2MAX;
    QVector<double> SanduliakS2MIN;

    QVector<double> SanduliakS3MAX;
    QVector<double> SanduliakS3MIN;


    QVector<double> SanduliakS1Center;
    QVector<double> SanduliakS2Center;
    QVector<double> SanduliakS3Center;


    double d1;
    double d2;
    double d3;
};
