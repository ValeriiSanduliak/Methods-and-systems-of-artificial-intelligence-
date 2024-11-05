#include "LabVs.h"
#include <QFileDialog>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QtMath> // для функції sqrt()
#include <QVector>
#include <QMap>

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


    connect(ui.buttonSelectImages2, &QPushButton::clicked, this, &LabVs::on_buttonSelectImages2_clicked);

    connect(ui.buttonSelectImages3, &QPushButton::clicked, this, &LabVs::on_buttonSelectImages3_clicked);

    connect(ui.buttonSelectImages4, &QPushButton::clicked, this, &LabVs::on_buttonSelectImages4_clicked);

    connect(ui.buttonTransform_4, &QPushButton::clicked, this, &LabVs::on_buttonTransform4_clicked);

    connect(ui.buttonSegm_2, &QPushButton::clicked, this, &LabVs::on_buttonSegmentation4_clicked);

    // Connect the buttonTransform_All button to the on_buttonTransform_All_clicked slot
    connect(ui.buttonTransform_All, &QPushButton::clicked, this, &LabVs::on_buttonTransform_All_clicked);




    connect(ui.buttonCompare, &QPushButton::clicked, this, &LabVs::on_buttonCompare_clicked);

    connect(ui.buttonCompare_2, &QPushButton::clicked, this, &LabVs::on_buttonCompareD_clicked);

}

LabVs::~LabVs()
{

}



void LabVs::on_buttonSelectImages1_clicked()
{
    LoadImage(imagesClass1);
}



void LabVs::on_buttonTransform_clicked()
{
    convertImagesToGrayscale(imagesClass1);

    int threshold = ui.lineEdit->text().toInt();

    convertGrayPixelsToBlack(threshold, imagesClass1);
    
    displayImagesInLoop(imagesClass1);
    
    for (int i = 0; i < imagesClass1.size(); ++i) {
		QImage& image = imagesClass1[i];
		QImage croppedImage = autoCropImage(image);
		imagesClass1[i] = croppedImage;
	}

    displayImagesInLoop(imagesClass1);
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
    SegmentImage(imagesClass1, imageSegmentationDataClass1);

    qDebug() << "Segmentation for class 1 is done";

    qDebug() << "Segmentation for class 2 is started";

    SegmentImage(imagesClass2, imageSegmentationDataClass2);

    qDebug() << "Segmentation for class 2 is done";

    qDebug() << "Segmentation for class 3 is started";

    SegmentImage(imagesClass3, imageSegmentationDataClass3);

    qDebug() << "Segmentation for class 3 is done";

   

}

