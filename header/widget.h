#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QMutex>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QThread;
class VideoCapWork;
class QLabel;
class QPushButton;
class AdapterToImage;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent* ev)override;
protected:
    /**
     * @brief 更新图片尺寸或者内容
     */
    void updateImage();

    void setPause();
    void setBegin();
public slots:
    void on_blur_slider_sliderMoved(int value);
    /**
     * @brief 使用按钮打开视频
     */
    void on_video_open_clicked();

    /**
     * @brief 点击开始或者暂停的按钮
     * 
     */
    void on_video_play_clicked();

    void on_video_bright_slider1_sliderMoved(int value);
    void on_video_bright_slider2_sliderMoved(int value);
    void on_video_bright_edit_returnPressed();
    void on_video_save_btn1_clicked();
    void on_video_saveMat_clicked();
    
    void on_video_setArea_hide_clicked();
signals:
    void intoWorkThread();


private:
    Ui::MainWindow *ui;

    VideoCapWork* work = nullptr;
    QThread* work_thread = nullptr;

    cv::Mat ori_mat;
    cv::Mat hand_mat;

    QMutex mutex;
};
#endif // MAINWINDOW_H
