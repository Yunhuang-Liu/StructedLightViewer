#include "../include/Render.h"

Render::Render() : skyBoxArrayLightStyle(nullptr),skyBoxArrayDarkStyle(nullptr),skyBoxVbo(QOpenGLBuffer::VertexBuffer),fovy(80),cameraPos(0,0,-500), magCofficient(1.f),
    coodinateVbo(QOpenGLBuffer::VertexBuffer),currentTexture(nullptr),rows(1024),cols(1280),isGetWorldPoint(false),meshEBO(QOpenGLBuffer::IndexBuffer), window(nullptr),
    displayState(DisplayState::threeDimensional),displayMode(DisplayMode::point),depthTexture(QOpenGLTexture::Target2D),depthBuffer(QOpenGLBuffer::VertexBuffer){
    model.setToIdentity();
    model.rotate(180, QVector3D(0, 0, 1));
    view.setToIdentity();
    view.translate(cameraPos);
    project.setToIdentity();
    rotateQuaternion = QQuaternion();
    R1Inv.setToIdentity();
    colorTexture = cv::Mat(rows, cols, CV_8UC3, cv::Scalar(255, 255, 255));
}

Render::~Render(){

}

void Render::initializeGL(){
    initializeOpenGLFunctions();
    glClearColor(0.0f,0.0f,0.0f,1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    initializeSkyBox();
    initializeCoodinate();
    initializeGrid();
    initializeCloud();
}

void Render::resizeGL(const int w, const int h){
    windowWidth = w;
    windowHeight = h;
    glViewport(0,0,windowWidth,windowHeight);
    project.perspective(fovy,static_cast<float>(windowWidth) / windowHeight,1,10000000);
}

void Render::repaintGL(){
    window->beginExternalCommands();

    glClearColor(0.0f,0.0f,0.0f,1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawSkyBox();
    drawCoodinate();
    drawGrid();
    std::lock_guard<std::mutex> renderLock(renderMutex);
    drawCloud();

    window->endExternalCommands();
}

void Render::drawSkyBox(){
    skyBoxVao.bind();
    //skyBoxVbo.bind();
    currentTexture->bind();
    skyBoxShaderProgram.bind();
    skyBoxShaderProgram.setUniformValue("view",view);
    skyBoxShaderProgram.setUniformValue("project",project);
    glDrawArrays(GL_QUADS,0,24);
    skyBoxVao.release();
    //skyBoxVbo.release();
    currentTexture->release();
    skyBoxShaderProgram.release();
}

void Render::initializeSkyBox(){
    skyBoxShaderProgram.create();
    skyBoxShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shader/shader/skyBox.vert");
    skyBoxShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shader/shader/skyBox.frag");
    skyBoxShaderProgram.link();

    QImage image_xn(":/skyBox/cubeMap/nightSky/1.png");
    QImage image_zp(":/skyBox/cubeMap/nightSky/2.png");
    QImage image_xp(":/skyBox/cubeMap/nightSky/3.png");
    QImage image_zn(":/skyBox/cubeMap/nightSky/4.png");
    QImage image_yp(":/skyBox/cubeMap/nightSky/5.png");
    QImage image_yn(":/skyBox/cubeMap/nightSky/6.png");
    skyBoxArrayLightStyle = new QOpenGLTexture(QOpenGLTexture::Target2DArray);
    skyBoxArrayLightStyle->create();
    skyBoxArrayLightStyle->setSize(skyBoxWidth, skyBoxHeight);
    skyBoxArrayLightStyle->setLayers(6);
    skyBoxArrayLightStyle->setFormat(QOpenGLTexture::QOpenGLTexture::RGB8_UNorm);
    skyBoxArrayLightStyle->allocateStorage();
    skyBoxArrayLightStyle->setData(0,0,QOpenGLTexture::RGB,QOpenGLTexture::UInt8,image_xn.convertToFormat(QImage::Format_RGB888).constBits());
    skyBoxArrayLightStyle->setData(0,1,QOpenGLTexture::RGB,QOpenGLTexture::UInt8,image_zp.convertToFormat(QImage::Format_RGB888).constBits());
    skyBoxArrayLightStyle->setData(0,2,QOpenGLTexture::RGB,QOpenGLTexture::UInt8,image_xp.convertToFormat(QImage::Format_RGB888).constBits());
    skyBoxArrayLightStyle->setData(0,3,QOpenGLTexture::RGB,QOpenGLTexture::UInt8,image_zn.convertToFormat(QImage::Format_RGB888).constBits());
    skyBoxArrayLightStyle->setData(0,4,QOpenGLTexture::RGB,QOpenGLTexture::UInt8,image_yp.convertToFormat(QImage::Format_RGB888).constBits());
    skyBoxArrayLightStyle->setData(0,5,QOpenGLTexture::RGB,QOpenGLTexture::UInt8,image_yn.convertToFormat(QImage::Format_RGB888).constBits());
    skyBoxArrayLightStyle->setMinMagFilters(QOpenGLTexture::Nearest,QOpenGLTexture::Linear);
    skyBoxArrayLightStyle->setWrapMode(QOpenGLTexture::ClampToEdge);

    QImage image_xn_darkStyle(":/skyBox/cubeMap/nightSky2/1.png");
    QImage image_zp_darkStyle(":/skyBox/cubeMap/nightSky2/2.png");
    QImage image_xp_darkStyle(":/skyBox/cubeMap/nightSky2/3.png");
    QImage image_zn_darkStyle(":/skyBox/cubeMap/nightSky2/4.png");
    QImage image_yp_darkStyle(":/skyBox/cubeMap/nightSky2/5.png");
    QImage image_yn_darkStyle(":/skyBox/cubeMap/nightSky2/6.png");

    skyBoxArrayDarkStyle = new QOpenGLTexture(QOpenGLTexture::Target2DArray);
    skyBoxArrayDarkStyle->create();
    skyBoxArrayDarkStyle->setSize(skyBoxWidth, skyBoxHeight);
    skyBoxArrayDarkStyle->setLayers(6);
    skyBoxArrayDarkStyle->setFormat(QOpenGLTexture::QOpenGLTexture::RGB8_UNorm);
    skyBoxArrayDarkStyle->allocateStorage();
    skyBoxArrayDarkStyle->setData(0,0,QOpenGLTexture::RGB,QOpenGLTexture::UInt8,image_xn_darkStyle.convertToFormat(QImage::Format_RGB888).constBits());
    skyBoxArrayDarkStyle->setData(0,1,QOpenGLTexture::RGB,QOpenGLTexture::UInt8,image_zp_darkStyle.convertToFormat(QImage::Format_RGB888).constBits());
    skyBoxArrayDarkStyle->setData(0,2,QOpenGLTexture::RGB,QOpenGLTexture::UInt8,image_xp_darkStyle.convertToFormat(QImage::Format_RGB888).constBits());
    skyBoxArrayDarkStyle->setData(0,3,QOpenGLTexture::RGB,QOpenGLTexture::UInt8,image_zn_darkStyle.convertToFormat(QImage::Format_RGB888).constBits());
    skyBoxArrayDarkStyle->setData(0,4,QOpenGLTexture::RGB,QOpenGLTexture::UInt8,image_yp_darkStyle.convertToFormat(QImage::Format_RGB888).constBits());
    skyBoxArrayDarkStyle->setData(0,5,QOpenGLTexture::RGB,QOpenGLTexture::UInt8,image_yn_darkStyle.convertToFormat(QImage::Format_RGB888).constBits());
    skyBoxArrayDarkStyle->setMinMagFilters(QOpenGLTexture::Nearest,QOpenGLTexture::Linear);
    skyBoxArrayDarkStyle->setWrapMode(QOpenGLTexture::ClampToEdge);

    currentTexture = skyBoxArrayLightStyle;

    const GLfloat skyBoxVertexs[]{
        //vertex points
        //left side
        -100000.0,+100000.0,+100000.0,
        -100000.0,-100000.0,+100000.0,
        -100000.0,-100000.0,-100000.0,
        -100000.0,+100000.0,-100000.0,
        //behind side
        -100000.0,+100000.0,-100000.0,
        -100000.0,-100000.0,-100000.0,
        +100000.0,-100000.0,-100000.0,
        +100000.0,+100000.0,-100000.0,
        //right side
        +100000.0,+100000.0,-100000.0,
        +100000.0,-100000.0,-100000.0,
        +100000.0,-100000.0,+100000.0,
        +100000.0,+100000.0,+100000.0,
        //front side
        -100000.0,+100000.0,+100000.0,
        +100000.0,+100000.0,+100000.0,
        +100000.0,-100000.0,+100000.0,
        -100000.0,-100000.0,+100000.0,
        //top side
        -100000.0,+100000.0,-100000.0,
        +100000.0,+100000.0,-100000.0,
        +100000.0,+100000.0,+100000.0,
        -100000.0,+100000.0,+100000.0,
        //bottom side
        -100000.0,-100000.0,-100000.0,
        -100000.0,-100000.0,+100000.0,
        +100000.0,-100000.0,+100000.0,
        +100000.0,-100000.0,-100000.0,

        //texture points
        //left side
        +0.0,+0.0,
        +0.0,+1.0,
        +1.0,+1.0,
        +1.0,+0.0,
        //behind side
        +0.0,+0.0,
        +0.0,+1.0,
        +1.0,+1.0,
        +1.0,+0.0,
        //right side
        +0.0,+0.0,
        +0.0,+1.0,
        +1.0,+1.0,
        +1.0,+0.0,
        //front side
        +1.0,+0.0,
        +0.0,+0.0,
        +0.0,+1.0,
        +1.0,+1.0,
        //top side
        +0.0,+1.0,
        +1.0,+1.0,
        +1.0,+0.0,
        +0.0,+0.0,
        //bottom side
        +0.0,+0.0,
        +0.0,+1.0,
        +1.0,+1.0,
        +1.0,+0.0
    };

    skyBoxVao.create();
    skyBoxVao.bind();

    skyBoxVbo.create();
    skyBoxVbo.bind();
    skyBoxVbo.allocate(skyBoxVertexs,sizeof(skyBoxVertexs));

    skyBoxShaderProgram.bind();
    skyBoxShaderProgram.enableAttributeArray(0);
    skyBoxShaderProgram.setAttributeBuffer(0,GL_FLOAT,0,3,3*sizeof(GLfloat));
    skyBoxShaderProgram.enableAttributeArray(1);
    skyBoxShaderProgram.setAttributeBuffer(1,GL_FLOAT,6*4*3*sizeof(GLfloat),2,2*sizeof(GLfloat));

    skyBoxVbo.release();
    skyBoxVao.release();
    skyBoxShaderProgram.release();
}

void Render::setCameraPos(const QVector3D& cameraPos_){
    cameraPos = cameraPos_;
    view.setToIdentity();
    view.translate(cameraPos);
    view.rotate(rotateQuaternion);
}

void Render::setView(const QQuaternion& quaternion){
    rotateQuaternion = quaternion;
    view.setToIdentity();
    view.translate(cameraPos);
    view.rotate(rotateQuaternion);
}

void Render::setView(const QMatrix4x4& view_){
    rotateQuaternion = QQuaternion();
    view = view_;
}

void Render::setFovy(const float fovy_){
    fovy = fovy_;
    project.setToIdentity();
    project.perspective(fovy,static_cast<float>(windowWidth) / windowHeight,1,10000000);
}

void Render::drawCoodinate(){
    coodinateVao.bind();
    //coodinateVbo.bind();
    coodinateShaderProgram.bind();
    coodinateShaderProgram.setUniformValue("view",view);
    coodinateShaderProgram.setUniformValue("project",project);
    glLineWidth(2);
    glDrawArrays(GL_LINES,0,6);
    coodinateVao.release();
    //coodinateVbo.release();
    coodinateShaderProgram.release();

    coodinateTriangleVao.bind();
    coodinateShaderProgram.bind();
    //triangleVbo.bind();
    coodinateShaderProgram.setUniformValue("view",view);
    coodinateShaderProgram.setUniformValue("project",project);
    glLineWidth(2);
    glDrawArrays(GL_TRIANGLES,0,18);
    //triangleVbo.release();
    coodinateShaderProgram.release();
    coodinateTriangleVao.release();
}

void Render::initializeCoodinate(){
    static float coord_vertices[] = {
        //坐标轴
        0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        100.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.0f,  100.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  100.0f,  0.0f,  0.0f,  1.0f,
    };

    static float triangle_vertices[] = {
        //箭头
        100.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        90.f,  0.0f,  -10.f,  1.0f,  0.0f,  0.0f,
        90.f,  0.0f,  10.f,  1.0f,  0.0f,  0.0f,
        100.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        90.f,  10.f,  0.0f,  1.0f,  0.0f,  0.0f,
        90.f,  -10.f,  0.0f,  1.0f,  0.0f,  0.0f,

        0.0f,  100.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -10.f,  90.f,  0.0f,  0.0f,  1.0f,  0.0f,
        10.f,  90.f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.0f, 100.f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.0f,  90.f,  10.f,  0.0f,  1.0f,  0.0f,
        0.0f,  90.f,  -10.f,  0.0f,  1.0f,  0.0f,

        0.0f,  0.0f,  100.f,  0.0f,  0.0f,  1.0f,
        -10.f,  0.0f,  90.f,  0.0f,  0.0f,  1.0f,
        10.f,  0.0f,  90.f,  0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  100.f,  0.0f,  0.0f,  1.0f,
        0.0f,  10.f,  90.f,  0.0f,  0.0f,  1.0f,
        0.0f,  -10.f,  90.f,  0.0f,  0.0f,  1.0f
    };
    coodinateVao.create();
    coodinateVao.bind();
    coodinateVbo.create();
    coodinateVbo.bind();
    coodinateVbo.allocate(coord_vertices,sizeof(coord_vertices));
    coodinateShaderProgram.create();
    coodinateShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shader/shader/coordinate.vert");
    coodinateShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shader/shader/coordinate.frag");
    coodinateShaderProgram.link();
    coodinateShaderProgram.bind();
    coodinateShaderProgram.enableAttributeArray(0);
    coodinateShaderProgram.setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(GLfloat) * 6);
    coodinateShaderProgram.enableAttributeArray(1);
    coodinateShaderProgram.setAttributeBuffer(1,GL_FLOAT,sizeof(GLfloat) * 3,3,sizeof(GLfloat) * 6);
    coodinateVao.release();
    coodinateVbo.release();

    coodinateTriangleVao.create();
    coodinateTriangleVao.bind();
    triangleVbo.create();
    triangleVbo.bind();
    triangleVbo.allocate(triangle_vertices,sizeof(triangle_vertices));
    coodinateShaderProgram.enableAttributeArray(0);
    coodinateShaderProgram.setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(GLfloat) * 6);
    coodinateShaderProgram.enableAttributeArray(1);
    coodinateShaderProgram.setAttributeBuffer(1,GL_FLOAT,sizeof(GLfloat) * 3,3,sizeof(GLfloat) * 6);
    coodinateTriangleVao.release();
    triangleVbo.release();
    coodinateShaderProgram.release();
}

