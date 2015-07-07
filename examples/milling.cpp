#include "remc.h"
#include <cstdlib>
#include <getopt.h>
#include <unistd.h>
using namespace std;

int main(int argc, char** argv)
{
    if (!setlocale(LC_ALL, "POSIX")) {
        re_warning << "can not set locale to POSIX\n";
    }

    string cfgfile, segfile, ippfile, ilpfile, ncfile;
    string omeshfile;
    Ncparams np;

    struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"cfgfile", required_argument, NULL, 'c'},
        {"template", no_argument, NULL, 't'},
        {"segfile", required_argument, NULL, 's'},
        {"ippfile", required_argument, NULL, 'p'},
        {"ilpfile", required_argument, NULL, 'l'},
        {"gcode", required_argument, NULL, 'g'},
        {"offset", required_argument, NULL, 'o'}
    };
    int c = 0;
    while (c != EOF) {
        int option_index = 0;
        c = getopt_long(argc, argv, "hc:ts:p:l:g:o:", long_options, &option_index);
        switch (c) {
            case 'c': cfgfile = optarg; break;
            case 's': segfile = optarg; break;
            case 'p': ippfile = optarg; break;
            case 'l': ilpfile = optarg; break;
            case 'g': ncfile = optarg; break;
            case 'o': omeshfile = optarg; break;
            case 't': np.write("template.cfg");
            case 'h':
                cerr << "Usage: milling [OPTION] filename\n"
                    " -h    --help          show this infomation\n"
                    " -c C  --cfgfile=C     NC config input file for milling.\n"
                    " -t    --template      create default template config file.\n"
                    " -s S  --segfile=S     original segments tool path file.\n"
                    " -p P  --ippfile=P     ipp tool path output file.\n"
                    " -l L  --ilpfile=L     ilp tool path output file.\n"
                    " -g G  --gcode=N       NC code output file.\n"
                    " -o O  --offset=M      the surface offset output file.\n"
                    ;
                return 0;
            case '?': cerr << "Try `milling --help` for more information.\n";
                return 1;
        }
    }

    Surface s;
    s.read(argv[argc-1]);
    s.write_info(cout);

    np.read(cfgfile);
    np.write(cerr);                             // log
    g_return_val_if_fail(np.check(), 1);        // log

    if (np.ball_radius > 0.0) {
        Vector mv(0, 0, -np.ball_radius);
        s.move(mv);

        GTimer* off_time = g_timer_new();
        g_timer_start(off_time);
        s.offset(np.ball_radius);
        g_timer_stop(off_time);
        cout << "Offset Time: " << g_timer_elapsed(off_time, NULL) << endl;;
    }

    if (!omeshfile.empty()) s.write(omeshfile);

    s.face_downward_remove();

    GTimer* timer = g_timer_new();
    if (np.method == Ncparams::ISO_PLANAR) {
        if (!segfile.empty()) {
            Ipc ipc(np);
            ipc.make_curves(s);
            ipc.write(segfile);
        }

        g_timer_start(timer);
        Ipp ipp(np);
        ipp.make_path(s);
        g_timer_stop(timer);
        cout << "IPP Total: " << g_timer_elapsed(timer, NULL) << endl;;

        if (!ippfile.empty()) ipp.write(ippfile);
        if (!ncfile.empty()) ipp.write_gcode(ncfile);
    }
    else if (np.method == Ncparams::ISO_LAYER) {
        if (!segfile.empty()) {
            Ipc ipc(np);
            ipc.make_curves(s);
            ipc.write(segfile);
        }

        if (!ippfile.empty()) {
            Ipp ipp(np);
            ipp.make_path(s);
            ipp.write(ippfile);
        }

        g_timer_start(timer);
        Ilp ilp(np);
        ilp.make_path(s);
        g_timer_stop(timer);
        cout << "ILP Total: " << g_timer_elapsed(timer, NULL) << endl;;

        if (!ilpfile.empty()) ilp.write(ilpfile);
        if (!ncfile.empty()) ilp.write_gcode(ncfile);
    }
    return 0;
}
