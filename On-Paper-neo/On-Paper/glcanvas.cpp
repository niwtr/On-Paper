#include "glcanvas.h"

GLCanvas::GLCanvas(QWidget* parent)
    : QOpenGLWidget(parent)
{
     setFixedSize(1024,768);
}

void GLCanvas::setImage(const QImage& image)
{
    img = image;
}

void GLCanvas::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    //Set the painter to use a smooth scaling algorithm.
    p.setRenderHint(QPainter::SmoothPixmapTransform, 1);

    p.drawImage(this->rect(), img);
}
