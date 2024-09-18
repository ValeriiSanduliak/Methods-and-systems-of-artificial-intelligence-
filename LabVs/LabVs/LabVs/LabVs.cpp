#include "LabVs.h"
#include <QFileDialog>
LabVs::LabVs(QWidget *parent)
    : QMainWindow(parent), currentImageIndex(0)
{
    ui.setupUi(this);

    QObject::disconnect(ui.buttonSelectImages1, nullptr, nullptr, nullptr);
    imageCropWidget = new ImageCropWidget(this);
    connect(ui.buttonSelectImages1, &QPushButton::clicked, this, &LabVs::on_buttonSelectImages1_clicked);



    connect(ui.buttonTransform, &QPushButton::clicked, this, &LabVs::on_buttonTransform_clicked);

    connect(ui.buttonNextImage1, &QPushButton::clicked, this, &LabVs::on_buttonNext1_clicked);


    connect(ui.buttonCrop, &QPushButton::clicked, this, &LabVs::on_buttonCrop_clicked);

    connect(ui.buttonSegm, &QPushButton::clicked, this, &LabVs::on_buttonSegmentation_clicked);

    connect(ui.buttonShowData, &QPushButton::clicked, this, &LabVs::on_buttonShowData_clicked);
}

LabVs::~LabVs()
{

}

void LabVs::convertImagesToGrayscale()
{
    for (int i = 0; i < images.size(); ++i) {
        QImage& image = images[i];
        QImage grayscaleImage = image.convertToFormat(QImage::Format_Grayscale8);
        images[i] = grayscaleImage;
    }
}

void LabVs::on_buttonSelectImages1_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Select Images", "", "Images (*.png *.jpg)");
    if (!fileNames.isEmpty()) {
        images.clear();  // Очистити попередні зображення
        for (const QString& fileName : fileNames) {
            QImage image(fileName);
            if (!image.isNull()) {
                // Масштабуємо зображення точно до розміру 500x500 без збереження пропорцій
                image = image.scaled(500, 500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                images.append(image);
            }
            else {
                qWarning() << "Failed to load image:" << fileName;
            }
        }
        if (!images.isEmpty()) {
            currentImageIndex = 0;  
            imageCropWidget->setGeometry(0, 0, 500, 500); 
            imageCropWidget->setImage(images[currentImageIndex]);
        }
        else {
            imageCropWidget->clearImage();
        }
    }

}

void LabVs::on_buttonTransform_clicked()
{
    convertImagesToGrayscale();

    int threshold = ui.lineEdit->text().toInt();

    convertGrayPixelsToBlack(threshold);

    imageCropWidget->setImage(images[currentImageIndex]);
}

void LabVs::on_buttonNext1_clicked()
{

    if (images.isEmpty()) {
        imageCropWidget->clearImage(); 
        return;
    }
    currentImageIndex = (currentImageIndex + 1) % images.size();
    imageCropWidget->setImage(images[currentImageIndex]);

}

void LabVs::on_buttonCrop_clicked()
{

    if (images.isEmpty()) {
        return;  
    }

    QImage croppedImage = imageCropWidget->getCroppedImage();
    if (!croppedImage.isNull()) {
        images[currentImageIndex] = croppedImage;
        imageCropWidget->setImage(images[currentImageIndex]);
        imageCropWidget->update();
    }
}

