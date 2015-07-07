#include <unistd.h>
#include <getopt.h>
#include <iomanip>
#include "remc.h"
using namespace std;

int main(int argc, char** argv)
{

    option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"info", no_argument, NULL, 'i'},
        {"ofile", required_argument, NULL, 'o'},
        {"dist", required_argument, NULL, 'd'},
        {"type", required_argument, NULL, 't'}
    };

    gdouble dist = 0.0;
    string ifile = argv[argc-1], ofile;
    bool is_info = false;
    int type = 0;
    int c = 0;
    while (c != EOF) {
        int i = 0;
        c = getopt_long(argc, argv, "hi:o:d:t:", options, &i);
        switch (c) {
            case 'i':
                is_info = true;
                break;
            case 'o':
                ofile = optarg;
                break;
            case 'd':
                dist = atof(optarg);
                break;
            case 't':
                type = atoi(optarg);
                break;
            case 'h':
                cerr << "Usage: offset [OPTIONS]\n"
                    "-h     --help          show this infomation\n"
                    "-i     --info          show mesh infomation\n"
                    "-o O   --ofile=O       file for output\n"
                    "-t T   --type=T        offset type(0/1/2)\n"
                    "-d D   --dist=D        distance for offset\n"
                    ;
                return 0;
            case '?':
                cerr << "Try `offset --help` for more information\n";
                return 1;
        }
    }

    Surface sf;
    if (!ifile.empty()) sf.read(ifile);
    GTimer* timer = g_timer_new();
    g_timer_start(timer);
    if (dist > 0.) {
        switch (type) {
            case 0:
                sf.offset(dist);
                break;
            case 1:
                sf.offset1(dist);
                break;
            case 2:
                sf.offset2(dist);
                break;
        }
    }
    g_timer_stop(timer);
    cout << "offset ";
    if (is_info) sf.write_info(cout);
    else sf.write_number(cout);
    cout << "time(sec) offset            |"
        << setw(10) << g_timer_elapsed(timer, NULL) << endl;
    if (!ofile.empty()) sf.write(ofile);
}
