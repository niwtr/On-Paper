#ifndef GLCANVAS_H
#define GLCANVAS_H

#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QPainter>
#include <functional>
#include <unordered_map>
#include <string>
#include <QMouseEvent>
#include <QCoreApplication>
#include <opencv2/core.hpp>
#include <cvutils.h>
using std::string;
using functor=std::function<void(int x, int y)>; //function for mouse event.
using Hash = std::unordered_map<string, functor>;
class GLCanvas : public QOpenGLWidget{
public:
    GLCanvas(QWidget* parent = NULL);
    void setImage(const QImage& image);
    void updateImage(const QImage& image);
    void imshow(const cv::Mat& image);//for guys familiar with cv.
    inline void imshow_resized(const cv::Mat& image){ setFixedSize(image.cols, image.rows); this->imshow(image);}
    void register_mouseEvent_callbacks(string key, functor callback);
    void remove_mouseEvent_callbacks(string key);
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;
    int waitKey(void);
protected:
    void paintEvent(QPaintEvent*);
private:
    QImage img;
    int _key;
    Hash mouseEvent_callbacks;
    void fire_mouseEvents(QPoint p);
};


#endif // GLCANVAS_H
