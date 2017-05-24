
#include "OnPaper.h"

//TODO add HandDetector to namespace on_paper
using namespace on_paper;


int main(int argc, char **argv) {
    OnPaper op;
    op.init();
    op.main_loop();
}




//Mat readPDFtoCV(const string& filename,int DPI) {
//    poppler::document* mypdf = poppler::document::load_from_file(filename);
//    if(mypdf == NULL) {
//        cerr << "couldn't read pdf\n";
//        return Mat();
//    }
//    cout << "pdf has " << mypdf->pages() << " pages\n";
//    poppler::page* mypage = mypdf->create_page(0);
//
//    poppler::page_renderer renderer;
//    renderer.set_render_hint(poppler::page_renderer::text_antialiasing);
//    poppler::image myimage = renderer.render_page(mypage,DPI,DPI);
//    cout << "created image of  " << myimage.width() << "x"<< myimage.height() << "\n";
//
//    Mat cvimg;
//    if(myimage.format() == poppler::image::format_rgb24) {
//        Mat(myimage.height(),myimage.width(),CV_8UC3,myimage.data()).copyTo(cvimg);
//    } else if(myimage.format() == poppler::image::format_argb32) {
//        Mat(myimage.height(),myimage.width(),CV_8UC4,myimage.data()).copyTo(cvimg);
//    } else {
//        cerr << "PDF format no good\n";
//        return Mat();
//    }
//    return cvimg;
//}
