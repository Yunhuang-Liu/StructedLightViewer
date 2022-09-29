#include "../include/OpenGLItem.h"

OpenGLItem::OpenGLItem() : render(nullptr), fps(0), front(QVector3D(0,0,-1)), right(QVector3D(1,0,0)),upper(QVector3D(0,1,0)),
    cameraPos(QVector3D(0,0,-500)),moveSensity(5.f),rotateSensity(0.5f),fovy(80),nearFarSensity(5.f),caliInfo(nullptr),isStop(true),
    camera(nullptr),minDepth(1000),maxDepth(3000),grayExposureTime(400),colorExposureTime(400),isUpdateWindow(false), frameCaptured(0), frameRestructed(0){
    timer.start(10,this);
    lastTime = QTime::currentTime();
    connect(this,&QQuickItem::windowChanged,this,[&](QQuickWindow* window){
        if(nullptr != window){
            connect(window,&QQuickWindow::beforeSynchronizing,this,&OpenGLItem::sync,Qt::DirectConnection);
            connect(window,&QQuickWindow::sceneGraphInvalidated,this,&OpenGLItem::cleanUp,Qt::DirectConnection);
            window->setColor(Qt::white);
        }
    });
    imgsLast.resize(3);
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
    //if (isUpdateWindow.load(std::memory_order_acquire)) {
        window()->update();
        //isUpdateWindow.store(false, std::memory_order_release);
    //}
    //window()->update();
    //isUpdateWindow.store(false, std::memory_order_release);
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
    creatorThread = std::thread(&OpenGLItem::creatImg, this);
}

void OpenGLItem::remapImg(const cv::Mat& src, const cv::cuda::GpuMat& remap_x, const cv::cuda::GpuMat& remap_y, cv::cuda::GpuMat& outImg, cv::cuda::Stream stream, const bool convertToGray) {
    cv::cuda::GpuMat src_dev;
    src_dev.upload(src, stream);
    if (convertToGray)
        cv::cuda::cvtColor(src_dev, src_dev, cv::COLOR_BGR2GRAY, 0, stream);
    cv::cuda::remap(src_dev, outImg, remap_x, remap_y, cv::INTER_LINEAR, 0, cv::Scalar(), stream);
}

void OpenGLItem::pause(){
    isStop.store(true, std::memory_order_release);
    startThread.join();
    creatorThread.join();
    std::queue<RestructedFrame> emptyCreator;
    creatorQueue.swap(emptyCreator);
    isUpdateWindow.store(false, std::memory_order_release);
    frameCaptured = 0;
}

void OpenGLItem::creatImg() {
    camera->setCameraExposure(grayExposureTime, colorExposureTime);
    while (!isStop.load(std::memory_order_acquire)) {
        auto start = std::chrono::system_clock::now();
        RestructedFrame frame;
        camera->getOneFrameImgs(frame);
        if(frameCaptured == 0)
            creatorQueue.push(frame);
        else {
            std::vector<cv::Mat> firstLeft(frame.leftImgs.begin(), frame.leftImgs.begin() + 3);
            std::vector<cv::Mat> firstRight(frame.leftImgs.begin(), frame.leftImgs.begin() + 3);
            std::vector<cv::Mat> secLeft(frame.leftImgs.end() - 3, frame.leftImgs.end());
            std::vector<cv::Mat> secRight(frame.rightImgs.end() - 3, frame.rightImgs.end());
            creatorQueue.push(RestructedFrame(firstLeft,
                firstRight,
                frame.colorImgs));

            creatorQueue.push(RestructedFrame(secLeft,
                secRight,
                frame.colorImgs));
        }

        ++frameCaptured;

        auto end = std::chrono::system_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * static_cast<float>(std::chrono::microseconds::period::num) / std::chrono::milliseconds::period::den;
        std::cout << "Capture img frame rate:" << 2.f / time << std::endl;
    }
}

