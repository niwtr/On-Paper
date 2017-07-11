#include "pdfreader.h"
#include "cvutils.h"
#include <iostream>
#include <QPainter>

using std::cout;using std::endl;

on_paper::PDFReader::PDFReader()
{
    _is_loaded = false;
    pdf_path="";                                    //pdf文件的路径
    page_nums=0;
    magnification_coe=1;                          //放大系数
   // desk_piX=QApplication::desktop()->physicalDpiX();//所采像素密度
      // desk_piY=QApplication::desktop()->physicalDpiY();
    desk_piX=200;desk_piY=200;
    imageX=0;                                       //默认pdf渲染图片的位置
    imageY=0;
}

void on_paper::PDFReader::load_pdf(QString file_path)
{
     pdf_path=file_path;
     doc = Poppler::Document::load(pdf_path);
     if (!doc || doc->isLocked()) {
         // ... error message ....
         doc=NULL;
         return;
     }
     page_nums = doc->numPages();   //total page num
     Poppler::Page* pdfPage = doc->page(0);
     page_size = pdfPage->pageSize();    //per page size
     delete pdfPage;
     _is_loaded=true;
     return;
}

bool on_paper::PDFReader::render_pdf_page(int page_num)
{
    if(page_num<0||page_num>page_nums){
        return false;
    }
    else
    {
        if (!doc || doc->isLocked()) {
            // ... error message ....
            return false;
        }

        //设置反锯齿化 and transparent
        //doc->setPaperColor(Qt::transparent);
        doc->setRenderHint(Poppler::Document::TextAntialiasing);
        //doc->setRenderHint(Poppler::Document::Antialiasing);
        doc->setRenderHint(Poppler::Document::IgnorePaperColor);
        doc->setRenderBackend((Poppler::Document::RenderBackend)0);//splash
        //doc->setRenderBackend(Poppler::Document::ArthurBackend);

        //获取页码相应的pdf
        Poppler::Page* pdfPage = doc->page(page_num);
        if (pdfPage == 0) {
            // ... error message ...
            return false;
        }


        //生成pdf渲染图像
        image = pdfPage->renderToImage(desk_piX*magnification_coe, desk_piY*magnification_coe, -1, -1, -1, -1) ;

        page_size=pdfPage->pageSize();
        // after the usage, the page must be deleted
        delete pdfPage;

        if (!image.isNull()) {
            cur_page=page_num;
            return true;
        }
        else
            return false;
    }
}

cv::Mat on_paper::PDFReader::cv_get_pdf_image()
{
    auto img = get_pdf_image();
    return utils::QImage2Mat(img);
}

void on_paper::PDFReader::magnification()
{
    magnification_coe+=0.1;
    render_pdf_page(cur_page);
}

void on_paper::PDFReader::shrink()
{
    if(magnification_coe>0.2)
    {
        magnification_coe-=0.1;
        render_pdf_page(cur_page);
    }
    return;
}

void on_paper::PDFReader::move_page(int movex, int movey)
{
    imageX+=movex;
    imageY+=movey;
}
