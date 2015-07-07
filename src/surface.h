
#ifndef  surface_h_INC
#define  surface_h_INC

#include "vertex.h"
#include "edge.h"
#include "face.h"
#include "display.h"

typedef Vertex* (*SubvFunc) (GtsEHeap* heap, Edge* edge, gpointer data);
typedef GtsKeyFunc CostFunc;
typedef GtsStopFunc StopFunc;

class Qem_params
{
public:
    Qem_params(): border_weight(3.0),
                  nedge(0),
                  is_verbose(FALSE),
                  is_uniform(FALSE),
                  is_union(FALSE)
    {}
    gdouble border_weight;
    guint nedge;
    gboolean is_verbose;
    gboolean is_uniform;
    gboolean is_union;
};

class Fme_params: public GtsVolumeOptimizedParams
{
public:
    Fme_params(): nedge(0), is_verbose(FALSE)
    {
        volume_weight = 0.5;
        boundary_weight = 0.5;
        shape_weight = 0.0;
    }
    guint nedge;
    gboolean is_verbose;
};

class Surface
{
public:
    static const std::string PLY_ASCII;
    static const std::string PLY_NAME;
    static const std::string PLY_SUFFIX;
    static const std::string PLY_COMMENT;
    static const std::string PLY_ELEMENT_VERTEX;
    static const std::string PLY_ELEMENT_FACE;
    static const std::string PLY_END_HEADER;

    static const std::string GTS_ASCII;
    static const std::string GTS_SUFFIX;

    static gboolean is_surface_filename(const std::string& fname);
    static gboolean is_ply_filename(const std::string& fname);
    static gboolean is_gts_filename(const std::string& fname);

private:
    static Surface* _self;
    static display::State _state;

    static void displayfunc();
    static void reshapefunc(int width, int height);
    static void mousefunc(int button, int state, int x, int y);
    static void keyboardfunc(unsigned char key, int x, int y);
    static void specialfunc(int key, int x, int y);
    static void motionfunc(int x, int y);
    static void idlefunc();

public:
    Surface(VertexClass* vc = vertex_class(),
            EdgeClass* ec = edge_class(),
            FaceClass* fc = face_class());
    Surface(const Surface& s);
    ~Surface() {destroy();}

    Surface& operator=(const Surface& s);

    void add_face(Face* face);
    void add_sphere(gint order = 1);
    void add_square(gdouble len = 1.0);
    void add_rectangle(gdouble x0, gdouble y0, gdouble z0,
                        gdouble x1, gdouble y1, gdouble z1,
                        gdouble x2, gdouble y2, gdouble z2,
                        gdouble x3, gdouble y3, gdouble z3);

    void foreach_vertex(GtsFunc func, gpointer data) const;
    void foreach_vertex(GtsFunc func, gpointer data);
    void foreach_edge(GtsFunc func, gpointer data) const;
    void foreach_edge(GtsFunc func, gpointer data);
    void foreach_face(GtsFunc func, gpointer data) const;
    void foreach_face(GtsFunc func, gpointer data);

    void vertex_duplicate_remove();
    void edge_duplicate_remove();
    void face_duplicate_remove();
    void face_downward_remove();
    void face_split();
    void face_offset(gdouble d);

    void reverse();
    void move(const Vector& mv);
    void scale(const Vector& sv);
    void rotate(const Vector& rv, gdouble angle);
    void transform(GtsMatrix* tm);
    void offset(gdouble d);
    void offset1(gdouble d);
    void offset2(gdouble d);

    void read(const std::string& fname);
    void read(std::istream& is, const std::string& fformat);

    void write(const std::string& fname) const;
    void write(std::ostream& os, const std::string& fformat) const;

    void write_number(std::ostream& os) const;
    void write_info(std::ostream& os) const;

    VertexClass* get_vclass() const {return _vertex_class;}
    EdgeClass* get_eclass() const {return _edge_class;}
    FaceClass* get_fclass() const {return _face_class;}
    void get_stats(GtsSurfaceStats& stats) const;
    void get_qstats(GtsSurfaceQualityStats& qstats) const;

    GtsBBox* gen_bbox(GtsBBoxClass* bbclass = gts_bbox_class()) const;
    GNode* gen_bbtree() const;
    GSList* gen_intersection(const Surface& s, GNode *t1, GNode *t2) const;

    void edge_collapse(guint nedge, gpointer params,
                       SubvFunc fsubv, CostFunc fcost, StopFunc fstop);

    void qem_coarsen(Qem_params params = Qem_params());
    void fme_coarsen(Fme_params params = Fme_params());

    void show(GLenum mode);

private:
    void read_gts(std::istream& is);
    void read_ply(std::istream& is);
    void write_gts(std::ostream& os) const;
    void write_ply(std::ostream& os) const;
    void destroy();

    GtsSurface* _surface;
    VertexClass* _vertex_class;
    EdgeClass* _edge_class;
    FaceClass* _face_class;
};
#endif   // ----- #ifndef surface_INC  -----