void Render::initializeGrid(){
    const float gridDistance = 10000;
    const float farDistance = gridDistance / 2;
    const float constYDepth = 500;
    const float widthPerGridCeil = gridDistance / (25 - 1);
    const int vertexNum = (25 + 25) * 2 * 3 + 4 * 3;
    float grid_vertex[vertexNum];
    for(int i=0;i< 25;i++){
        grid_vertex[i * 2 * 3] = -farDistance;
        grid_vertex[i * 2 * 3 + 1] = -constYDepth;
        grid_vertex[i * 2 * 3 + 2] = -farDistance + widthPerGridCeil * i;

        grid_vertex[i * 2 * 3 + 3] = farDistance;
        grid_vertex[i * 2 * 3 + 4] = -constYDepth;
        grid_vertex[i * 2 * 3 + 5] = -farDistance + widthPerGridCeil * i;
    }

    for(int i=0;i< 25;i++){
        grid_vertex[25 * 2 * 3 + i * 2 * 3] = -farDistance + widthPerGridCeil * i;
        grid_vertex[25 * 2 * 3 + i * 2 * 3 + 1] = -constYDepth;
        grid_vertex[25 * 2 * 3 + i * 2 * 3 + 2] = -farDistance;

        grid_vertex[25 * 2 * 3 + i * 2 * 3 + 3] = -farDistance + widthPerGridCeil * i;
        grid_vertex[25 * 2 * 3 + i * 2 * 3 + 4] = -constYDepth;
        grid_vertex[25 * 2 * 3 + i * 2 * 3 + 5] = farDistance;
    }

    float boldGrid_vertex[12] = {
        -farDistance, -constYDepth, 0,
        farDistance, -constYDepth, 0,
        0, -constYDepth, -farDistance,
        0, -constYDepth,farDistance
    };

    gridVao.create();
    gridVao.bind();
    gridVbo.create();
    gridVbo.bind();
    gridVbo.allocate(grid_vertex,sizeof(grid_vertex));
    gridShaderProgram.create();
    gridShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shader/shader/grid.vert");
    gridShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shader/shader/grid.frag");
    gridShaderProgram.link();
    gridShaderProgram.bind();
    gridShaderProgram.enableAttributeArray(0);
    gridShaderProgram.setAttributeBuffer(0,GL_FLOAT,0,3);
    gridVbo.release();
    gridVao.release();
    boldGridVao.create();
    boldGridVao.bind();
    boldGridVbo.create();
    boldGridVbo.bind();
    boldGridVbo.allocate(boldGrid_vertex,sizeof(boldGrid_vertex));
    gridShaderProgram.enableAttributeArray(0);
    gridShaderProgram.setAttributeBuffer(0,GL_FLOAT,0,3);
    boldGridVao.release();
    boldGridVbo.release();
    gridShaderProgram.release();
}

