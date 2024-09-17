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
    void convertImagesToGrayscale();
    void on_buttonSelectImages1_clicked(); // Slot for image selection
    void on_buttonTransform_clicked(); // Slot for image transformation
    void on_buttonNext1_clicked();
    void on_buttonCrop_clicked();
    void on_buttonSegmentation_clicked();
    void on_buttonShowData_clicked();
protected:
    void convertGrayPixelsToBlack(int threshold);
    void normalizeData();
private:
    Ui::LabVsClass ui;
    QVector<QImage> images;
    int currentImageIndex;
    ImageCropWidget* imageCropWidget;
    QMap<int, QVector<int>> imageSegmentationData;

};
