#ifndef OPENGLITEM_H
#define OPENGLITEM_H

#include "Render.h"

#include <queue>
#include <atomic>
#include <thread>

#include <structedLightCamera.h>

#include <opencv2/cudafilters.hpp>

#include <QQuickItem>
#include <QQuickWindow>
#include <QObject>
#include <QTime>
#include <QBasicTimer>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QPoint>
#include <QVector3D>

//@brief 重建帧
struct DisplayFrame {
    /**
     * @brief           重建帧构造函数
     * 
     * @param depthImg_ 输入，深度图
     * @param colorImg_ 输入，纹理图
    */
    DisplayFrame(const cv::cuda::GpuMat& depthImg_, const cv::cuda::GpuMat& colorImg_) {
        depthImg = depthImg_;
        colorImg = colorImg_;
    }
    //深度图
    cv::cuda::GpuMat depthImg;
    //纹理图
    cv::cuda::GpuMat colorImg;
};

/** @brief Eigen标定信息
 * 
 *  @note 深度相机与左相机重合
 */
struct CaliInfoEigen {
  //深度相机内参
  Eigen::Matrix3f intrinsicD;
  //深度相机内参逆矩阵
  Eigen::Matrix3f intrinsicInvD;
  //右相机
  Eigen::Matrix3f intrinsicR;
  //彩色相机内参
  Eigen::Matrix3f intrinsicC;
  //深度相机到右相机旋转矩阵
  Eigen::Matrix3f rotateDToR;
  //深度相机到右相机平移矩阵
  Eigen::Vector3f translateDToR;
  //深度相机到彩色相机旋转矩阵
  Eigen::Matrix3f rotateDToC;
  //深度相机到彩色相机平移矩阵
  Eigen::Vector3f translateDToC;
  //深度相机八参数相位-高度映射矩阵
  Eigen::Vector<float, 8> coefficientD;
  //深度相机投影矩阵
  Eigen::Matrix4f projectD;
  //右相机投影矩阵
  Eigen::Matrix4f projectR;
};

