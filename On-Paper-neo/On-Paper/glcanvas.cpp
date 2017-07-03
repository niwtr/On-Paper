#include "glcanvas.h"
#include "defs.h"

GLCanvas::GLCanvas(QWidget* parent)
    : QOpenGLWidget(parent)
{
//     setFixedSize(MAIN_WIDTH, MAIN_HEIGHT);
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
