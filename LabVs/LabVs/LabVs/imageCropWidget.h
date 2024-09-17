#pragma once
#include <qwidget.h>
class ImageCropWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageCropWidget(QWidget* parent = nullptr);

    void setImage(const QImage& image); // Завантаження зображення для відображення
    QImage getCroppedImage() const;     // Отримання обрізаного зображення
    void clearImage();  // Додайте метод для очищення зображення
    void setSectors(int sectors);
protected:
    void paintEvent(QPaintEvent* event) override;

    // Відображення зображення
    void mousePressEvent(QMouseEvent* event) override;      // Початок вибору області кропу
    void mouseMoveEvent(QMouseEvent* event) override;       // Оновлення області кропу під час руху миші
    void mouseReleaseEvent(QMouseEvent* event) override;    // Завершення вибору області

private:
    QImage currentImage;  // Зображення, яке відображається
    QRect selectionRect;
    QPoint startPoint;    // Початкова точка для кропу
    bool selecting;
    QImage image;
    int numberOfSectors = 0;



signals:
};

