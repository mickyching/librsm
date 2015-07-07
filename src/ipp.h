
#ifndef  ipp_h_INC
#define  ipp_h_INC

#include <list>
#include "surface.h"
#include "ncparams.h"
#include <vector>

class Ipp
{
public:
    static const std::string IPP_ASCII;
    static const std::string IPP_SUFFIX;

    static gboolean is_ipp_filename(const std::string& fname);

private:
    static Ipp* _self;
    static display::State _state;

    static void displayfunc();
    static void reshapefunc(int width, int height);
    static void mousefunc(int button, int state, int x, int y);
    static void keyboardfunc(unsigned char key, int x, int y);
    static void specialfunc(int key, int x, int y);
    static void motionfunc(int x, int y);
    static void idlefunc();

public:
    typedef std::list<Vertex*>          planar;
    typedef std::vector<planar>         container;
    typedef container::iterator         iterator;
    typedef container::const_iterator   const_iterator;

    Ipp(const Ncparams& np = Ncparams()): _np(np) {}
    virtual ~Ipp() {destroy();}

    bool is_empty() const {return _ipp.empty();}
    iterator begin() {return _ipp.begin();}
    iterator end() {return _ipp.end();}
    const_iterator begin() const {return _ipp.begin();}
    const_iterator end() const {return _ipp.end();}
    guint planar_number() const {return _ipp.size();}
    gdouble side_interval() const;

    GtsBBox* gen_bbox(GtsBBoxClass* bbclass = gts_bbox_class()) const;

    void foreach_vertex(GtsFunc f, gpointer data) const;
    void foreach_vertex(GtsFunc f, gpointer data);

    void scale(const Vector& s){};
    void move(const Vector& mv){};
    void rotate(const Vector& rv, gdouble angle){};

    void read(const std::string& fname);
    void read(std::istream& is);

    void write(const std::string& fname) const;
    void write(std::ostream& os) const;
    void write_gcode(const std::string& fname) const;
    void write_gcode(std::ostream& os) const;

    void make_path(const Surface& s);

    void gen_lines_list(display::State& state);
    void show(GLenum mode);

private:
    void write_ipp_gcode(std::ostream& os, guint* nline) const;
    void add_rectangle_border(VertexClass* vclass, EdgeClass* eclass);
    void add_ellipse_border(VertexClass* vclass, EdgeClass* eclass);
    void destroy();

    container _ipp;
    Ncparams _np;
}; // -----  end of class Ipp  -----

#endif   // ----- #ifndef ipp_h_INC  -----
