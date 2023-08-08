#include "axbopenglwidget.h"
#include <QFile>


AXBOpenGLWidget::AXBOpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    ,VBO(QOpenGLBuffer::VertexBuffer)
    ,xtrans(0),ytrans(0),ztrans(0.0)
{

    QSurfaceFormat format;
    format.setAlphaBufferSize(24);  //设置alpha缓冲大小
    format.setVersion(3,3);         //设置版本号
    format.setSamples(10);          //设置重采样次数，用于反走样
    this->setFormat(format);

    vertices = loadAscllStl(":/shaders/rovmini.stl",1);

}

AXBOpenGLWidget::~AXBOpenGLWidget()
{
    makeCurrent();
}

void AXBOpenGLWidget::set_viewat(QVector3D _camera_position, QVector3D _camera_focus, QVector3D _camera_upward_facing)
{
    camera_position = _camera_position;
    camera_focus = _camera_focus;
    camera_upward_facing = _camera_upward_facing;
}

void AXBOpenGLWidget::drawShape(float _pitch,float _roll, float _yaw)
{
//   qDebug()<<"drawShape"<<endl;
//   qDebug()<<msg.THR;
//   qDebug()<<msg.YAW;
//   qDebug()<<msg.ROL;
//   qDebug()<<msg.PIT;

    m_angle_x=(int)_roll;
    m_angle_y=(int)_pitch;
    m_angle_z= - (int)_yaw;
    update();
}

void AXBOpenGLWidget::setWirefame(bool wireframe)
{
    makeCurrent();
    if(wireframe)
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    update();
    doneCurrent();
}


QVector<float> AXBOpenGLWidget::loadAscllStl(QString filename,int ratio)
{
    QVector<float> vertices_temp;
    qDebug() << "load text file:" << filename;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Open stl_file failed." << endl;
    }
    while (!file.atEnd())
    {
          QString line = file.readLine().trimmed(); // trimmed去除了开头和结尾的空白字符串
          QStringList  words = line.split(' ', QString::SkipEmptyParts);

          if (words[0] == "facet") {
          Normal = {ratio*words[2].toFloat(), ratio*words[3].toFloat(),ratio*words[4].toFloat()};
          }
          else if (words[0] == "vertex") {
            Position = {ratio*words[1].toFloat(), ratio*words[2].toFloat(),ratio*words[3].toFloat()};
            vertices_temp.append(Position);
            vertices_temp.append(Normal);
          }
          else
          {
              continue;
          }

     }

    qDebug() << "write vertice_temp success!" << filename;
    return vertices_temp;
 //   file.close();
}

void AXBOpenGLWidget::initializeGL()
{
    this->initializeOpenGLFunctions();

    shaderprogram.create();//生成着色器程序
    if(!shaderprogram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/stl.vert")){
        qDebug()<<"ERROR:"<<shaderprogram.log();    //如果编译出错,打印报错信息
    }
    if(!shaderprogram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/stl.frag")){
        qDebug()<<"ERROR:"<<shaderprogram.log();    //如果编译出错,打印报错信息
    }
    //将添加到此程序的着色器与addshader链接在一起
     if(!shaderprogram.link()){
         qDebug()<<"ERROR:"<<shaderprogram.log();    //如果链接出错,打印报错信息
     }

     VAO.create();// 创建一个VAO对象，OpenGL会给它（顶点数组缓存对象）分配一个id
     VAO.bind();//将RC中的当前顶点数组缓存对象Id设置为VAO的id
     VBO.create();
     VBO.bind();
     VBO.allocate(vertices.data(),sizeof(float)*vertices.size());//将顶点数据分配到VBO中，第一个参数为数据指针，第二个参数为数据的字节长度

     shaderprogram.setAttributeBuffer("aPos", GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);
     shaderprogram.enableAttributeArray("aPos");
     shaderprogram.setAttributeBuffer("aNormal", GL_FLOAT,sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);
     shaderprogram.enableAttributeArray("aNormal");
     this->glEnable(GL_DEPTH_TEST);


     view.setToIdentity();
     view.lookAt(camera_position,camera_focus,camera_upward_facing);
//     view.lookAt(QVector3D(0.0f, -1.3f, 0.3f), QVector3D(0.0f,0.0f,0.0f), QVector3D(0.0f,0.0f,1.0f));//从屁股后面看
//     view.lookAt(QVector3D(0.0f, 0.0f, 1.5f), QVector3D(0.0f,0.0f,0.0f), QVector3D(0.0f,1.0f,0.0f));//从顶部向下看
     //lookAt参数为三个QVector3D，分别为摄像机位置，摄像机看的点位，最后是摄像机上方的朝向
     // view 是相机坐标系
//    view.translate(0.0,0.0,3);
 //    view.rotate(0.0f,1.0f, 0.0f, 0.0f);

}

void AXBOpenGLWidget::resizeGL(int w, int h)
{
    this->glViewport(0,0,w,h);
    projection.setToIdentity();
//    projection.perspective(45.0f, (GLfloat)w/(GLfloat)h, 0.1f, 100.0f);
    projection.perspective(60.0f, (GLfloat)w/(GLfloat)h, 0.001f, 100.0f);
}

void AXBOpenGLWidget::paintGL()
{

    this->glClearColor(0.5725f,0.7843f,0.8784f,1.0f);//设置清屏颜色
    this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清空颜色缓冲区


    shaderprogram.bind();
    {

    QVector3D lightColor(1.0f,1.0f,1.0f);
    QVector3D objectColor(1.0f,0.5f,0.31f);
    QVector3D lightPos(0.0f,-60.0f,60.0f);//灯的位置需要参考相机位置

    shaderprogram.setUniformValue("objectColor",objectColor);
    shaderprogram.setUniformValue("lightColor",lightColor);
    shaderprogram.setUniformValue("lightPos", lightPos);

    shaderprogram.setUniformValue("ratio", ratio);
    shaderprogram.setUniformValue("view", view);
    shaderprogram.setUniformValue("projection", projection);


    model.setToIdentity();
    model.translate(xtrans, ytrans, ztrans);
    model.rotate(m_angle_x, 1.0f, 0.0f, 0.0f);
    model.rotate(m_angle_y, 0.0f, 1.0f, 0.0f);
    model.rotate(m_angle_z, 0.0f, 0.0f, 1.0f);

    shaderprogram.setUniformValue("model", model);

    int n = vertices.capacity()/sizeof(float);
    qDebug() << n;
    QOpenGLVertexArrayObject::Binder bind(&VAO);//绑定VAO
    this->glDrawArrays(GL_TRIANGLES,0,n);
    }

}


