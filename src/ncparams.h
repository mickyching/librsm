
#ifndef  ncparams_INC
#define  ncparams_INC
#include <string>
#include <gts.h>

struct Ncparams {
    enum Enums {
        NONE = 0,
        RECTANGLE = 1, ELLIPES = 1 << 1,
        ISO_LAYER = 1, ISO_PLANAR = 1 << 1,
        LAYER_PLANAR = ISO_LAYER|ISO_PLANAR};
public:
    Ncparams();
    Ncparams(const std::string& filename);
    void read(const std::string& fname);
    void read(std::istream& is);
    void write(const std::string& fname) const;
    void write(std::ostream& os) const;
    gboolean check() const;

    gdouble xmin, ymin, zmin, xmax, ymax, zmax;  // 加工总区间
    gdouble ipprize, ippside, ippfeed, ippzmin, ippzmax;  // 截面法加工参数
    gdouble ilprize, ilpside, ilpfeed, ilpzmin, ilpzmax;  // 层面法加工参数
    gdouble ball_radius;                         // 道具半径
    gdouble rise_height;                         // 抬刀高度
    gint method;                                 // 加工方法
    gint figure;                                 // 加工轮廓

    static const std::string NC_FORMAT, NC_CFG_FORMAT;
    static const std::string NC_CFG_NAME, NC_CFG_SUFFIX;
    static const std::string NC_NAME;
    static const std::string NC_SUFFIX;
};

#endif   // ----- #ifndef ncparams_INC  -----
