
#include "remc.h"
#include <getopt.h>
#include <unistd.h>
#include <iomanip>
using namespace std;


int main(int argc, char** argv)
{
    if (!setlocale(LC_ALL, "POSIX")) {
        re_warning << "can not set locale to POSIX\n";
    }

    Qem_params qparams;
    Fme_params fparams;

    gboolean memless = FALSE;
    bool is_info = false;

    string outfile = "out.ply";

    struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"info", no_argument, NULL, 'i'},
        {"verbose", no_argument, NULL, 'v'},
        {"memless", no_argument, NULL, 'm'},
        {"union", no_argument, NULL, 'U'},
        {"uniform", no_argument, NULL, 'u'},
        {"edges", required_argument, NULL, 'e'},
        {"output", required_argument, NULL, 'o'}
    };

    int c = 0;
    while (c != EOF) {
        int option_index = 0;
        c = getopt_long(argc, argv, "hi:vUe:mcuo:", long_options, &option_index);
        switch (c) {
            case 'i':
                is_info = true;
                break;
            case 'u':
                qparams.is_uniform = TRUE;
                break;
            case 'U':
                qparams.is_union = TRUE;
                break;
            case 'v':
                qparams.is_verbose = TRUE;
                fparams.is_verbose = TRUE;
                break;
            case 'e':
                qparams.nedge = atoi(optarg);
                fparams.nedge = atoi(optarg);
                break;
            case 'm':
                memless = TRUE;
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'h':
                cerr << "Usage: simplify [OPTION] filename\n"
                    " -h    --help          show this infomation\n"
                    " -m    --memless       use memless method to simplify.\n"
                    " -v    --verbose       simplify verbose.\n"
                    " -u    --uniform       using uniform quadric.\n"
                    " -U    --union         using union instead of add.\n"
                    " -e E  --edges=E       simplify tartget edge number.\n"
                    " -o O  --output=O      output file name.\n"
                    ;
                return 0;
            case '?':
                cerr << "Try `simplify --help` for more information.\n";
                return 1;
        }
    }

    Surface s;
    s.read(argv[argc-1]);
    GTimer* timer = g_timer_new();

    g_timer_start(timer);
    if (memless) s.fme_coarsen(fparams);
    else s.qem_coarsen(qparams);
    g_timer_stop(timer);

    cout << "simplified ";
    if (is_info) s.write_info(cout);
    else s.write_number(cout);
    cout << "time(sec) simplify cost     |"
        << setw(10) << g_timer_elapsed(timer, NULL) << endl;

    s.write(outfile);
}
