#ifndef OPENGLITEM_H
#define OPENGLITEM_H

#include <QQuickItem>
#include <QQuickWindow>
#include <QObject>
#include <QTime>
#include <QBasicTimer>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QPoint>
#include <QVector3D>
#include <queue>
#include <atomic>
#include <StructedLightCamera.h>
#include <thread>
#include "Render.h"

//重建帧
struct DisplayFrame {
    DisplayFrame(const cv::cuda::GpuMat& depthImg_, const cv::cuda::GpuMat& colorImg_) {
        depthImg_.copyTo(depthImg);
        colorImg_.copyTo(colorImg);
    }
    cv::cuda::GpuMat depthImg;
    cv::cuda::GpuMat colorImg;
};

class OpenGLItem : public QQuickItem{
    Q_OBJECT
    Q_PROPERTY(int fps READ getFps NOTIFY fpsChanged)
public:
    OpenGLItem();
    ~OpenGLItem();
    int getFps();
    Q_INVOKABLE void updateQuaternion(const float moveX, const float moveY);
    Q_INVOKABLE void updateCameraPos(const bool press_A,const bool press_D, const bool press_W, const bool press_S);
    Q_INVOKABLE void updateFovy(const float fovy);
    Q_INVOKABLE void resetCamera();
    Q_INVOKABLE void dragNearst();
    Q_INVOKABLE void dragFar();
    Q_INVOKABLE void changeScene(const int sceneType);
    Q_INVOKABLE void setViewPort(const bool isUpper, const bool isFront, const bool isLeft);
    Q_INVOKABLE void start();
    Q_INVOKABLE void pause();
    Q_INVOKABLE bool connectCamera();
    Q_INVOKABLE void setDisplayState(const int state);
    Q_INVOKABLE void updateMagCofficient(const float magCofficient);
    Q_INVOKABLE void setDisplayMode(const int mode);
signals:
    void fpsChanged();
protected:
    void sync();
    void cleanUp();
    void timerEvent(QTimerEvent *event);
private:
    float fovy;
    const float moveSensity;
    const float rotateSensity;
    const float nearFarSensity;
    QVector3D front;
    QVector3D upper;
    QVector3D right;
    QVector3D cameraPos;
    QMatrix4x4 viewMat;
    QVector3D rotateAxis;
    QQuaternion rotateQuaternion;
    QTime lastTime;
    int fps;
    QBasicTimer timer;
    Render* render;

    //结构光相机
    CameraControl* camera;
    //标定参数工具
    MatrixsInfo* caliInfo;
    //程序运行状态
    std::atomic_bool isStop;
    //待重建帧
    std::queue<RestructedFrame> creatorQueue;
    //生产者线程
    std::thread creatorThread;
    /** \生产数据图片 **/
    void creatImg();
    //开始线程
    std::thread startThread;
    //是否需要刷新 
    std::atomic_bool isUpdateWindow;
    /** /开始线程入口函数 **/
    void startRun();
    /** \GPU极线校正 **/
    void remapImg(const cv::Mat& src, const cv::cuda::GpuMat& remap_x, const cv::cuda::GpuMat& remap_y, cv::cuda::GpuMat& outImg, cv::cuda::Stream stream, const bool convertToGray = true);
    //最小深度
    int minDepth;
    //最大深度
    int maxDepth;
    //灰度相机曝光时间
    int grayExposureTime;
    //彩色相机曝光时间
    int colorExposureTime;
    //已采集帧数
    int frameCaptured;
    //已重建帧数
    int frameRestructed;
    //上一帧所需三幅图像
    std::vector<cv::Mat> imgsLast;
};

#endif // OPENGLITEM_H
