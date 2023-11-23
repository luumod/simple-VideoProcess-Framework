#ifndef VIDEOCAPWORK_H
#define VIDEOCAPWORK_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QMutex>
#include <atomic>

class VideoCapWork:public QObject
{
    Q_OBJECT
public:
    static VideoCapWork* videoCapWork;
    static VideoCapWork* getInstance();
    ~VideoCapWork();
private:
    VideoCapWork();

public:
    bool startExportVideo(const std::string& fileName);
    bool stopExportVideo();

    bool openVideo(const std::string& name);

    bool cutVideo(const std::string& name);

    void captureMat(const cv::Mat& mat);
    cv::Mat getCaptureMat();
public slots:
    void captureVideoFrame();
signals:
    void havingVideoFrame(const cv::Mat& mat);
    void havingVideoFrameDst(const cv::Mat& mat);
    void sendFrameToProcessTask(const cv::Mat& mat);
    void cutRefresh();
public:
    cv::VideoCapture videoCap;
    cv::VideoWriter videoWriter;
    double fps = 0; //视频帧率
    double fps_count = 0; //视频帧数
    int width = 0, height = 0;

    bool isStop = false; //视频停止：优先级最高
    bool isPause = false;//视频暂停
    bool isWriter = false;//视频录制
    cv::Mat captureMat_;

    QMutex mutex;
    std::atomic<bool> cutChangeFinished = false;//切换视频完成，则更新刷新标记
    std::atomic<bool> needRefresh = false;      //标记需要刷新
    std::atomic<int> countRefresh = 0;          //刷新次数，需要刷新两次
};

#endif // VIDEOCAPWORK_H