void LabVs::on_buttonShowData_clicked()
{
    // Очищуємо текстове поле перед виведенням нових даних

   /* if (imageSegmentationDataClass1.isEmpty() || imageSegmentationDataClass2.isEmpty() || imageSegmentationDataClass3.isEmpty()){
		return;
	}*/

    ui.plainTextEdit->clear();
    // Проходимо всі зображення та їхні сегментаційні дані
    for (auto it = imageSegmentationDataClass1.begin(); it != imageSegmentationDataClass1.end(); ++it) {
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
    
    qDebug() << "Normalization for class 1:";
    normalizeData(imageSegmentationDataClass1, Class1SanduliakS1, Class1SanduliakM1);


    ui.plainTextEdit_2->clear();

    for (auto it = imageSegmentationDataClass2.begin(); it != imageSegmentationDataClass2.end(); ++it) {
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
        ui.plainTextEdit_2->appendPlainText(result);
    }

    qDebug() << "Normalization for class 2:";
    normalizeData(imageSegmentationDataClass2, Class2SanduliakS2, Class2SanduliakM2);

    ui.plainTextEdit_3->clear();


    for (auto it = imageSegmentationDataClass2.begin(); it != imageSegmentationDataClass2.end(); ++it) {
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
        ui.plainTextEdit_3->appendPlainText(result);
    }
    qDebug() << "Normalization for class 3:";
    normalizeData(imageSegmentationDataClass3, Class3SanduliakS3, Class3SanduliakM3);


    qDebug()<<"Finding max and min values for class 1";
    FindMaxMinValue(Class1SanduliakS1, SanduliakS1MAX, SanduliakS1MIN);
    qDebug() << "Finding max and min values for class 2";
    FindMaxMinValue(Class2SanduliakS2, SanduliakS2MAX, SanduliakS2MIN);
    qDebug() << "Finding max and min values for class 3";
    FindMaxMinValue(Class3SanduliakS3, SanduliakS3MAX, SanduliakS3MIN);
  


    SanduliakS1Center = calculateColumnMeans(Class1SanduliakS1);
    SanduliakS2Center = calculateColumnMeans(Class2SanduliakS2);
    SanduliakS3Center = calculateColumnMeans(Class3SanduliakS3);
    // Виводимо результат
    qDebug() << "Center for class 1: " << SanduliakS1Center;
    qDebug() << "Center for class 2: " << SanduliakS2Center;
    qDebug() << "Center for class 3: " << SanduliakS3Center;

}

void LabVs::on_buttonSelectImages2_clicked()
{
    LoadImage(imagesClass2);
}

void LabVs::on_buttonSelectImages3_clicked()
{
    LoadImage(imagesClass3);
}

void LabVs::on_buttonSelectImages4_clicked()
{
   imageCropWidget->clearImage();
   imagesCompare.clear();
   LoadImage(imagesCompare);
   d1 = 0;
   d2 = 0;
   d3 = 0;
}

void LabVs::on_buttonTransform4_clicked()
{
    convertImagesToGrayscale(imagesCompare);

    int threshold = ui.lineEdit->text().toInt();

    convertGrayPixelsToBlack(threshold, imagesCompare);

    displayImagesInLoop(imagesCompare);

    for (int i = 0; i < imagesCompare.size(); ++i) {
        QImage& image = imagesCompare[i];
        QImage croppedImage = autoCropImage(image);
        imagesCompare[i] = croppedImage;
    }

    displayImagesInLoop(imagesCompare);
}

void LabVs::on_buttonSegmentation4_clicked()
{

    // Запускаємо сегментацію та нормалізацію в окремому потоці
    QtConcurrent::run([this]() {
        imageCropWidget->clearImage();
        //imageSegmentationDataCompare.clear();
        //CompareSanduliakM1.clear();
        //CompareSanduliakS1.clear();
        SegmentImage(imagesCompare, imageSegmentationDataCompare);
        //qDebug() << "Segmentation for compare images is done";
        //qDebug() << "Normalization for compare images:";
        normalizeData(imageSegmentationDataCompare, CompareSanduliakS1, CompareSanduliakM1);
        });

}

void LabVs::on_buttonCompare_clicked()
{
    if (CompareSanduliakS1.isEmpty()) {
        qDebug() << "No data for comparison.";
        return;
    }

    QVector<double> compareVector = CompareSanduliakS1[0]; // Використовуємо перше зображення для порівняння

    // Перевіряємо для кожного класу
    bool isClass1 = true, isClass2 = true, isClass3 = true;

    // Перевіряємо для класу 1
    for (int i = 0; i < compareVector.size(); i++) {
        if (!(compareVector[i] >= SanduliakS1MIN[i] && compareVector[i] <= SanduliakS1MAX[i])) {

            isClass1 = false;
            break;
        }
        qDebug()<< SanduliakS1MIN[i] << " " << compareVector[i] << " " << SanduliakS1MAX[i];
    }

    // Перевіряємо для класу 2
    for (int i = 0; i < compareVector.size(); i++) {
        if (!(compareVector[i] >= SanduliakS2MIN[i] && compareVector[i] <= SanduliakS2MAX[i])) {
            isClass2 = false;
            break;
        }
    }

    // Перевіряємо для класу 3
    for (int i = 0; i < compareVector.size(); i++) {
        if (!(compareVector[i] >= SanduliakS3MIN[i] && compareVector[i] <= SanduliakS3MAX[i])) {
            isClass3 = false;
            break;
        }
    }

    // Виводимо результат
    if (isClass1) {
        qDebug() << "The image belongs to class 1";
    }
    else if (isClass2) {
        qDebug() << "The image belongs to class 2";
    }
    else if (isClass3) {
        qDebug() << "The image belongs to class 3";
    }
    else {
        qDebug() << "The image does not belong to any class";
    }
}

void LabVs::on_buttonCompareD_clicked()
{
  /*  d1 = calculateEuclideanDistance(CompareSanduliakS1[0], SanduliakS1Center);
    d2 = calculateEuclideanDistance(CompareSanduliakS1[0], SanduliakS2Center);
    d3 = calculateEuclideanDistance(CompareSanduliakS1[0], SanduliakS3Center);

    qDebug() << "Distance to class 1: " << d1;
    qDebug() << "Distance to class 2: " << d2;
    qDebug() << "Distance to class 3: " << d3;

    if (d1 < d2 && d1 < d3) {
		qDebug() << "The image belongs to class 1";
	}
	else if (d2 < d1 && d2 < d3) {
		qDebug() << "The image belongs to class 2";
	}
	else if (d3 < d1 && d3 < d2) {
		qDebug() << "The image belongs to class 3";
	}
	else {
		qDebug() << "The image does not belong to any class";
	}*/

	parceptt();
}



void LabVs::on_buttonTransform_All_clicked()
{
    convertImagesToGrayscale(imagesClass1);
    convertImagesToGrayscale(imagesClass2);
    convertImagesToGrayscale(imagesClass3);

    int threshold = ui.lineEdit->text().toInt();

    convertGrayPixelsToBlack(threshold, imagesClass1);
    convertGrayPixelsToBlack(threshold, imagesClass2);
    convertGrayPixelsToBlack(threshold, imagesClass3);

    for (int i = 0; i < imagesClass1.size(); ++i) {
        QImage& image = imagesClass1[i];
        QImage croppedImage = autoCropImage(image);
        imagesClass1[i] = croppedImage;
    }

    for (int i = 0; i < imagesClass2.size(); ++i) {
		QImage& image = imagesClass2[i];
		QImage croppedImage = autoCropImage(image);
		imagesClass2[i] = croppedImage;
	}

    for (int i = 0; i < imagesClass3.size(); ++i){
		QImage& image = imagesClass3[i];
		QImage croppedImage = autoCropImage(image);
		imagesClass3[i] = croppedImage;
	}
    displayImagesInLoop(imagesClass1);
    displayImagesInLoop(imagesClass2);
    displayImagesInLoop(imagesClass3);
}


void LabVs::convertImagesToGrayscale(QVector<QImage> &imagesLocal)
{
    for (int i = 0; i < imagesLocal.size(); ++i) {
        QImage& image = imagesLocal[i];
        QImage grayscaleImage = image.convertToFormat(QImage::Format_Grayscale8);
        imagesLocal[i] = grayscaleImage;
    }
}


void LabVs::convertGrayPixelsToBlack(int threshold, QVector<QImage>& imagesLocal)
{
    for (QImage& image : imagesLocal) {
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

QImage LabVs::autoCropImage(const QImage& image) {
    int left = image.width(), right = 0, top = image.height(), bottom = 0;

    // Проходимо по пікселях, щоб знайти межі, де немає білих пікселів (255)
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (qGray(image.pixel(x, y)) < 255) {  // Якщо піксель не білий
                if (x < left) left = x;
                if (x > right) right = x;
                if (y < top) top = y;
                if (y > bottom) bottom = y;
            }
        }
    }

    // Перевірка, чи не всі пікселі білі
    if (left > right || top > bottom) {
        // Якщо все біле, повертаємо оригінальне зображення
        return image;
    }

    // Обрізаємо зображення до виявлених меж
    QRect cropRect(left, top, right - left + 1, bottom - top + 1);
    return image.copy(cropRect);
}


void LabVs::SegmentImage(QVector<QImage>& imagesLocal, QMap<int, QVector<int>>& imageSegmentationDataLocal)
{
    int numberOfSectors = ui.sectorSpinBox->value();
    if (imagesLocal.isEmpty()) {
        return;
    }

    for (int i = 0; i < imagesLocal.size(); i++) {

        QImage& image = imagesLocal[i];
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
        imageSegmentationDataLocal[i] = blackPixelCounts;

        // Формуємо результат у вигляді рядка
        QString result = "image " + QString::number(i + 1) + " vector: [";
        for (int ir = 0; ir < numberOfSectors; ++ir) {
            result += QString::number(blackPixelCounts[ir]);
            if (ir < numberOfSectors - 1) {
                result += ", ";  // Додаємо коми між елементами
            }
        }
        result += "]";

        // Виводимо результат
        qDebug() << result;
        imageCropWidget->clearImage();
        imageCropWidget->setImage(imagesLocal[i]);
        // Можливо, потрібно оновити UI для відображення зображень одразу
       // QApplication::processEvents();
        QThread::sleep(0.5); // Пауза між відображенням зображень (1 секунда)
        imageCropWidget->setSectors(numberOfSectors);
        imageCropWidget->update();
    }
}

void LabVs::normalizeData(QMap<int, QVector<int>>& imageSegmentationDataLocal, QMap<int, QVector<double>> &dataS, QMap<int, QVector<double>> &dataM)
{

    for (auto it = imageSegmentationDataLocal.begin(); it != imageSegmentationDataLocal.end(); ++it) {
        int imageIndex = it.key();
        QVector<int> blackPixelCounts = it.value();

        // Нормування за сумою 1 (S1)
        int totalBlackPixels = std::accumulate(blackPixelCounts.begin(), blackPixelCounts.end(), 0);
        QVector<double> normalizedS1;
        for (int count : blackPixelCounts) {
            double normalizedValue = static_cast<double>(count) / totalBlackPixels;
            normalizedS1.append(normalizedValue);
        }
        dataS[imageIndex] = normalizedS1;

        // Нормування за модулем 1 (M1)
        int maxBlackPixels = *std::max_element(blackPixelCounts.begin(), blackPixelCounts.end());
        QVector<double> normalizedM1;
        for (int count : blackPixelCounts) {
            double normalizedValue = static_cast<double>(count) / maxBlackPixels;
            normalizedM1.append(normalizedValue);
        }
        dataM[imageIndex] = normalizedM1;
    }

    for (auto it = dataS.begin(); it != dataS.end(); ++it) {
        int imageIndex = it.key();
        QVector<double> normalizedS1 = it.value();
        QVector<double> normalizedM1 = dataM[imageIndex];

        qDebug() << "image" << imageIndex + 1 << "S vector: " << normalizedS1;
       // qDebug() << "image" << imageIndex + 1 << "M vector: " << normalizedM1;
    }
}

void LabVs::LoadImage(QVector<QImage> &imagesLocal)
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Select Images", "", "Images (*.png *.jpg)");
    if (!fileNames.isEmpty()) {
        images.clear();  // Очистити попередні зображення
        for (const QString& fileName : fileNames) {
            QImage image(fileName);
            if (!image.isNull()) {
                // Масштабуємо зображення точно до розміру 500x500 без збереження пропорцій
                image = image.scaled(500, 500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                imagesLocal.append(image);
            }
            else {
                qWarning() << "Failed to load image:" << fileName;
            }
        }
        displayImagesInLoop(imagesLocal);
    }
}


void LabVs::displayImagesInLoop(QVector<QImage>& imagesLocal) {
    if (!imagesLocal.isEmpty()) {
        for (int i = 0; i < imagesLocal.size(); ++i) {
            imageCropWidget->setImage(imagesLocal[i]);
            // Можливо, потрібно оновити UI для відображення зображень одразу
            QApplication::processEvents();
            QThread::sleep(0.5); // Пауза між відображенням зображень (1 секунда)
        }
    }
}

void LabVs::FindMaxMinValue(QMap<int, QVector<double>>& data, QVector<double>& max, QVector<double>& min)
{
    if (data.isEmpty()) return; // Перевіряємо чи дані не порожні

    int vectorSize = data.first().size(); // Розмір вектора для стовпців
    max.fill(std::numeric_limits<double>::lowest(), vectorSize); // Заповнюємо max мінімальними можливими значеннями
    min.fill(std::numeric_limits<double>::max(), vectorSize);    // Заповнюємо min максимальними можливими значеннями

    for (auto it = data.begin(); it != data.end(); ++it) {
        QVector<double> normalized = it.value();

        for (int i = 0; i < normalized.size(); ++i) {
            if (normalized[i] > max[i]) {
                max[i] = normalized[i]; // Оновлюємо максимум для кожного стовпця
            }
            if (normalized[i] < min[i]) {
                min[i] = normalized[i]; // Оновлюємо мінімум для кожного стовпця
            }
        }
    }

    // Виводимо результат
    for (int i = 0; i < max.size(); ++i) {
        qDebug() << "MaxMin Value for column " << i + 1 << " is [ " << max[i] << " , " << min[i] << " ]";
    }
}




double LabVs::calculateEuclideanDistance(const QVector<double>& vec1, const QVector<double>& vec2)
{
    // Перевірка, чи вектори мають однакову довжину
    if (vec1.size() != vec2.size()) {
        qWarning("Вектори повинні бути однакової довжини!");
        return -1.0; // Повертаємо помилкове значення
    }

    double sum = 0.0;

    // Обчислюємо суму квадратів різниць компонент
    for (int i = 0; i < vec1.size(); ++i) {
        sum += qPow(vec1[i] - vec2[i], 2);
    }

    // Повертаємо квадратний корінь з цієї суми
    return qSqrt(sum);
}

QVector<double> LabVs::calculateColumnMeans(const QMap<int, QVector<double>>& data)
{
    if (data.isEmpty()) {
        return {};
    }

    int vectorSize = data.first().size(); // Передбачається, що всі вектори мають однаковий розмір
    QVector<double> means(vectorSize, 0.0); // Вектор для зберігання середніх значень
    int numVectors = data.size(); // Кількість векторів (зображень)

    // Обчислення суми для кожної компоненти (стовпця)
    for (auto it = data.begin(); it != data.end(); ++it) {
        const QVector<double>& vec = it.value(); // Беремо кожен вектор

        for (int i = 0; i < vectorSize; ++i) {
            means[i] += vec[i]; // Сумуємо компоненти
        }
    }

    // Ділимо суму кожної компоненти на кількість векторів, щоб отримати середнє
    for (int i = 0; i < vectorSize; ++i) {
        means[i] /= numVectors;
    }

    return means;
}

#include <QVector>
#include <QMap>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

void LabVs::parceptt()
{
    QVector<QVector<double>> trainingData;
    QVector<double> trainingLabels;

    // Збираємо тренувальні дані
    for (auto it = Class1SanduliakS1.begin(); it != Class1SanduliakS1.end(); ++it) {
        QVector<double> data = it.value();
        data.push_back(0); // мітка класу 1
        trainingData.append(data);
        trainingLabels.push_back(0);
    }
    for (auto it = Class2SanduliakS2.begin(); it != Class2SanduliakS2.end(); ++it) {
        QVector<double> data = it.value();
        data.push_back(1); // мітка класу 0
        trainingData.append(data);
        trainingLabels.push_back(1);
    }

    // Ініціалізуємо ваги
    QVector<double> weights(trainingData[0].size(), 0);
    randomize_weights(weights); // Випадкова ініціалізація ваг

    // Тренування
    training_function(trainingData, weights);

    // Тестування
    for (auto it = CompareSanduliakS1.begin(); it != CompareSanduliakS1.end(); ++it) {
        QVector<double> testData = it.value();
        testData.push_back(0); // Додаємо для сумісності з вагою
        int result = classify(testData, weights);
        std::cout << "Test input: ";
        for (const auto& val : testData) {
            std::cout << val << " "; // Виводимо кожен елемент вектора
        }
        std::cout << " classified as: " << result << "\n";
    }
}