void LabVs::on_buttonSegmentation_clicked()
{
    int numberOfSectors = ui.sectorSpinBox->value();
    if (images.isEmpty()) {
        return;
    }

    QImage& image = images[currentImageIndex];
    QSize imageSize = image.size();

    // Центр зображення тепер у правому нижньому куті
    QPoint center(imageSize.width(), imageSize.height());

    QVector<int> blackPixelCounts(numberOfSectors, 0);

    // Крок кута для кожного сектору
    double angleStep = 90.0 / numberOfSectors;

    // Проходимо по всіх пікселях зображення
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            int gray = qGray(pixel);  // Отримуємо градацію сірого

            if (gray == 0) {  // Якщо піксель чорний
                QPoint pixelPoint(x, y);
                QPoint vector = pixelPoint - center;  // Вектор від пікселя до центра

                // Обчислюємо кут у діапазоні від 0 до 90 градусів
                double angle = std::atan2(std::abs(vector.y()), std::abs(vector.x())) * 180.0 / M_PI;

                // Визначаємо сектор за кутом
                int sectorIndex = static_cast<int>(angle / angleStep);
                blackPixelCounts[sectorIndex]++;
            }
        }
    }

    // Зберігаємо результат у QMap для поточного зображення
    imageSegmentationData[currentImageIndex] = blackPixelCounts;

    // Формуємо результат у вигляді рядка
    QString result = "image " + QString::number(currentImageIndex + 1) + " vector: [";
    for (int i = 0; i < numberOfSectors; ++i) {
        result += QString::number(blackPixelCounts[i]);
        if (i < numberOfSectors - 1) {
            result += ", ";  // Додаємо коми між елементами
        }
    }
    result += "]";

    // Виводимо результат
    qDebug() << result;

    // Оновлюємо віджет
    imageCropWidget->setSectors(numberOfSectors);
    imageCropWidget->update();
}

void LabVs::on_buttonShowData_clicked()
{
    // Очищуємо текстове поле перед виведенням нових даних
    ui.plainTextEdit->clear();
    if (imageSegmentationData.isEmpty()) {
		return; // Немає даних для виведення
	}
    // Проходимо всі зображення та їхні сегментаційні дані
    for (auto it = imageSegmentationData.begin(); it != imageSegmentationData.end(); ++it) {
        int imageIndex = it.key();
        QVector<int> blackPixelCounts = it.value();

        // Формуємо текст для виведення
        QString result = "image " + QString::number(imageIndex + 1) + " vector: [";
        for (int i = 0; i < blackPixelCounts.size(); ++i) {
            result += QString::number(blackPixelCounts[i]);
            if (i < blackPixelCounts.size() - 1) {
                result += ", "; // Додаємо коми між елементами
            }
        }
        result += "]\n";

        // Додаємо текст до plainTextEdit
        ui.plainTextEdit->appendPlainText(result);
    }

    normalizeData();
}


void LabVs::convertGrayPixelsToBlack(int threshold)
{
    for (QImage& image : images) {
        for (int y = 0; y < image.height(); ++y) {
            for (int x = 0; x < image.width(); ++x) {
                QRgb pixel = image.pixel(x, y);
                int gray = qGray(pixel);
                int newPixel = (gray < threshold) ? 0 : 255;
                image.setPixel(x, y, qRgb(newPixel, newPixel, newPixel));
            }
        }
    }
}


void LabVs::normalizeData()
{
    // Масиви для збереження нормованих векторів
    QMap<int, QVector<double>> SanduliakS1;
    QMap<int, QVector<double>> SanduliakM1;

    // Проходимо всі зображення та їхні вектори ознак
    for (auto it = imageSegmentationData.begin(); it != imageSegmentationData.end(); ++it) {
        int imageIndex = it.key();
        QVector<int> blackPixelCounts = it.value();

        // Нормування за сумою 1 (S1)
        int totalBlackPixels = std::accumulate(blackPixelCounts.begin(), blackPixelCounts.end(), 0);
        QVector<double> normalizedS1;
        for (int count : blackPixelCounts) {
            double normalizedValue = static_cast<double>(count) / totalBlackPixels;
            normalizedS1.append(normalizedValue);
        }
        SanduliakS1[imageIndex] = normalizedS1;

        // Нормування за модулем 1 (M1)
        int maxBlackPixels = *std::max_element(blackPixelCounts.begin(), blackPixelCounts.end());
        QVector<double> normalizedM1;
        for (int count : blackPixelCounts) {
            double normalizedValue = static_cast<double>(count) / maxBlackPixels;
            normalizedM1.append(normalizedValue);
        }
        SanduliakM1[imageIndex] = normalizedM1;
    }


    for (auto it = SanduliakS1.begin(); it != SanduliakS1.end(); ++it) {
        int imageIndex = it.key();
        QVector<double> normalizedS1 = it.value();
        QVector<double> normalizedM1 = SanduliakM1[imageIndex];

        qDebug() << "image" << imageIndex + 1 << "S1 vector: " << normalizedS1;
        qDebug() << "image" << imageIndex + 1 << "M1 vector: " << normalizedM1;
    }
}