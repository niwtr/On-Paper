
#include "OnPaper.h"

//TODO add HandDetector to namespace on_paper
using namespace on_paper;

#include <unistd.h>

int main(int argc, char **argv) {
    close(2);
    OnPaper op;
    op.init();
    op.main_loop();
}



