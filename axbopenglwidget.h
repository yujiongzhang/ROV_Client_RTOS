#ifndef AXBOPENGLWIDGET_H
#define AXBOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "msg.h"
#include <QVector>
#include <QVector2D>


#include <QStringList>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QQuaternion>


class AXBOpenGLWidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit AXBOpenGLWidget(QWidget *parent = nullptr);
    ~AXBOpenGLWidget();
    void set_viewat(QVector3D _camera_position, QVector3D _camera_focus,QVector3D _camera_upward_facing);

    void drawShape(float _pitch,float _roll,float _yaw);
    void setWirefame(bool wireframe);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    QVector<float> loadAscllStl(QString filename,int ratio);//文件名和放大系数

private:


    QVector<float> vertices;
    QVector<float> Position;
    QVector<float> Normal;//读文件时的俩个临时变量顶点位置，法向量

    QOpenGLShaderProgram shaderprogram;
    QOpenGLVertexArrayObject VAO;//声明VAO顶点数组对象
    QOpenGLBuffer VBO;//声明VBO数组缓冲对象

    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;
    float ratio=1.0;

    QVector3D camera_position;
    QVector3D camera_focus;
    QVector3D camera_upward_facing;

    int m_angle_x=0;
    int m_angle_y=0;
    int m_angle_z=0;

    GLfloat xtrans, ytrans, ztrans; // translation on x,y,z-axis

    QQuaternion rotation;


};



#endif // AXBOPENGLWIDGET_H
