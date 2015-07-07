#include "remc.h"
#include <getopt.h>
#include <unistd.h>
#include <iomanip>

using namespace std;
int main(int argc, char** argv)
{
    option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"info", no_argument, NULL, 'i'},
        {"ofile", required_argument, NULL, 'o'}
    };

    string ifile = argv[argc-1], ofile;
    bool is_info = false;
    int c = 0;
    while (c != EOF) {
        int i = 0;
        c = getopt_long(argc, argv, "hi:o:", options, &i);
        switch (c) {
            case 'i':
                is_info = true;
                break;
            case 'o':
                ofile = optarg;
                break;
            case 'h':
                cerr << "Usage: iotime [OPTION] file\n"
                    " -h    --help          show this information.\n"
                    " -i    --info          show mesh info.\n"
                    " -o    --ofile         file for output.\n"
                    ;
                return 0;
            case '?':
                cerr << "Try `iotime --help` for more information.\n";
                return 1;
        }
    }

    GTimer* timer = g_timer_new();
    g_timer_start(timer);
    Surface sf;
    if (!ifile.empty()) sf.read(ifile);
    g_timer_stop(timer);
    if (is_info) sf.write_info(cout);
    else sf.write_number(cout);
    cout << "time(sec) read/write        |"
        << std::setw(8) << g_timer_elapsed(timer, NULL);

    g_timer_start(timer);
    if (!ofile.empty()) sf.write(ofile);
    g_timer_stop(timer);
    cout << std::setw(8) << g_timer_elapsed(timer, NULL) << endl;
    g_timer_destroy(timer);
}
