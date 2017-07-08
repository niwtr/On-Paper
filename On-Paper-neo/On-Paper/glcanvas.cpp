#include "glcanvas.h"
#include "defs.h"
#include <iostream>
using std::cout;using std::endl;
GLCanvas::GLCanvas(QWidget* parent)
    : QOpenGLWidget(parent)
{
     setFixedSize(MAIN_WIDTH, MAIN_HEIGHT);
     setMouseTracking(true);
     _key=-1;
}

void GLCanvas::setImage(const QImage& image)
{
    img = image;
}

void GLCanvas::updateImage(const QImage &image)
{
    setImage(image);
    update();
}

void GLCanvas::imshow(const cv::Mat &image)
{
    setImage(on_paper::utils::Mat2QImage(image));
    update();show();
}

void GLCanvas::register_mouseEvent_callbacks(std::string key, functor callback)
{
    this->mouseEvent_callbacks.insert(std::make_pair(key, callback));
}

void GLCanvas::remove_mouseEvent_callbacks(std::string key)
{
    this->mouseEvent_callbacks.erase(key);
}

void GLCanvas::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    fire_mouseEvents(event->pos());
}

void GLCanvas::keyPressEvent(QKeyEvent *event)
{
    event->accept();
    _key=event->key();
}

int GLCanvas::waitKey()
{
    while(_key == -1)
        QCoreApplication::processEvents();
    int key=_key; _key=-1;
    return key;
}

void GLCanvas::fire_mouseEvents(QPoint p)
{
    for(const auto& _cell :
        mouseEvent_callbacks)
        _cell.second(p.x(), p.y());
}



void GLCanvas::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    //Set the painter to use a smooth scaling algorithm.
    p.setRenderHint(QPainter::SmoothPixmapTransform, 1);

    p.drawImage(this->rect(), img);
}
