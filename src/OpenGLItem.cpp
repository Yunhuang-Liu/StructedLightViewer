#include "../include/OpenGLItem.h"

using namespace sl;
using namespace sl::device;
using namespace sl::tool;

OpenGLItem::OpenGLItem() : render(nullptr), fps(0), front(QVector3D(0,0,-1)), right(QVector3D(1,0,0)),upper(QVector3D(0,1,0)),
    cameraPos(QVector3D(0,0,-500)),moveSensity(5.f),rotateSensity(0.5f),fovy(80),nearFarSensity(5.f),caliInfo(nullptr),isStop(true),
    camera(nullptr),minDepth(1000),maxDepth(2000),grayExposureTime(4000),colorExposureTime(4000),isUpdateWindow(false), frameCaptured(0), frameRestructed(0){
    //timer.start(10,this);
    lastTime = QTime::currentTime();
    connect(this,&QQuickItem::windowChanged,this,[&](QQuickWindow* window){
        if(nullptr != window){
            connect(window,&QQuickWindow::beforeSynchronizing,this,&OpenGLItem::sync,Qt::DirectConnection);
            connect(window,&QQuickWindow::sceneGraphInvalidated,this,&OpenGLItem::cleanUp,Qt::DirectConnection);
            window->setColor(Qt::white);
        }
    });
    imgsOperateDevL.resize(4);
    imgsOperateDevR.resize(4);
    imgsOperateDevC.resize(4);
    imgsOperateDevCColor.resize(4);
}

OpenGLItem::~OpenGLItem(){

}

void OpenGLItem::sync(){
    if(nullptr == render){
        render = new Render();
        render->initializeGL();
        render->resizeGL(window()->width(),window()->height());
        /*
        connect(window(),&QQuickWindow::beforeRendering,this,[&](){
            render->initializeGL();
            render->resizeGL(window()->width(),window()->height());
        },Qt::DirectConnection);
        */
        connect(window(),&QQuickWindow::beforeRenderPassRecording,this,[&](){
            render->repaintGL();
        },Qt::DirectConnection);

        connect(window(),&QQuickWindow::afterRendering,this,[&](){
            //计算fps
            /*
            static int frame = 0;
            frame++;
            if(lastTime.msecsTo(QTime::currentTime()) > 1000){
                fps = frame;
                emit fpsChanged();
                frame = 0;
                lastTime = QTime::currentTime();
            }
            */
        },Qt::DirectConnection);
        connect(window(),&QQuickWindow::widthChanged,this,[&](){
            render->resizeGL(window()->width(),window()->height());
        },Qt::DirectConnection);
        connect(window(),&QQuickWindow::heightChanged,this,[&](){
            render->resizeGL(window()->width(),window()->height());
        },Qt::DirectConnection);
    }
    render->setWindow(window());
    window()->update();
}

void OpenGLItem::cleanUp(){
    if(nullptr != render){
        delete render;
        render = nullptr;
    }
}

void OpenGLItem::timerEvent(QTimerEvent *event){
    event->accept();
    if (isUpdateWindow.load(std::memory_order_acquire)) {
        window()->update();
        isUpdateWindow.store(false, std::memory_order_release);
    }
}

int OpenGLItem::getFps(){
    return fps;
}

void OpenGLItem::updateQuaternion(const float moveX, const float moveY){
    if(nullptr != render){
        QVector3D rotateAxis_ = QVector3D(moveY, moveX, 0);
        QVector3D n = rotateAxis_.normalized();
        rotateAxis = (rotateAxis + n).normalized();
        rotateQuaternion = QQuaternion::fromAxisAndAngle(rotateAxis,rotateSensity) * rotateQuaternion;
        render->setView(rotateQuaternion);
        window()->update();
    }
}

void OpenGLItem::updateCameraPos(const bool press_A,const bool press_D, const bool press_W, const bool press_S){
    if(nullptr != render){
        if(press_A)
            cameraPos -= QVector3D::crossProduct(upper,front).normalized() * moveSensity;
        if(press_D)
            cameraPos += QVector3D::crossProduct(upper,front).normalized() * moveSensity;
        if(press_W)
            cameraPos -= QVector3D::crossProduct(right,front).normalized() * moveSensity;
        if(press_S)
            cameraPos += QVector3D::crossProduct(right,front).normalized() * moveSensity;
        render->setCameraPos(cameraPos);
        window()->update();
    }
}