void Render::drawGrid(){
    gridVao.bind();
    //gridVbo.bind();
    gridShaderProgram.bind();
    gridShaderProgram.setUniformValue("view",view);
    gridShaderProgram.setUniformValue("project",project);
    glLineWidth(2);
    glDrawArrays(GL_LINES,0,100);
    gridVao.release();
    //gridVbo.release();
    gridShaderProgram.release();

    boldGridVao.bind();
    //boldGridVbo.bind();
    gridShaderProgram.bind();
    gridShaderProgram.setUniformValue("view",view);
    gridShaderProgram.setUniformValue("project",project);
    glLineWidth(10);
    glDrawArrays(GL_LINES,0,4);
    boldGridVao.release();
    //boldGridVbo.release();
    gridShaderProgram.release();
}

void Render::changeScence(const SkyBox skyBoxType){
    switch (skyBoxType) {
        case SkyBox::lightNight:{
            currentTexture = skyBoxArrayLightStyle;
            break;
        }
        case SkyBox::darkNight:{
            currentTexture = skyBoxArrayDarkStyle;
            break;
        }
    }
}

void Render::initializeCloud() {
    size_t size = sizeof(float) * 7 * rows * cols;
    cloudVao.create();
    cloudVao.bind();
    glGenBuffers(1, &cloudVbo);
    glBindBuffer(GL_ARRAY_BUFFER, cloudVbo);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
    cudaGraphicsGLRegisterBuffer(&m_vboResource, cloudVbo, cudaGraphicsRegisterFlagsNone);
    cudaGraphicsMapResources(1, &m_vboResource, 0);
    cudaGraphicsResourceGetMappedPointer((void**)&ptrGLResource, &size, m_vboResource);
    
    cloudProgram.create();
    cloudProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/shader/cloud.vert");
    cloudProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/shader/cloud.frag");
    cloudProgram.link();
    
    cloudProgram.enableAttributeArray(0);
    cloudProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(GLfloat) * 7);
    cloudProgram.enableAttributeArray(1);
    cloudProgram.setAttributeBuffer(1, GL_FLOAT, sizeof(GLfloat) * 3, 4, sizeof(GLfloat) * 7);

    /**         模型如下
     *          2 - 3 - 3 - 1
     *          3 - 6 - 6 - 3
     *          3 - 6 - 6 - 3
     *          1 - 3 - 3 - 2
     */
    meshNum = (rows * cols - rows * 2 - (cols - 2) * 2) * 6 + (rows - 2 + cols - 2) * 2 * 3 + 1 * 2 + 2 * 2;
    GLuint* index = new GLuint[meshNum];
    int countIndex = 0;
    for (int i = 1; i < rows; ++i) {
        for (int j = 1; j < cols; ++j) {
            index[countIndex] = rows * i + j;
            index[countIndex + 1] = rows * (i - 1) + j;
            index[countIndex + 2] = rows * (i - 1) + j - 1;
            index[countIndex + 3] = rows * (i - 1) + j - 1;
            index[countIndex + 4] = rows * i + j - 1;
            index[countIndex + 5] = rows * i + j;
            countIndex += 6;
        }
    }
    meshEBO.create();
    meshEBO.bind();
    meshEBO.allocate(index, sizeof(index));

    cloudVao.release();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    meshEBO.release();
    cloudProgram.release();

    //2D状态
    depthVao.create();
    depthVao.bind();
    float vertexColorData[] = {
        0.5f, -0.5f, 1.f, 0.f,
        0.5f, 0.5f, 1.f, 1.f,
        -0.5f, 0.5f, 0.f, 1.f,

        0.5f, -0.5f, 1.f, 0.f,
        -0.5f, 0.5f, 0.f, 1.f,
        -0.5f, -0.5f, 0.f, 0.f
    };
    depthBuffer.create();
    depthBuffer.bind();
    depthBuffer.allocate(vertexColorData,sizeof(vertexColorData));
    depthTexture.create();
    depthTexture.setData(QImage(colorTexture.data, colorTexture.cols, colorTexture.rows, colorTexture.step, QImage::Format_BGR888).mirrored());
    depthTexture.setMinMagFilters(QOpenGLTexture::Nearest,QOpenGLTexture::Linear);
    depthTexture.generateMipMaps();
    depthProgram.create();
    depthProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shader/shader/depth.vert");
    depthProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shader/shader/depth.frag");
    depthProgram.link();
    depthProgram.enableAttributeArray(0);
    depthProgram.setAttributeBuffer(0,GL_FLOAT,0,2,4 * sizeof(GLfloat));
    depthProgram.enableAttributeArray(1);
    depthProgram.setAttributeBuffer(1,GL_FLOAT,2 * sizeof(GLfloat),2,4 * sizeof(GLfloat));
    depthVao.release();
    depthProgram.release();
    depthBuffer.release();
}

