
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
    int refine = 1;
    string outfile = "out.ply";
    double radius = 1.0;
    struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"number", required_argument, NULL, 'n'},
        {"radius", required_argument, NULL, 'r'},
        {"output", required_argument, NULL, 'o'}
    };

    int c = 0;
    while (c != EOF) {
        int option_index = 0;
        c = getopt_long(argc, argv, "hr:n:o:", long_options, &option_index);
        switch (c) {
            case 'n':
                refine = atoi(optarg);
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'r':
                radius = atof(optarg);
                break;
            case 'h':
                cerr << "Usage: sphere [OPTION] file\n"
                    " -h    --help          show this infomation\n"
                    " -n N  --number=N      number of refine times. nfaces = 20 * 4^(N-1).\n"
                    " -r R  --radius=R      sphere radius.\n"
                    " -o O  --output=O      output file name.\n"
                    ;
                return 0;
            case '?':
                cerr << "Try `sphere --help` for more infomation.\n";
                return 1;
        }
    }

    Surface s;
    s.add_sphere(refine);
    s.scale(Vector(radius, radius, radius));
    s.write(outfile);
    return 0;
}