void OpenGLItem::updateFovy(const float fovy_){
    if(nullptr != render){
        fovy = fovy_;
        render->setFovy(fovy);
        window()->update();
    }
}

void OpenGLItem::resetCamera(){
    fovy = 80;
    rotateAxis = QVector3D(0,0,0);
    rotateQuaternion = QQuaternion();
    cameraPos = QVector3D(0,0,-500);
    render->setCameraPos(cameraPos);
    render->setFovy(80);
    render->setView(rotateQuaternion);
    window()->update();
}

void OpenGLItem::dragNearst(){
    cameraPos -= front * nearFarSensity;
    render->setCameraPos(cameraPos);
    window()->update();
}

void OpenGLItem::dragFar(){
    cameraPos += front * nearFarSensity;
    render->setCameraPos(cameraPos);
    window()->update();
}

void OpenGLItem::changeScene(const int sceneType){
    if(nullptr != render){
        switch (sceneType) {
            case 0:{
                render->changeScence(Render::SkyBox::lightNight);
                break;
            }
            case 1:{
                render->changeScence(Render::SkyBox::darkNight);
                break;
            }
        }
    }
    window()->update();
}

void OpenGLItem::setViewPort(const bool isUpper, const bool isFront, const bool isLeft){
    fovy = 80;
    rotateAxis = QVector3D(0,0,0);
    rotateQuaternion = QQuaternion();
    QMatrix4x4 viewMat;
    if(isUpper){
        cameraPos = QVector3D(0,500,0);
        viewMat.lookAt(cameraPos,cameraPos+QVector3D(0,-1,0),QVector3D(0,0,-1));
    }
    if(isFront){
        cameraPos = QVector3D(0,0,500);
        viewMat.lookAt(cameraPos,cameraPos+QVector3D(0,0,-1),QVector3D(0,1,0));
    }
    if(isLeft){
        cameraPos = QVector3D(-500,0,0);
        viewMat.lookAt(cameraPos,cameraPos+QVector3D(1,0,0),QVector3D(0,1,0));
    }
    render->setCameraPos(cameraPos);
    render->setFovy(fovy);
    render->setView(viewMat);
    window()->update();
}

void OpenGLItem::start() {
    isStop.store(false, std::memory_order_release);
    startThread = std::thread(&OpenGLItem::startRun, this);
    //等待一段时间让重建线程先行就绪完毕
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    creatorThread = std::thread(&OpenGLItem::creatImg, this);
}

void OpenGLItem::pause(){
    camera->stopProject();
    isStop.store(true, std::memory_order_release);
    startThread.join();
    creatorThread.join();
    std::queue<RestructedFrame> emptyCreator;
    creatorQueue.swap(emptyCreator);
    isUpdateWindow.store(false, std::memory_order_release);
    frameCaptured = 0;
    frameRestructed = 0;
}

void OpenGLItem::creatImg() {
    camera->setCameraExposure(grayExposureTime, colorExposureTime);
    RestructedFrame frame;
    //连续投影有闪烁现象
    camera->project(true);
    while (!isStop.load(std::memory_order_acquire)) {
        camera->getOneFrameImgs(frame);
        creatorQueue.emplace(frame);
        /*
        if (frameCaptured == 0) {
            creatorQueue.emplace(frame);
        }
        else {
            std::vector<cv::Mat> firstLeft(2);
            std::vector<cv::Mat> firstRight(2);
            std::vector<cv::Mat> firstColor(2);
            std::vector<cv::Mat> secLeft(2);
            std::vector<cv::Mat> secRight(2);
            std::vector<cv::Mat> secColor(2);
            for (int i = 0; i < 2; ++i) {
                firstLeft[i] = frame.leftImgs[i];
                firstRight[i] = frame.rightImgs[i];
                firstColor[i] = frame.colorImgs[i];
                secLeft[i] = frame.leftImgs[i + 2];
                secRight[i] = frame.rightImgs[i + 2];
                secColor[i] = frame.colorImgs[i + 2];
            }
            creatorQueue.emplace(RestructedFrame(firstLeft,
                firstRight,
                firstColor));

            creatorQueue.emplace(RestructedFrame(secLeft,
                secRight,
                secColor));
        }
        */
        ++frameCaptured;
        
        std::vector<int> fpsCameras = camera->getFrameFps();
        std::cout << "camera fps:" << fpsCameras[0] << "\t" << fpsCameras[1] << "\t" << fpsCameras[2] << std::endl;
    }
}