//@brief 用于显示点云的控件
class OpenGLItem : public QQuickItem{
    Q_OBJECT
    /**
     * @brief 帧率属性
     */
    Q_PROPERTY(int fps READ getFps NOTIFY fpsChanged)
public:
    OpenGLItem();
    ~OpenGLItem();
    /**
     * @brief           获取帧率
     * 
     * @return          帧率
     */
    int getFps();
    /**
     * @brief           更新四元数位姿
     * 
     * @param moveX     X方向移动量
     * @param moveY     Y方向移动量
     * 
     * @return
     */
    Q_INVOKABLE void updateQuaternion(const float moveX, const float moveY);
    /**
     * @brief           更新相机位姿
     * 
     * @param press_A   输入，是否按键A按下
     * @param press_D   输入，是否按键D按下
     * @param press_W   输入，是否按键W按下
     * @param press_S   输入，是否按键S按下
     * 
     * @return 
     */
    Q_INVOKABLE void updateCameraPos(const bool press_A,const bool press_D, const bool press_W, const bool press_S);
    /**
     * @brief           更新视角
     * 
     * @param fovy      输入，视角值
     * 
     * @return  
     */
    Q_INVOKABLE void updateFovy(const float fovy);
    /**
     * @brief           重置相机
     * 
     * @return 
     */
    Q_INVOKABLE void resetCamera();
    /**
     * @brief           二维图片拉近
     * 
     * @return 
     */
    Q_INVOKABLE void dragNearst();
    /**
     * @brief           二维图片拉远
     * @return 
     */
    Q_INVOKABLE void dragFar();
    /**
     * @brief           更改场景
     * 
     * @param sceneType 输入，场景代号
     * 
     * @return 
     */
    Q_INVOKABLE void changeScene(const int sceneType);
    /**
     * @brief           设置视图方向
     * 
     * @param isUpper   输入，是否为俯视图
     * @param isFront   输入，是否为前视图
     * @param isLeft    输入，是否为左视图
     * 
     * @return 
     */
    Q_INVOKABLE void setViewPort(const bool isUpper, const bool isFront, const bool isLeft);
    /**
     * @brief           开始拉流
     * 
     * @return 
     */
    Q_INVOKABLE void start();
    /**
     * @brief           暂停拉流
     * 
     * @return 
     */
    Q_INVOKABLE void pause();
    /**
     * @brief           连接相机
     * 
     * @return          True：成功，false：失败
    */
    Q_INVOKABLE bool connectCamera();
    /**
     * @brief           设置显示状态
     * 
     * @param state     输入，状态代号
     * 
     * @return 
     */
    Q_INVOKABLE void setDisplayState(const int state);
    /**
     * @brief               更新放大系数
     * 
     * @param magCofficient 输入，放大系数
     * 
     * @return          
     */
    Q_INVOKABLE void updateMagCofficient(const float magCofficient);
    /**
     * @brief               设置显示模式
     * 
     * @param mode          输入，模式代号
     * 
     * @return 
     */
    Q_INVOKABLE void setDisplayMode(const int mode);
signals:
    /**
     * @brief 帧率改变信号
    */
    void fpsChanged();
protected:
    /**
     * @brief 状态同步
     */
    void sync();
    /**
     * @brief 释放资源
     */
    void cleanUp();
    /**
     * @brief       定时触发函数
     * 
     * @param event 输入，定时事件
    */
    void timerEvent(QTimerEvent *event);
private:
    //移动灵敏度
    const float moveSensity;
    //旋转灵敏度
    const float rotateSensity;
    //放大缩小灵敏度
    const float nearFarSensity;
    /** @brief 生产数据图片 **/
    void creatImg();
    /** @brief 开始线程入口函数 **/
    void startRun();
    /** @brief 从标定信息中获取Eigen标定矩阵信息 **/
    void initCaliEigen(const sl::tool::Info& info);
    //视角
    float fovy;
    //前向量
    QVector3D front;
    //上向量
    QVector3D upper;
    //右向量
    QVector3D right;
    //相机位置
    QVector3D cameraPos;
    //view矩阵
    QMatrix4x4 viewMat;
    //旋转轴
    QVector3D rotateAxis;
    //旋转四元数
    QQuaternion rotateQuaternion;
    //上一次渲染事件戳
    QTime lastTime;
    //帧率
    int fps;
    //定时器
    QBasicTimer timer;
    //OpenGL渲染器
    Render* render;
    //结构光相机
    sl::device::CameraControl* camera;
    //标定参数工具
    sl::tool::MatrixsInfo* caliInfo;
    //Eigen矩阵标定信息
    CaliInfoEigen caliEigen;
    //程序运行状态
    std::atomic_bool isStop;
    //待重建帧
    std::queue<sl::device::RestructedFrame> creatorQueue;
    //生产者线程
    std::thread creatorThread;
    //开始线程
    std::thread startThread;
    //是否需要刷新 
    std::atomic_bool isUpdateWindow;
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
    //上一帧所需左相机图像
    std::vector<cv::cuda::GpuMat> imgsOperateDevL;
    //上一帧所需右相机图像
    std::vector<cv::cuda::GpuMat> imgsOperateDevR;
    //上一帧所需彩色相机图像
    std::vector<cv::cuda::GpuMat> imgsOperateDevC;
    //上一帧所需彩色相机图像彩色
    std::vector<cv::cuda::GpuMat> imgsOperateDevCColor;
    //左相机参考平面图片
    cv::Mat leftRefImg;
    //左相机在右相机中的极线方程系数A
    cv::cuda::GpuMat epilinesA;
    //左相机在右相机中的极线方程系数B
    cv::cuda::GpuMat epilinesB;
    //左相机在右相机中的极线方程系数C
    cv::cuda::GpuMat epilinesC;
};

#endif // OPENGLITEM_H
