
#ifndef  simstr_INC
#define  simstr_INC
#include <string>
#include <iostream>
#include <sstream>
#include <gts.h>

const char RE_COMMENT = '#';

#define re_warning (std::cerr << __FILE__ << __LINE__ << " warning: ")
#define re_error (std::cerr << __FILE__ << __LINE__ << " error: ")
#define re_log (std::clog << __FILE__ << __LINE__ << "log: ")

inline bool is_comment_line(const std::string& line, char comment = RE_COMMENT)
{
    for (std::string::const_iterator i = line.begin(); i != line.end(); ++i) {
        if (*i == comment) return TRUE;
        else if (*i == ' ' || *i == '\t') continue;
        else return FALSE;
    }
    return TRUE;
}

inline bool get_valid_line(std::istream& is, std::string& line)
{
    while (getline(is, line)) {
        if (is_comment_line(line)) continue;
        return TRUE;
    }
    return FALSE;
}

inline bool get_ssline(std::istream& is, std::stringstream& ssline)
{
    std::string line;
    get_valid_line(is, line);
    ssline.clear();
    ssline.str(line);
}

inline std::ostream& write_G0(std::ostream& os, guint* nline)
{
    return os << "N" << ++(*nline) << " G00 ";
}

inline std::ostream& write_G1(std::ostream& os, guint* nline,
        gdouble x, gdouble y, gdouble z)
{
    return os << "N" << ++(*nline) << " G01 "
        << "X" << x << " Y" << y << " Z" << z << "\n";
}

#endif   // ----- #ifndef simstr_INC  -----
