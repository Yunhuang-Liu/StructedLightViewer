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

//@brief �ؽ�֡
struct DisplayFrame {
    /**
     * @brief           �ؽ�֡���캯��
     * 
     * @param depthImg_ ���룬���ͼ
     * @param colorImg_ ���룬����ͼ
    */
    DisplayFrame(const cv::cuda::GpuMat& depthImg_, const cv::cuda::GpuMat& colorImg_) {
        depthImg = depthImg_;
        colorImg = colorImg_;
    }
    //���ͼ
    cv::cuda::GpuMat depthImg;
    //����ͼ
    cv::cuda::GpuMat colorImg;
};

/** @brief Eigen�궨��Ϣ
 * 
 *  @note ��������������غ�
 */
struct CaliInfoEigen {
  //�������ڲ�
  Eigen::Matrix3f intrinsicD;
  //�������ڲ������
  Eigen::Matrix3f intrinsicInvD;
  //�����
  Eigen::Matrix3f intrinsicR;
  //��ɫ����ڲ�
  Eigen::Matrix3f intrinsicC;
  //���������������ת����
  Eigen::Matrix3f rotateDToR;
  //�������������ƽ�ƾ���
  Eigen::Vector3f translateDToR;
  //����������ɫ�����ת����
  Eigen::Matrix3f rotateDToC;
  //����������ɫ���ƽ�ƾ���
  Eigen::Vector3f translateDToC;
  //�������˲�����λ-�߶�ӳ�����
  Eigen::Vector<float, 8> coefficientD;
  //������ͶӰ����
  Eigen::Matrix4f projectD;
  //�����ͶӰ����
  Eigen::Matrix4f projectR;
};