void OpenGLItem::startRun() {
    const Info& info = caliInfo->getInfo();
    const cv::Size imgSize = cv::Size(info.S.at<double>(0,0), info.S.at<double>(1, 0));
    render->setR1Inv(info.R1.inv());
    cv::Mat invM1 = info.M1.inv();
    invM1.convertTo(invM1, CV_32FC1);
    Eigen::Matrix3f intrinsic_inv;
    cv::cv2eigen(invM1, intrinsic_inv);
    PhaseSolverType::FourFloorFourStepMaster_GPU* phaseSolverLeft = new PhaseSolverType::FourFloorFourStepMaster_GPU();
    PhaseSolverType::FourFloorFourStepMaster_GPU* phaseSolverRight = new PhaseSolverType::FourFloorFourStepMaster_GPU();
    RestructorType::Restructor_GPU* restructor = new RestructorType::Restructor_GPU(info, -400, 400, minDepth, maxDepth);
    cv::Mat remap_x_L;
    cv::Mat remap_y_L;
    cv::Mat remap_x_R;
    cv::Mat remap_y_R;
    cv::cuda::GpuMat remap_x_deice_L;
    cv::cuda::GpuMat remap_y_deice_L;
    cv::cuda::GpuMat remap_x_deice_R;
    cv::cuda::GpuMat remap_y_deice_R;
    if (remap_x_L.empty()) {
        cv::initUndistortRectifyMap(info.M1, info.D1, info.R1, info.P1, imgSize, CV_32FC1, remap_x_L, remap_y_L);
        cv::initUndistortRectifyMap(info.M2, info.D2, info.R2, info.P2, imgSize, CV_32FC1, remap_x_R, remap_y_R);
    }
    if (remap_x_deice_L.empty()) {
        remap_x_deice_L.upload(remap_x_L);
        remap_y_deice_L.upload(remap_y_L);
        remap_x_deice_R.upload(remap_x_R);
        remap_y_deice_R.upload(remap_y_R);
    }
    while (!isStop.load(std::memory_order_acquire)) {
        while (creatorQueue.size() <= 0) { std::this_thread::sleep_for(std::chrono::milliseconds(30)); }
        auto start = std::chrono::system_clock::now();
        RestructedFrame& frame = creatorQueue.front();

        frame.colorImgs.resize(1);
        std::thread threadsCaculateColor = std::thread([&] {
                if (frameRestructed == 0)
                    frame.colorImgs[0] = (frame.leftImgs[2] + frame.leftImgs[4]) / 2;
                else
                    frame.colorImgs[0] = (imgsLast[2] + frame.leftImgs[1]) / 2;
                cv::cvtColor(frame.colorImgs[0], frame.colorImgs[0], cv::COLOR_GRAY2BGR);
            }
        );

        const int imgNums = frame.leftImgs.size();
        std::vector<cv::cuda::Stream> streamsRecImg(imgNums * 2, cv::cuda::Stream(cudaStreamNonBlocking));
        std::vector<cv::cuda::GpuMat> imgRec_dev_L(imgNums);
        std::vector<cv::cuda::GpuMat> imgRec_dev_R(imgNums);
        for (int i = 0; i < imgNums; i++) {
            imgRec_dev_L[i] = cv::cuda::createContinuous(imgSize, CV_8UC1);
            remapImg(frame.leftImgs[i], remap_x_deice_L, remap_y_deice_L, imgRec_dev_L[i], streamsRecImg[i * 2],false);
            imgRec_dev_R[i] = cv::cuda::createContinuous(imgSize, CV_8UC1);
            remapImg(frame.rightImgs[i], remap_x_deice_R, remap_y_deice_R, imgRec_dev_R[i], streamsRecImg[i * 2 + 1], false);
        }

        threadsCaculateColor.join();

        for (auto& stream : streamsRecImg) {
            stream.waitForCompletion();
        }

        phaseSolverLeft->changeSourceImg(imgRec_dev_L);
        phaseSolverRight->changeSourceImg(imgRec_dev_R);

        cv::cuda::Stream leftSolvePhase(cudaStreamNonBlocking);
        cv::cuda::Stream rightSolvePhase(cudaStreamNonBlocking);
        std::vector<cv::cuda::GpuMat> unwrapImgLeft_dev;
        std::vector<cv::cuda::GpuMat> unwrapImgRight_dev;
        phaseSolverLeft->getUnwrapPhaseImg(unwrapImgLeft_dev, leftSolvePhase);
        phaseSolverRight->getUnwrapPhaseImg(unwrapImgRight_dev, rightSolvePhase);

        leftSolvePhase.waitForCompletion();
        rightSolvePhase.waitForCompletion();

        cv::cuda::Stream stream_Restructor(cudaStreamNonBlocking);
        cv::cuda::GpuMat depthImg;
        cv::cuda::GpuMat colorImg;
        colorImg.upload(frame.colorImgs[0], stream_Restructor);
        restructor->restruction(unwrapImgLeft_dev[0], unwrapImgRight_dev[0], 0, stream_Restructor);

        stream_Restructor.waitForCompletion();
        restructor->download(0, depthImg);

        cv::Mat test[10];
        depthImg.download(test[0]);
        colorImg.download(test[1]);
        unwrapImgLeft_dev[0].download(test[4]);
        unwrapImgRight_dev[0].download(test[6]);

        /*
        while (isUpdateWindow.load(std::memory_order_acquire)) {
            std::cout << "Displaying!" << std::endl;
            if (isStop.load(std::memory_order_acquire))
                break;
        }
        */
        render->renderCloud(depthImg, colorImg, intrinsic_inv);
        //isUpdateWindow.store(true, std::memory_order_release);

        //imgsLast.clear();
        if (frameRestructed == 0) {
            imgsLast[0] = frame.leftImgs[3];
            imgsLast[1] = frame.leftImgs[4];
            imgsLast[2] = frame.leftImgs[5];
        }
        else {
            imgsLast[0] = frame.leftImgs[0];
            imgsLast[1] = frame.leftImgs[1];
            imgsLast[2] = frame.leftImgs[2];
        }
        creatorQueue.pop();

        ++frameRestructed;

        auto end = std::chrono::system_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * static_cast<float>(std::chrono::microseconds::period::num) / std::chrono::milliseconds::period::den;
        fps = 1 / time;
        emit fpsChanged();
        //std::cout << "RenderRate:" << 1 / time << std::endl;

        if (isStop.load(std::memory_order_acquire))
            break;
    }
    delete restructor;
    delete phaseSolverLeft;
    delete phaseSolverRight;
}

bool OpenGLItem::connectCamera(){
    try{
        //更完善的软件设计方式应当增加异常处理
        if(nullptr == caliInfo)
            caliInfo = new MatrixsInfo("../../systemFile/calibrationFiles/intrinsic.yml", "../../systemFile/calibrationFiles/extrinsic.yml");
        if (nullptr == camera){
            camera = new CameraControl(6, CameraControl::CameraUsedState::LeftGrayRightGray);
            camera->setCaptureImgsNum(6, 6);
        }
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
