#ifndef RENDER_H
#define RENDER_H

#include <QQuickWindow>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QQuaternion>
#include <QImage>
#include <QObject>
#include <Eigen/Core>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cuda_gl_interop.h>

extern "C" void renderCloud_CUDA(const cv::cuda::GpuMat & depthImg, const cv::cuda::GpuMat & colorImg, const Eigen::Matrix3f & intrinsic_inc,
    float* ptrCloud);

class Render : public QObject, protected QOpenGLFunctions{
    Q_OBJECT
public:
    enum SkyBox{
        lightNight = 0,
        darkNight
    };
    enum DisplayState {
        Plain = 0,
        threeDimensional
    };
    enum DisplayMode{
        point = 0,
        normal,
        mesh
    };
    Render();
    ~Render();
    void initializeGL();
    void resizeGL(const int w,const int h);
    void repaintGL();
    void setWindow(QQuickWindow* window);

    void setCameraPos(const QVector3D& cameraPos);
    void setView(const QQuaternion& quaternion);
    void setView(const QMatrix4x4& view);
    void setFovy(const float fovy);
    void changeScence(const SkyBox skyBoxType);
    void renderCloud(const cv::cuda::GpuMat& depthImg, const cv::cuda::GpuMat& colorImg, const Eigen::Matrix3f& intrinsic_inv);
    void setR1Inv(const cv::Mat& R1Inv);
    void setMeshMode(const DisplayMode displayMode);
    void setDisplayState(const DisplayState displayState);
    void setMagCofficient(const float magCofficient);

    bool isGetWorldPoint;
    float pixel_x;
    float pixel_y;
    QVector3D worldPoint;
    float deviceRadio;
protected:

private:
    //窗口
    QQuickWindow* window;
    //天空盒
    const int skyBoxWidth = 2048;
    const int skyBoxHeight = 2048;
    QOpenGLTexture* skyBoxArrayLightStyle;
    QOpenGLTexture* skyBoxArrayDarkStyle;
    QOpenGLTexture* currentTexture;
    QOpenGLShaderProgram skyBoxShaderProgram;
    QOpenGLVertexArrayObject skyBoxVao;
    QOpenGLBuffer skyBoxVbo;
    void drawSkyBox();
    void initializeSkyBox();
    //坐标系
    QOpenGLShaderProgram coodinateShaderProgram;
    QOpenGLVertexArrayObject coodinateVao;
    QOpenGLVertexArrayObject coodinateTriangleVao;
    QOpenGLBuffer coodinateVbo;
    QOpenGLBuffer triangleVbo;
    void drawCoodinate();
    void initializeCoodinate();
    //网格地面
    QOpenGLShaderProgram gridShaderProgram;
    QOpenGLVertexArrayObject gridVao;
    QOpenGLVertexArrayObject boldGridVao;
    QOpenGLBuffer gridVbo;
    QOpenGLBuffer boldGridVbo;
    void drawGrid();
    void initializeGrid();
    //相机位姿
    int windowWidth;
    int windowHeight;
    float fovy;
    QMatrix4x4 view;
    QMatrix4x4 project;
    QVector3D cameraPos;
    QQuaternion rotateQuaternion;
    //点云
    QMatrix4x4 model;
    size_t size;
    float* ptrGLResource;
    cudaGraphicsResource* m_vboResource;
    Eigen::Matrix3f intrinsic_inv;
    const int rows;
    const int cols;
    QOpenGLShaderProgram cloudProgram;
    QOpenGLVertexArrayObject cloudVao;
    int meshNum;
    QOpenGLBuffer meshEBO;
    GLuint cloudVbo;
    void drawCloud();
    void initializeCloud();

    std::mutex renderMutex;
    QMatrix4x4 R1Inv;

    QOpenGLTexture depthTexture;
    QOpenGLVertexArrayObject depthVao;
    QOpenGLBuffer depthBuffer;
    QOpenGLShaderProgram depthProgram;
    float magCofficient;
    cv::Mat colorTexture;
    void drawDepth();
    void initialDepth();

    DisplayState displayState;
    DisplayMode displayMode;
};

#endif // RENDER_H