void OpenGLItem::initCaliEigen(const sl::tool::Info& info) {
    cv::Mat M1, M2, M3, R, T, Rlc, Tlc, K1;
    info.M1.convertTo(M1, CV_32FC1);
    info.M2.convertTo(M2, CV_32FC1);
    info.M3.convertTo(M3, CV_32FC1);
    info.R.convertTo(R, CV_32FC1);
    info.T.convertTo(T, CV_32FC1);
    info.Rlc.convertTo(Rlc, CV_32FC1);
    info.Tlc.convertTo(Tlc, CV_32FC1);
    info.K1.convertTo(K1, CV_32FC1);
    cv::cv2eigen(M1, caliEigen.intrinsicD);
    cv::cv2eigen(M1.inv(), caliEigen.intrinsicInvD);
    cv::cv2eigen(M2, caliEigen.intrinsicR);
    cv::cv2eigen(M3, caliEigen.intrinsicC);
    cv::cv2eigen(R, caliEigen.rotateDToR);
    cv::cv2eigen(T, caliEigen.translateDToR);
    cv::cv2eigen(Rlc, caliEigen.rotateDToC);
    cv::cv2eigen(Tlc, caliEigen.translateDToC);
    cv::cv2eigen(K1, caliEigen.coefficientD);
    caliEigen.projectD << caliEigen.intrinsicD(0, 0), caliEigen.intrinsicD(0, 1), caliEigen.intrinsicD(0, 2), 0,
                          caliEigen.intrinsicD(1, 0), caliEigen.intrinsicD(1, 1), caliEigen.intrinsicD(1, 2), 0,
                          caliEigen.intrinsicD(2, 0), caliEigen.intrinsicD(2, 1), caliEigen.intrinsicD(2, 2), 0,
                          0, 0, 0, 1;
    Eigen::Matrix4f MR, TR;
    MR << caliEigen.intrinsicR(0, 0), caliEigen.intrinsicR(0, 1),caliEigen.intrinsicR(0, 2), 0,
          caliEigen.intrinsicR(1, 0), caliEigen.intrinsicR(1, 1),caliEigen.intrinsicR(1, 2), 0,
          caliEigen.intrinsicR(2, 0), caliEigen.intrinsicR(2, 1),caliEigen.intrinsicR(2, 2), 0,
          0, 0, 0, 1;
    TR << caliEigen.rotateDToR(0, 0), caliEigen.rotateDToR(0, 1), caliEigen.rotateDToR(0, 2), caliEigen.translateDToR(0, 0),
          caliEigen.rotateDToR(1, 0), caliEigen.rotateDToR(1, 1), caliEigen.rotateDToR(1, 2), caliEigen.translateDToR(1, 0),
          caliEigen.rotateDToR(2, 0), caliEigen.rotateDToR(2, 1), caliEigen.rotateDToR(2, 2), caliEigen.translateDToR(2, 0),
          0, 0, 0, 1;
    caliEigen.projectR = MR * TR;
}

