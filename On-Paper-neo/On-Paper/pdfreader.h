#ifndef PDFREADER_H
#define PDFREADER_H

#include <QString>
#include <QSize>
#include <QImage>
#include "poppler-qt5.h"
#include <opencv/cv.hpp>
#include <QApplication>
#include <QWidget>
#include <QDesktopWidget>
namespace on_paper {
class PDFReader
{
public:
    PDFReader();
    bool is_loaded(){ return _is_loaded;}
    void load_pdf(QString file_path);
    bool render_pdf_page(int page_num);
    inline QImage& get_pdf_image(){ return image; }
    cv::Mat cv_get_pdf_image();
    void magnification(); //enlarge
    void shrink();
    void move_page(int movex,int movey);
    int get_pagenum(){return page_nums;}
private:
    QString pdf_path;
    QSize page_size;
    Poppler::Document *doc;
    QImage image;
    int page_nums;
    int cur_page;

    float magnification_coe;
    int desk_piX;
    int desk_piY;

    int imageX;
    int imageY;
    bool _is_loaded;


};

}
#endif // PDFREADER_H
