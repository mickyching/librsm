#include "remc.h"
#include "display.h"
#include <getopt.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;
using namespace display;

int main(int argc, char** argv)
{

    if (!setlocale(LC_ALL, "POSIX")) {
        re_warning << "can not set locale to POSIX\n";
    }

    gboolean draw_point = FALSE;
    gboolean draw_line = FALSE;
    gboolean draw_triangle = FALSE;

    string filename(argv[argc-1]);

    option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"point", no_argument, NULL, 'p'},
        {"line", no_argument, NULL, 'l'},
        {"triangle", no_argument, NULL, 't'}
    };

    int c = 0;
    while (c != EOF) {
        int option_index = 0;
        c = getopt_long(argc, argv, "hplt", long_options, &option_index);
        switch (c) {
            case 'p': draw_point = TRUE; break;
            case 'l': draw_line  = TRUE; break;
            case 't': draw_triangle = TRUE; break;
            case 'h':
                cerr << "Usage: view [OPTION] filename\n"
                    " -h    --help          show this infomation\n"
                    " -p    --point         draw points\n"
                    " -l    --line          draw lines\n"
                    " -t    --triangle      draw triangles\n"
                    ;
                    return 0;
            case '?':
                cerr << "Try `view --help` for more information\n";
                return 1;
        }
    }

    if (Surface::is_surface_filename(filename)) {
        Surface s;
        s.read(filename);
        int mode = 0;
        if (draw_point) mode |= POINTS;
        if (draw_line) mode |= LINES;
        if (draw_triangle) mode |= TRIANGLES;
        s.show(mode);
    }

    if (Ipc::is_ipc_filename(filename)) {
        Ipc ipc;
        ipc.read(filename);
        int mode = 0;
        if (draw_point) mode |= POINTS;
        if (draw_line) mode |= LINES;
        if (draw_triangle) mode |= TRIANGLES;
        ipc.show(mode);
    }

    if (Ipp::is_ipp_filename(filename)) {
        Ipp ipp;
        ipp.read(filename);
        int mode = 0;
        if (draw_point) mode |= POINTS;
        if (draw_line) mode |= LINES;
        if (draw_triangle) mode |= TRIANGLES;
        ipp.show(mode);
    }

    if (Ilp::is_ilp_filename(filename)) {
        Ilp ilp;
        ilp.read(filename);
        int mode = 0;
        if (draw_point) mode |= POINTS;
        if (draw_line) mode |= LINES;
        if (draw_triangle) mode |= TRIANGLES;
        ilp.show(mode);
    }
}
