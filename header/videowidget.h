#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <QPoint>

class VideoWidget : public QLabel
{
    Q_OBJECT
public:
    VideoWidget(QWidget* parent = nullptr);
    ~VideoWidget();

    void imageClear();
    void initParams(int width,int height);

protected:
    void paintEvent(QPaintEvent* e)override;
public slots:
    void getMat(const cv::Mat& mat);

public:
    QImage img;
    int width = 0, height = 0;
    QPoint drawPos;
private:
    uchar* buf = nullptr;
    std::mutex mutex;
    bool refresh = false; //标识当前是否刷新完成
};

#endif // VIDEOWIDGET_H