void Render::drawCloud() {
    if(displayState == DisplayState::threeDimensional){
        cudaGraphicsUnmapResources(1, &m_vboResource, 0);
        ptrGLResource = nullptr;
        cloudVao.bind();
        //glBindBuffer(GL_ARRAY_BUFFER, cloudVbo);
        cloudProgram.bind();
        cloudProgram.setUniformValue("R1Inv", R1Inv);
        cloudProgram.setUniformValue("model", model);
        cloudProgram.setUniformValue("view", view);
        cloudProgram.setUniformValue("project", project);
        if(displayMode == DisplayMode::point)
            glDrawArrays(GL_POINTS, 0, rows*cols);
        if(displayMode == DisplayMode::mesh){
            //meshEBO.bind();
            glDrawElements(GL_TRIANGLES,meshNum,GL_UNSIGNED_INT,0);
            //meshEBO.release();
        }
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        cloudVao.release();
        cloudProgram.release();
        cudaGraphicsMapResources(1, &m_vboResource, 0);
        cudaGraphicsResourceGetMappedPointer((void**)&ptrGLResource, &size, m_vboResource);
    }
    else{
        depthTexture.destroy();
        depthTexture.create();
        depthTexture.setData(QImage(colorTexture.data, colorTexture.cols, colorTexture.rows, colorTexture.step, QImage::Format_BGR888).mirrored());
        depthTexture.setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Linear);
        depthTexture.generateMipMaps();
        depthVao.bind();
        depthTexture.bind(0);
        depthProgram.bind();
        depthProgram.setUniformValue("texture",0);
        depthProgram.setUniformValue("magCofficient",magCofficient);
        glDrawArrays(GL_TRIANGLES,0,6);
        depthVao.release();
        depthTexture.release();
        depthProgram.release();
    }
}


