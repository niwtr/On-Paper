#ifndef GLCANVAS_H
#define GLCANVAS_H

#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QPainter>
class GLCanvas : public QOpenGLWidget{
public:
    GLCanvas(QWidget* parent = NULL);
    void setImage(const QImage& image);
protected:
    void paintEvent(QPaintEvent*);
private:
    QImage img;
};


#endif // GLCANVAS_H
