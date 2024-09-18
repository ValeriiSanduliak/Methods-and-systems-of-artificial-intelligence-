#include "imageCropWidget.h"
#include <QPainter>
#include <QMouseEvent>
ImageCropWidget::ImageCropWidget(QWidget* parent)
    : QWidget(parent), selecting(false)
{
    this->move(0, 0);
}

void ImageCropWidget::setImage(const QImage& image)
{
    this->image = image;
    this->setGeometry(0, 0, image.width(), image.height());
    update();  // Перемалювати віджет
}

QImage ImageCropWidget::getCroppedImage() const
{
    if (selectionRect.isNull() || image.isNull()) {
        return QImage();  // Повернути порожнє зображення, якщо область обрізання не задана або зображення відсутнє
    }

    // Визначення координат області обрізання в межах віджету
    QRect widgetRect(QPoint(0, 0), this->size());
    QRect croppedRect = selectionRect & widgetRect;

    // Обрізати зображення та повернути його
    return image.copy(croppedRect);
}

void ImageCropWidget::clearImage()
{
    this->image = QImage();
    selectionRect = QRect();
    update();  // Перемалювати віджет
}

void ImageCropWidget::setSectors(int sectors)
{
    numberOfSectors = sectors;
    update();
}

void ImageCropWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    if (!image.isNull()) {
        // Малюємо зображення
        QSize widgetSize = this->size();
        QSize imageSize = image.size();
        imageSize.scale(widgetSize, Qt::KeepAspectRatio);
        QRect targetRect(QPoint(0, 0), imageSize);
        targetRect.moveCenter(rect().center());
        painter.drawImage(targetRect, image);

        // Якщо є визначені сектори, малюємо їх
        if (numberOfSectors > 0) {
            // Крок кута для кожного сектору (від 0 до 90 градусів)
            double angleStep = 90.0 / numberOfSectors;

            // Центр у правому нижньому куті
            QPoint center(targetRect.bottomRight());

            // Корекція масштабування для довжини ліній
            int lineLength = std::min(700, 700); // Довжина лінії до краю зображення

            for (int i = 0; i < numberOfSectors; ++i) {
                double angle = i * angleStep;

                // Обчислюємо кут в радіанах для поточного сектору
                double radians = qDegreesToRadians(angle);

                // Корекція напрямку малювання (враховуємо що центр внизу)
                QPointF endPoint = center + QPointF(-std::cos(radians) * lineLength, -std::sin(radians) * lineLength);

                // Малюємо секторну лінію червоного кольору
                painter.setPen(QPen(Qt::red, 2));
                painter.drawLine(center, endPoint.toPoint());
            }
        }

        // Малюємо червону рамку вибору, якщо вона є
        if (!selectionRect.isNull()) {
            painter.setPen(Qt::red);
            painter.drawRect(selectionRect);
        }
    }
}


void ImageCropWidget::mousePressEvent(QMouseEvent* event)
{
    if (!image.isNull() && event->button() == Qt::LeftButton) {
        startPoint = event->pos();
        selecting = true;
    }
}

void ImageCropWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (selecting && event->button() == Qt::LeftButton) {
        selecting = false;

        // Визначити прямокутник вибраної області
        QPoint endPoint = event->pos();
        selectionRect = QRect(startPoint, endPoint).normalized();

        // Обробка вибору області в межах відображеного зображення
        QSize widgetSize = this->size();
        QSize imageSize = image.size();
        imageSize.scale(widgetSize, Qt::KeepAspectRatio);
        QRect targetRect(QPoint(0, 0), imageSize);
        targetRect.moveCenter(rect().center());

        selectionRect = selectionRect.intersected(targetRect);  // Вибірка лише в межах зображення
        update();
    }
}

void ImageCropWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (selecting && !image.isNull()) {
        QPoint endPoint = event->pos();
        selectionRect = QRect(startPoint, endPoint).normalized();
        qDebug() << "StartPoint" << startPoint;
        qDebug() << "EndPoint: " << endPoint;
        update();
    }
}