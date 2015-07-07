
#ifndef  ilp_h_INC
#define  ilp_h_INC
#include "ipp.h"

class Ilp
{
public:
    static const std::string ILP_ASCII;
    static const std::string ILP_SUFFIX;

    static gboolean is_ilp_filename(const std::string& fname);

private:
    static Ilp* _self;
    static display::State _state;
    static guint _nlayer, _nblock;

    static void displayfunc1();
    static void displayfunc2();
    static void reshapefunc(int width, int height);
    static void mousefunc(int button, int state, int x, int y);
    static void keyboardfunc(unsigned char key, int x, int y);
    static void specialfunc(int key, int x, int y);
    static void motionfunc(int x, int y);
    static void idlefunc();

public:
    typedef std::vector<Edge*>  block;
    typedef block::iterator        edge_iterator;
    typedef block::const_iterator  edge_const_iterator;

    typedef std::vector<block>     layer;
    typedef layer::iterator        block_iterator;
    typedef layer::const_iterator  block_const_iterator;

    typedef std::vector<layer>          container;
    typedef container::iterator         iterator;
    typedef container::const_iterator   const_iterator;

    Ilp(const Ncparams& np = Ncparams()): _np(np) {}
    virtual ~Ilp() {destroy();}

    Ilp& operator=(const Ilp& ilp);
    const layer& operator[](guint n) const {return _ilp[n];}
    bool is_empty() const {return _ilp.empty();}
    iterator begin() {return _ilp.begin();}
    iterator end() {return _ilp.end();}
    const_iterator begin() const {return _ilp.begin();}
    const_iterator end() const {return _ilp.end();}
    guint layer_number() const {return _ilp.size();}

    GtsBBox* gen_bbox(GtsBBoxClass* bbclass = gts_bbox_class()) const;

    void foreach_vertex(GtsFunc f, gpointer data) const;
    void foreach_vertex(GtsFunc f, gpointer data);
    void foreach_edge(GtsFunc f, gpointer data) const;
    void foreach_edge(GtsFunc f, gpointer data);

    void move(const Vector& mv){}
    void scale(const Vector& sv){}
    void rotate(const Vector& rv, gdouble angle){}

    void read(const std::string& fname);
    void read(std::istream& ins);

    void write(const std::string& fname) const;
    void write(std::ostream& out) const;

    void write_gcode(const std::string& fname) const;
    void write_gcode(std::ostream& out) const;
    void write_ilp_gcode(std::ostream& os, guint* nline) const;
    void write_layer_gcode(std::ostream& os,  guint* nline,
            const layer& bc, bool isback) const;

    void make_path(const Surface& s);

    void show(GLenum mode);

private:
    void destroy();

    container _ilp;
    Ncparams _np;
}; // -----  end of class Ilp  -----

#endif   // ----- #ifndef ilp_h_INC  -----
