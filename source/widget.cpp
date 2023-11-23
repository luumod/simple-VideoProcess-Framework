#include "header/widget.h"
#include "header/videocapwork.h"
#include "header/framefilter.h"
#include "header/videowidget.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QPainter>
#include <QThread>
#include <QPushButton>
#include <QLabel>
#include <QCloseEvent>
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    work = VideoCapWork::getInstance();
    work_thread = new QThread(this);
    work->moveToThread(work_thread);
    work_thread->setStackSize(1024 * 1024 * 512);
    work_thread->start();

    //要开始多线程，必须要使得 线程内的对象 作为槽函数的对象
    connect(this, &MainWindow::intoWorkThread, work, &VideoCapWork::captureVideoFrame); //进入线程
    connect(work, &VideoCapWork::havingVideoFrame, ui->gl_wid1, &VideoWidget::getMat); //原始帧
    connect(work, &VideoCapWork::havingVideoFrameDst, ui->gl_wid2, &VideoWidget::getMat); //处理帧
    
    //某些按钮在视频打开前无法按下
    ui->video_save_btn1->setDisabled(true);
    ui->video_saveMat->setDisabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;

    work_thread->quit();
    work_thread->wait();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    work->isStop = true;
}

void MainWindow::resizeEvent(QResizeEvent* ev)
{
    updateImage();
}

void MainWindow::updateImage()
{
    work = VideoCapWork::getInstance();
    if (work->videoCap.isOpened()) {
        //1. 清除原始内容
        //2. 更新窗口
        ui->gl_wid1->imageClear();
        ui->gl_wid2->imageClear();
        ui->gl_wid1->setStyleSheet("background-color: white;");
        ui->gl_wid2->setStyleSheet("background-color: white;");
    }
}

void MainWindow::setPause()
{
    work->isPause = true;
    ui->video_play->setIcon(QIcon(":/images/kaishi.png"));
}

void MainWindow::setBegin()
{
    work->isPause = false;
    ui->video_play->setIcon(QIcon(":/images/zanting.png"));
}

void MainWindow::on_blur_slider_sliderMoved(int value)
{
    auto filter = frameFilter::getInstance();
    filter->clearTask();
    filter->addTask(Task{ Type::Blur,{value} });
}

void MainWindow::on_video_open_clicked()
{
    //切换视频
    setPause();
    QString name = QFileDialog::getOpenFileName(nullptr, "切换视频文件", "../videos/", "MP4 Files (*.mp4);; AVI Files (*.avi)");
    setBegin();
    if (name.isNull()) {
        return;
    }
    if (!work->cutVideo(name.toStdString())) {
        return; //视频打开失败
    }
    //ok

    //封装成函数
    ui->video_save_btn1->setDisabled(false);
    ui->video_saveMat->setDisabled(false);

    //清除原始图片
    ui->gl_wid1->imageClear();
    ui->gl_wid2->imageClear();

    setBegin();
    emit intoWorkThread();
}

void MainWindow::on_video_play_clicked()
{
    if (!work->videoCap.isOpened()) {
        //没有视频，则打开视频
        on_video_open_clicked();
        return;
    }
    //处理暂停与播放
    if (work->isPause) {
        setBegin();
    }
    else {
        setPause(); 
    }
}

//操作行为
#if 1
void MainWindow::on_video_bright_slider1_sliderMoved(int value)
{
    auto filter = frameFilter::getInstance();
    filter->clearTask();
    filter->addTask(Task{ Type::Bright,{value} });
    if (sender() == ui->video_bright_slider1) {
        ui->video_bright_slider2->setValue(ui->video_bright_slider2->minimum());
    }
}

void MainWindow::on_video_bright_slider2_sliderMoved(int value)
{
    value = -value;
    on_video_bright_slider1_sliderMoved(value);
    ui->video_bright_slider1->setValue(ui->video_bright_slider1->minimum());
}

void MainWindow::on_video_bright_edit_returnPressed()
{
    int value = ui->video_bright_edit->text().toInt(); //0-> fails
    on_video_bright_slider1_sliderMoved(value);
    if (sender() == ui->video_bright_edit) {
        ui->video_bright_slider1->setValue(ui->video_bright_slider1->minimum());
        ui->video_bright_slider2->setValue(ui->video_bright_slider2->minimum());
    }
}
#endif

void MainWindow::on_video_save_btn1_clicked()
{
    if (work->isWriter) {
        //点击保存
        work->stopExportVideo();
        ui->video_save_btn1->setText("开始");
        return;
    }
    //从当前位置保存视频
    setPause();
    QString name = QFileDialog::getSaveFileName(nullptr, "保存视频", "../videos/saves/", "MP4 Files (*.mp4)");
    setBegin();
    if (name.isNull()) {
        return;
    }

    if (!work->startExportVideo(name.toStdString())) {
        return; //打开视频解码器失败
    }
    ui->video_save_btn1->setText("停止导出");
}

void MainWindow::on_video_saveMat_clicked()
{
    //保存当前帧
    work->isPause = true;
    QString name = QFileDialog::getSaveFileName(nullptr, "保存图片", "../videos/saves/", "图片文件 (*.png)");
    work->isPause = false;
    if (name.isNull()){
        return;
    }
    cv::Mat msave = work->getCaptureMat();
    cv::imwrite(name.toStdString(), msave);
}

void MainWindow::on_video_setArea_hide_clicked(){
    QPropertyAnimation* animation = new QPropertyAnimation(ui->widget_setArea, "pos", this);

    if (ui->widget_setArea->isVisible()) {
        //隐藏
        // 设置动画的目标位置
        animation->setEndValue(QPoint(ui->widget_setArea->x(), ui->widget_setArea->y() + ui->widget_setArea->height()));
    }
    else {
        //显示
        animation->setEndValue(QPoint(ui->widget_setArea->x(), ui->widget_setArea->y() - ui->widget_setArea->height()));
    }

    // 设置动画的持续时间
    animation->setDuration(300); // 1000毫秒 = 1秒

    // 设置动画的缓和曲线，这里使用线性
    animation->setEasingCurve(QEasingCurve::InCurve);

    // 连接动画结束时的信号到槽函数，用于隐藏Widget
    connect(animation, &QPropertyAnimation::finished, ui->widget_setArea, [=]() {
        ui->widget_setArea->setVisible(!ui->widget_setArea->isVisible());
        updateImage();  
        animation->deleteLater();
        });

    // 启动动画
    animation->start();
}