void OpenGLItem::startRun() {
    const Info& info = caliInfo->getInfo();
    const cv::Size imgSize = cv::Size(info.S.at<double>(0,0), info.S.at<double>(1, 0));    
    std::unique_ptr<phaseSolver::FourStepRefPlainMaster_GPU> phaseSolveTool(new phaseSolver::FourStepRefPlainMaster_GPU(leftRefImg, true));
    phaseSolveTool->setCounterMode(false);
    std::unique_ptr<wrapCreator::WrapCreator_GPU> wrapCreateTool(new wrapCreator::WrapCreator_GPU());
    while (!isStop.load(std::memory_order_acquire)) {
      if (creatorQueue.size() >= 1) {
          auto start = std::chrono::steady_clock::now();
          RestructedFrame& frame = creatorQueue.front();
          // auto filter = cv::cuda::createGaussianFilter(CV_8UC1, CV_8UC1,
          // cv::Size(3,3), 0.8);
          std::vector<cv::cuda::Stream> streams(
              3, cv::cuda::Stream(cudaStreamNonBlocking));
          for (int i = 0; i < frame.leftImgs.size(); ++i) {
            imgsOperateDevL[i].upload(frame.leftImgs[i], streams[0]);
          }

          std::vector<cv::cuda::GpuMat> unwrapL;
          phaseSolveTool->changeSourceImg(imgsOperateDevL);
          phaseSolveTool->getUnwrapPhaseImg(unwrapL, streams[0]);

          cv::cuda::GpuMat depthImg;
          sl::tool::cudaFunc::phaseHeightMapEigCoe(
              unwrapL[0], caliEigen.intrinsicD, caliEigen.coefficientD, 1200,
              1600, depthImg, dim3(32, 8), streams[0]);

          for (int i = 0; i < frame.rightImgs.size(); ++i) {
            imgsOperateDevR[i].upload(frame.rightImgs[i], streams[1]);
          }

          cv::cuda::GpuMat wrapImgR, conditinImgR;
          wrapCreateTool->getWrapImg(imgsOperateDevR, wrapImgR, conditinImgR,
                                     false, streams[1]);

          for (int i = 0; i < frame.colorImgs.size(); ++i) {
            imgsOperateDevCColor[i].upload(frame.colorImgs[i], streams[2]);
            cv::cuda::cvtColor(imgsOperateDevCColor[i], imgsOperateDevCColor[i],
                             cv::COLOR_BayerRG2BGR, 3, streams[2]);
            cv::cuda::cvtColor(imgsOperateDevCColor[i], imgsOperateDevC[i],
                             cv::COLOR_BGR2GRAY, 1, streams[2]);
          }
          
          cv::cuda::GpuMat wrapImgC, conditionImgC;
          wrapCreateTool->getWrapImg(imgsOperateDevC, wrapImgC, conditionImgC,
                                   false, streams[2]);

          cv::cuda::GpuMat texture;
          sl::tool::cudaFunc::averageTexture(imgsOperateDevCColor, texture,
                                           dim3(32, 8), streams[2]);
          // if (frameRestructed == 0) {
          // filter->apply(imgsOperateDevL[i], imgsOperateDevL[i], streams[0]);

          // filter->apply(imgsOperateDevR[i], imgsOperateDevR[i], streams[1]);

          // filter->apply(imgsOperateDevC[i], imgsOperateDevC[i], streams[2]);
          //}
          /*
          else if(frameRestructed % 2 == 0){
              imgsOperateDevL[i + 2].upload(frame.leftImgs[i], streams[i]);
              imgsOperateDevR[i + 2].upload(frame.rightImgs[i], streams[i]);
              imgsOperateDevCColor[i + 2].upload(frame.colorImgs[i], streams[i]);
              cv::cuda::cvtColor(imgsOperateDevCColor[i + 2],
              imgsOperateDevCColor[i + 2], cv::COLOR_BayerRG2BGR, 0, streams[i]);
              cv::cuda::cvtColor(imgsOperateDevCColor[i + 2], imgsOperateDevC[i +
              2], cv::COLOR_BGR2GRAY, 0, streams[i]); filter->apply(imgsOperateDevL[i
              + 2], imgsOperateDevL[i + 2], streams[i]);
                filter->apply(imgsOperateDevR[i + 2], imgsOperateDevR[i + 2],
              streams[i]); filter->apply(imgsOperateDevC[i + 2], imgsOperateDevC[i +
              2], streams[i]);
            }
            else {
                imgsOperateDevL[i].upload(frame.leftImgs[i], streams[i]);
                imgsOperateDevR[i].upload(frame.rightImgs[i], streams[i]);
                imgsOperateDevCColor[i].upload(frame.colorImgs[i], streams[i]);
                cv::cuda::cvtColor(imgsOperateDevCColor[i], imgsOperateDevCColor[i],
                cv::COLOR_BayerRG2BGR, 0, streams[i]);
                cv::cuda::cvtColor(imgsOperateDevCColor[i], imgsOperateDevC[i],
              cv::COLOR_BGR2GRAY, 0, streams[i]); filter->apply(imgsOperateDevL[i],
              imgsOperateDevL[i], streams[i]); filter->apply(imgsOperateDevR[i],
              imgsOperateDevR[i], streams[i]); filter->apply(imgsOperateDevC[i],
              imgsOperateDevC[i], streams[i]);
              }
              */
            //}

          for (auto& stream : streams) 
              stream.waitForCompletion();

          cv::cuda::GpuMat depthRefine;
          sl::tool::cudaFunc::reverseMappingRefine(
              unwrapL[0], depthImg, wrapImgR, conditinImgR, wrapImgC,
              conditionImgC, caliEigen.intrinsicInvD, caliEigen.intrinsicR,
              caliEigen.intrinsicC, caliEigen.rotateDToR, caliEigen.translateDToR,
              caliEigen.rotateDToC, caliEigen.translateDToC, caliEigen.projectD,
              caliEigen.projectR, 0.6f, epilinesA, epilinesB, epilinesC,
              depthRefine);

          cv::cuda::GpuMat textureMapped;
          sl::tool::cudaFunc::reverseMappingTexture(
              depthRefine, texture, caliEigen.intrinsicInvD, caliEigen.intrinsicC,
              caliEigen.rotateDToC, caliEigen.translateDToC, textureMapped);
            cv::Mat test[10];
            unwrapL[0].download(test[0]);
            depthImg.download(test[1]);
            conditinImgR.download(test[2]);
            conditionImgC.download(test[3]);
            depthRefine.download(test[4]);
            texture.download(test[5]);
            textureMapped.download(test[6]);
            cv::imwrite("depth_0.tiff", test[4]);
            cv::imwrite("depth.tiff", test[1]);
            cv::imwrite("texture_0.png", test[6]);
            /*
            cv::imwrite("../../systemFile/outFile/depth/depth_" +
            std::to_string(frameRestructed) + ".tiff", test[0]);
            cv::imwrite("../../systemFile/outFile/color/color_" +
            std::to_string(frameRestructed) + ".bmp", test[1]);
            cv::imwrite("../../systemFile/outFile/img/L/wrap_" +
            std::to_string(frameRestructed) + ".tiff", test[2]);
            cv::imwrite("../../systemFile/outFile/img/L/condition_" +
            std::to_string(frameRestructed) + ".tiff", test[4]);
            cv::imwrite("../../systemFile/outFile/img/R/wrap_" +
            std::to_string(frameRestructed) + ".tiff", test[3]);
            cv::imwrite("../../systemFile/outFile/img/R/condition_" +
            std::to_string(frameRestructed) + ".tiff", test[5]);
            */

          render->renderCloud(depthRefine, textureMapped,
                              caliEigen.intrinsicInvD);

          isUpdateWindow.store(true, std::memory_order_release);

          creatorQueue.pop();
          //++frameRestructed;

          std::cout << "queue size :" << creatorQueue.size() << std::endl;

          auto end = std::chrono::steady_clock::now();
          auto time =std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * static_cast<float>(std::chrono::milliseconds::period::num) / std::chrono::milliseconds::period::den;
          fps = 1.f / time;
          emit fpsChanged();

          // std::cout << "time used :" << time << std::endl;
      }
    }
}

