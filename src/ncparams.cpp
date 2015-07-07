#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "simstr.h"
#include "ncparams.h"

const std::string Ncparams::NC_FORMAT              = "nc format";
const std::string Ncparams::NC_CFG_FORMAT          = "nc cfg format";
const std::string Ncparams::NC_SUFFIX              = ".nc";

const std::string Ncparams::NC_NAME                = "nc";
const std::string Ncparams::NC_CFG_NAME            = "nc cfg";
const std::string Ncparams::NC_CFG_SUFFIX          = ".cfg";

Ncparams::Ncparams():
    xmin(0.), xmax(0.), ymin(0.), ymax(0.), zmin(0.), zmax(0.),
    ipprize(0.), ippside(0.), ippfeed(0.), ippzmin(0.), ippzmax(0.),
    ilprize(0.), ilpside(0.), ilpfeed(0.), ilpzmin(0.), ilpzmax(0.),
    ball_radius(0.0), rise_height(0.0),
    method(ISO_LAYER),
    figure(NONE)
{}

Ncparams::Ncparams(const std::string& filename)
{
    std::ifstream fin(filename.c_str());
    read(fin);
    fin.close();
}

void Ncparams::read(const std::string& fname)
{
    std::ifstream fin(fname.c_str());
    read(fin);
    fin.close();
}
void Ncparams::read(std::istream& is)
{
    std::string line;
    while (getline(is, line)) {
        if (is_comment_line(line)) continue;
        g_return_if_fail(line == Ncparams::NC_CFG_NAME);
        break;
    }
#define get(item) \
{ \
    while (getline(is, line)) { \
        if (is_comment_line(line)) continue; \
        std::stringstream ss(line); \
        char eq; \
        ss >> line >> eq >> item; \
        g_return_if_fail(line == #item); \
        break; \
    } \
}
    get(method);
    get(figure);
    get(ball_radius);
    get(rise_height);
    get(xmin); get(ymin); get(zmin);
    get(xmax); get(ymax); get(zmax);
    get(ipprize); get(ippside); get(ippfeed); get(ippzmin); get(ippzmax);
    get(ilprize); get(ilpside); get(ilpfeed); get(ilpzmin); get(ilpzmax);
}

void Ncparams::write(const std::string& fname) const
{
    std::ofstream fout(fname.c_str());
    write(fout);
    fout.close();
}

void Ncparams::write(std::ostream& os) const
{
    os << Ncparams::NC_CFG_NAME << "\n";
#define out(item) \
{ \
    os << #item << " = " << item << "\n"; \
}
    os << "# --------------------------------------------------------------\n";
    os << "# method: ISO_LAYER(" << ISO_LAYER
        << "), ISO_PLANAR(" << ISO_PLANAR
        << "), LAYER_PLANAR(" << LAYER_PLANAR << ")\n";
    os << "# figure: NONE(" << NONE
        << "), RECTANGLE(" << RECTANGLE
        << "), ELLIPES(" << ELLIPES << ")\n";
    out(method);
    out(figure);
    os << "# --------------------------------------------------------------\n";
    os << "# milling tool parameters.\n";
    out(ball_radius);
    out(rise_height);
    os << "# --------------------------------------------------------------\n";
    os << "# planar intersection parameters.\n";
    out(xmin); out(ymin); out(zmin);
    out(xmax); out(ymax); out(zmax);
    os << "# --------------------------------------------------------------\n";
    os << "# iso planar path milling parameters.\n";
    out(ipprize); out(ippside); out(ippfeed); out(ippzmin); out(ippzmax);
    os << "# --------------------------------------------------------------\n";
    os << "# iso layer path milling parameters.\n";
    out(ilprize); out(ilpside); out(ilpfeed); out(ilpzmin); out(ilpzmax);
}

gboolean Ncparams::check() const
{
#define isok(c) \
{ \
    if (c) { \
        std::cerr << "[Ok]:   " << (#c) << "\n"; \
    } \
    else { \
        std::cerr << "[Error]:" << (#c) << "\n"; \
        return FALSE; \
    } \
}
    isok(method == ISO_PLANAR || method == ISO_LAYER || method == LAYER_PLANAR);
    isok(figure == NONE || figure == RECTANGLE || figure == ELLIPES);
    isok(ball_radius >= 0.0);
    isok(rise_height >= 0.0);
    isok(xmax >= xmin && ymax >= ymin && zmax >= zmin);

    isok(ippzmax >= ippzmin && ippzmin >= zmin);
    isok(ippfeed >= 0.0 && ippfeed <= 1.0);
    isok(ball_radius == 0 || ippside >= 0.0 && ippside < 2 * ball_radius);

    isok(ilpzmax >= ilpzmin && ilpzmin >= zmin);
    isok(ilpfeed >= 0.0 && ilpfeed <= 2.0);
    isok(ball_radius == 0 || ilpside >= 0.0 && ilpside < 2 * ball_radius);

    isok(ipprize >= 0 && ilprize >= 0)
    return TRUE;
#undef isok
}
