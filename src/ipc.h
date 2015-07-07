
#ifndef  ipc_INC
#define  ipc_INC

#include <string>
#include <iostream>
#include <vector>
#include "surface.h"
#include "ncparams.h"

class Ipc
{
public:
    static const std::string IPC_ASCII;
    static const std::string IPC_SUFFIX;

    static gboolean is_ipc_filename(const std::string& fname);

private:
    static Ipc* _self;
    static display::State _state;

    static void displayfunc();
    static void reshapefunc(int width, int height);
    static void mousefunc(int button, int state, int x, int y);
    static void keyboardfunc(unsigned char key, int x, int y);
    static void specialfunc(int key, int x, int y);
    static void motionfunc(int x, int y);
    static void idlefunc();

public:
    typedef std::vector<GSList*>        container;
    typedef container::iterator         iterator;
    typedef container::const_iterator   const_iterator;

    Ipc(const Ncparams& np = Ncparams()): _np(np) {}
    virtual ~Ipc() {destroy();}

    iterator begin() {return _ipc.begin();}
    iterator end() {return _ipc.end();}
    const_iterator begin() const {return _ipc.begin();}
    const_iterator end() const {return _ipc.end();}
    guint planar_number() const {return _ipc.size();}
    gdouble side_interval() const;

    GtsBBox* gen_bbox(GtsBBoxClass* bbclass = gts_bbox_class()) const;

    void foreach_vertex(GtsFunc f, gpointer data) const;
    void foreach_vertex(GtsFunc f, gpointer data);
    void foreach_edge(GtsFunc f, gpointer data) const;
    void foreach_edge(GtsFunc f, gpointer data);

    void scale(const Vector& s){};
    void move(const Vector& mv){};
    void rotate(const Vector& rv, gdouble angle){};

    void read(const std::string& fname);
    void read(std::istream& is);

    void write(const std::string& fname) const;
    void write(std::ostream& os) const;

    void make_curves(const Surface& s);

    void show(GLenum mode);

private:
    void destroy();

    container _ipc;
    Ncparams _np;
};
#endif   /* ----- #ifndef ipc_INC  ----- */