bool OpenGLItem::connectCamera(){
    try{
        //更完善的软件设计方式应当增加异常处理
        if (nullptr == caliInfo) {
            caliInfo = new MatrixsInfo(
                  "../../systemFile/calibrationFiles/intrinsic.yml",
                  "../../systemFile/calibrationFiles/extrinsic.yml");
            initCaliEigen(caliInfo->getInfo());
        }

        if (nullptr == camera){
            camera = new CameraControl(4, CameraControl::CameraUsedState::LeftGrayRightGrayExColor);
        }
        
        leftRefImg = cv::imread("../../systemFile/refImg/refImgL.tiff", cv::IMREAD_UNCHANGED);
        cv::Mat epilineA_CV = cv::imread("../../systemFile/epilines/epilinesA.tiff", cv::IMREAD_UNCHANGED);
        cv::Mat epilineB_CV = cv::imread("../../systemFile/epilines/epilinesB.tiff", cv::IMREAD_UNCHANGED);
        cv::Mat epilineC_CV = cv::imread("../../systemFile/epilines/epilinesC.tiff", cv::IMREAD_UNCHANGED);
        epilinesA.upload(epilineA_CV);
        epilinesB.upload(epilineB_CV);
        epilinesC.upload(epilineC_CV);
    }
    catch(...){
        return false;
    }
    return true;
}

void OpenGLItem::setDisplayMode(const int mode) {
    switch (mode) {
        case 0:{
            render->setMeshMode(Render::DisplayMode::point);
            break;
        }
        case 1:{
            render->setMeshMode(Render::DisplayMode::normal);
            break;
        }
        case 2:{
            render->setMeshMode(Render::DisplayMode::mesh);
            break;
        }
    }
    window()->update();
}

void OpenGLItem::setDisplayState(const int state){
    switch (state) {
        case 0:{
            render->setDisplayState(Render::DisplayState::Plain);
            render->setMagCofficient(1.f);
            break;
        }
        case 1:{
            render->setDisplayState(Render::DisplayState::threeDimensional);
            break;
        }
    }
    window()->update();
}

void OpenGLItem::updateMagCofficient(const float magCofficient_){
    render->setMagCofficient(magCofficient_);
    window()->update();
}
