﻿#include "header/videowidget.h"
#include "header/videocapwork.h"
#include <QPainter>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QThread>
#include <QOpenGLContext>
#include <QPaintEvent>
#include <QElapsedTimer>
#include "gl/GL.h"
#include "gl/GLU.h"
VideoWidget::VideoWidget(QWidget* parent) : QLabel(parent){

}

VideoWidget::~VideoWidget()
{
    if (buf){
        delete[] buf;
        buf = nullptr;
    }
}

void VideoWidget::imageClear()
{
    if (buf){
        //原图像清空
        delete[] buf;
        buf = nullptr;
        img = QImage();
    }

}

void VideoWidget::initParams(int width,int height)
{
    //初始化参数
    this->width = width;
    this->height=  height;
    this->width = (width + 3) & ~3;
    this->height = (height + 3) & ~3;
    this->drawPos = QPoint(this->size().width()/2 - width /2 ,this->size().height()/2 - height/2);
}

void VideoWidget::getMat(const cv::Mat &mat)
{
    if (img.isNull()){
        QImage temp(mat.data,mat.cols,mat.rows,mat.step,QImage::Format_BGR888);
        QImage ttemp = temp.scaled(this->size(),Qt::KeepAspectRatio);
        //尺寸参数初始化
        initParams(ttemp.size().width(),ttemp.size().height());
        if (!buf){
            buf = new uchar[width*height*3];
            img=QImage(buf,width,height,QImage::Format_BGR888);
        }
    }

    cv::Mat dst;
    cv::resize(mat,dst,cv::Size(img.width(),img.height())); //缩放
    memcpy(img.bits(),dst.data,dst.rows*dst.cols*dst.elemSize());
    update();
}

void VideoWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter;
    painter.begin(this);
    //正常绘制
    painter.drawImage(drawPos ,img); //绘制目标操作后图片
    painter.end();
}