//@brief ������ʾ���ƵĿؼ�
class OpenGLItem : public QQuickItem{
    Q_OBJECT
    /**
     * @brief ֡������
     */
    Q_PROPERTY(int fps READ getFps NOTIFY fpsChanged)
public:
    OpenGLItem();
    ~OpenGLItem();
    /**
     * @brief           ��ȡ֡��
     * 
     * @return          ֡��
     */
    int getFps();
    /**
     * @brief           ������Ԫ��λ��
     * 
     * @param moveX     X�����ƶ���
     * @param moveY     Y�����ƶ���
     * 
     * @return
     */
    Q_INVOKABLE void updateQuaternion(const float moveX, const float moveY);
    /**
     * @brief           �������λ��
     * 
     * @param press_A   ���룬�Ƿ񰴼�A����
     * @param press_D   ���룬�Ƿ񰴼�D����
     * @param press_W   ���룬�Ƿ񰴼�W����
     * @param press_S   ���룬�Ƿ񰴼�S����
     * 
     * @return 
     */
    Q_INVOKABLE void updateCameraPos(const bool press_A,const bool press_D, const bool press_W, const bool press_S);
    /**
     * @brief           �����ӽ�
     * 
     * @param fovy      ���룬�ӽ�ֵ
     * 
     * @return  
     */
    Q_INVOKABLE void updateFovy(const float fovy);
    /**
     * @brief           �������
     * 
     * @return 
     */
    Q_INVOKABLE void resetCamera();
    /**
     * @brief           ��άͼƬ����
     * 
     * @return 
     */
    Q_INVOKABLE void dragNearst();
    /**
     * @brief           ��άͼƬ��Զ
     * @return 
     */
    Q_INVOKABLE void dragFar();
    /**
     * @brief           ���ĳ���
     * 
     * @param sceneType ���룬��������
     * 
     * @return 
     */
    Q_INVOKABLE void changeScene(const int sceneType);
    /**
     * @brief           ������ͼ����
     * 
     * @param isUpper   ���룬�Ƿ�Ϊ����ͼ
     * @param isFront   ���룬�Ƿ�Ϊǰ��ͼ
     * @param isLeft    ���룬�Ƿ�Ϊ����ͼ
     * 
     * @return 
     */
    Q_INVOKABLE void setViewPort(const bool isUpper, const bool isFront, const bool isLeft);
    /**
     * @brief           ��ʼ����
     * 
     * @return 
     */
    Q_INVOKABLE void start();
    /**
     * @brief           ��ͣ����
     * 
     * @return 
     */
    Q_INVOKABLE void pause();
    /**
     * @brief           �������
     * 
     * @return          True���ɹ���false��ʧ��
    */
    Q_INVOKABLE bool connectCamera();
    /**
     * @brief           ������ʾ״̬
     * 
     * @param state     ���룬״̬����
     * 
     * @return 
     */
    Q_INVOKABLE void setDisplayState(const int state);
    /**
     * @brief               ���·Ŵ�ϵ��
     * 
     * @param magCofficient ���룬�Ŵ�ϵ��
     * 
     * @return          
     */
    Q_INVOKABLE void updateMagCofficient(const float magCofficient);
    /**
     * @brief               ������ʾģʽ
     * 
     * @param mode          ���룬ģʽ����
     * 
     * @return 
     */
    Q_INVOKABLE void setDisplayMode(const int mode);
signals:
    /**
     * @brief ֡�ʸı��ź�
    */
    void fpsChanged();
protected:
    /**
     * @brief ״̬ͬ��
     */
    void sync();
    /**
     * @brief �ͷ���Դ
     */
    void cleanUp();
    /**
     * @brief       ��ʱ��������
     * 
     * @param event ���룬��ʱ�¼�
    */
    void timerEvent(QTimerEvent *event);
private:
    //�ƶ�������
    const float moveSensity;
    //��ת������
    const float rotateSensity;
    //�Ŵ���С������
    const float nearFarSensity;
    /** @brief ��������ͼƬ **/
    void creatImg();
    /** @brief ��ʼ�߳���ں��� **/
    void startRun();
    /** @brief �ӱ궨��Ϣ�л�ȡEigen�궨������Ϣ **/
    void initCaliEigen(const sl::tool::Info& info);
    //�ӽ�
    float fovy;
    //ǰ����
    QVector3D front;
    //������
    QVector3D upper;
    //������
    QVector3D right;
    //���λ��
    QVector3D cameraPos;
    //view����
    QMatrix4x4 viewMat;
    //��ת��
    QVector3D rotateAxis;
    //��ת��Ԫ��
    QQuaternion rotateQuaternion;
    //��һ����Ⱦ�¼���
    QTime lastTime;
    //֡��
    int fps;
    //��ʱ��
    QBasicTimer timer;
    //OpenGL��Ⱦ��
    Render* render;
    //�ṹ�����
    sl::device::CameraControl* camera;
    //�궨��������
    sl::tool::MatrixsInfo* caliInfo;
    //Eigen����궨��Ϣ
    CaliInfoEigen caliEigen;
    //��������״̬
    std::atomic_bool isStop;
    //���ؽ�֡
    std::queue<sl::device::RestructedFrame> creatorQueue;
    //�������߳�
    std::thread creatorThread;
    //��ʼ�߳�
    std::thread startThread;
    //�Ƿ���Ҫˢ�� 
    std::atomic_bool isUpdateWindow;
    //��С���
    int minDepth;
    //������
    int maxDepth;
    //�Ҷ�����ع�ʱ��
    int grayExposureTime;
    //��ɫ����ع�ʱ��
    int colorExposureTime;
    //�Ѳɼ�֡��
    int frameCaptured;
    //���ؽ�֡��
    int frameRestructed;
    //��һ֡���������ͼ��
    std::vector<cv::cuda::GpuMat> imgsOperateDevL;
    //��һ֡���������ͼ��
    std::vector<cv::cuda::GpuMat> imgsOperateDevR;
    //��һ֡�����ɫ���ͼ��
    std::vector<cv::cuda::GpuMat> imgsOperateDevC;
    //��һ֡�����ɫ���ͼ���ɫ
    std::vector<cv::cuda::GpuMat> imgsOperateDevCColor;
    //������ο�ƽ��ͼƬ
    cv::Mat leftRefImg;
    //�������������еļ��߷���ϵ��A
    cv::cuda::GpuMat epilinesA;
    //�������������еļ��߷���ϵ��B
    cv::cuda::GpuMat epilinesB;
    //�������������еļ��߷���ϵ��C
    cv::cuda::GpuMat epilinesC;
};

#endif // OPENGLITEM_H