void Render::renderCloud(const cv::cuda::GpuMat& depthImg, const cv::cuda::GpuMat& colorImg, const Eigen::Matrix3f& intrinsic_inv) {
    if(displayState == DisplayState::threeDimensional){
        std::lock_guard<std::mutex> renderLock(renderMutex);
        renderCloud_CUDA(depthImg, colorImg, intrinsic_inv, ptrGLResource);
    }
    else{
        cv::Mat textureDepth;
        depthImg.download(textureDepth);
        textureDepth = textureDepth / 50;
        textureDepth.convertTo(textureDepth, CV_8UC1);
        cv::applyColorMap(textureDepth, colorTexture, cv::COLORMAP_JET);
    }
}

void Render::setR1Inv(const cv::Mat& R1Inv_) {
    cv::Mat transR1Inv = R1Inv_.t();
    R1Inv.setToIdentity();
    R1Inv(0, 0) = transR1Inv.ptr<double>(0)[0];
    R1Inv(0, 1) = transR1Inv.ptr<double>(0)[1];
    R1Inv(0, 2) = transR1Inv.ptr<double>(0)[2];

    R1Inv(1, 0) = transR1Inv.ptr<double>(1)[0];
    R1Inv(1, 1) = transR1Inv.ptr<double>(1)[1];
    R1Inv(1, 2) = transR1Inv.ptr<double>(1)[2];

    R1Inv(2, 0) = transR1Inv.ptr<double>(2)[0];
    R1Inv(2, 1) = transR1Inv.ptr<double>(2)[1];
    R1Inv(2, 2) = transR1Inv.ptr<double>(2)[2];
}

void Render::setMeshMode(const DisplayMode displayMode_){
    displayMode = displayMode_;
}

void Render::setDisplayState(const DisplayState displayState_){
    displayState = displayState_;
    magCofficient = 1.f;
}

void Render::setMagCofficient(const float magCofficient_){
    magCofficient = magCofficient_;
}

void Render::setWindow(QQuickWindow* window_) {
    window = window_;
}