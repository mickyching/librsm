
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstring>
#include "config.h"
#include "simstr.h"
#include "quadric.h"
#include "surface.h"

const std::string Surface::PLY_ASCII              = "format ascii 1.0";
const std::string Surface::PLY_NAME               = "ply";
const std::string Surface::PLY_SUFFIX             = ".ply";
const std::string Surface::PLY_COMMENT            = "comment";
const std::string Surface::PLY_ELEMENT_VERTEX     = "element vertex";
const std::string Surface::PLY_ELEMENT_FACE       = "element face";
const std::string Surface::PLY_END_HEADER         = "end_header";

const std::string Surface::GTS_ASCII              = "gts ascii";
const std::string Surface::GTS_SUFFIX             = ".gts";

Surface* Surface::_self = NULL;
display::State Surface::_state;


gboolean Surface::is_surface_filename(const std::string& fname)
{
    return (is_gts_filename(fname) || is_ply_filename(fname));
}


gboolean Surface::is_ply_filename(const std::string& fname)
{
    return (fname.rfind(PLY_SUFFIX) == fname.length() - PLY_SUFFIX.length());
}


gboolean Surface::is_gts_filename(const std::string& fname)
{
    return (fname.rfind(GTS_SUFFIX) == fname.length() - GTS_SUFFIX.length());
}


void Surface::displayfunc()
{
    display::display(_state);
}


void Surface::reshapefunc(int width, int height)
{
    display::reshape(_state, width, height);
}


void Surface::mousefunc(int button, int state, int x, int y)
{
    display::mouse(_state, button, state, x, y);
}


void Surface::keyboardfunc(unsigned char key, int x, int y)
{
#define endcase \
    if (_state.draw_mode & display::POINTS)     list_points(_state, *_self); \
    if (_state.draw_mode & display::LINES)      list_lines(_state, *_self); \
    if (_state.draw_mode & display::TRIANGLES)  list_triangles(_state, *_self); \
    glutPostRedisplay();break
    int mod = glutGetModifiers();
    std::string fname;
    if (mod & GLUT_ACTIVE_ALT) {
        switch (key) {
            case 'm':
                std::cout << "Input Move Vector: ";
                std::cin >> _state.model_move;
                _self->move(_state.model_move);
                endcase;
            case 's':
                std::cout << "Input Scale Vector: ";
                std::cin >> _state.model_scale;
                _self->scale(_state.model_scale);
                endcase;
            case 'r':
                std::cout << "Input Rotate Vector: ";
                std::cin >> _state.model_rotate;
                std::cout << "Input Rotate Angle: ";
                std::cin >> _state.model_rangle;
                _self->rotate(_state.model_rotate, _state.model_rangle);
                endcase;
            case 'R':
                std::cout << "Reverse surface..." << std::flush;
                _self->reverse();
                std::cout << "done!" << std::endl;
                endcase;
            case 'S':
                std::cout << "save surface as(input file name): " << std::flush;
                std::cin >> fname;
                _self->write(fname);
                std::cout << "done!" << std::endl;
                break;
        }
    }
    else {
        display::keyboard(_state, key, x, y);
    }
#undef endcase
}


void Surface::specialfunc(int key, int x, int y)
{
    display::special(_state, key, x, y);
    switch (key) {
        case GLUT_KEY_F1:
            _self->write_info(std::cout);
            break;
    }
}


void Surface::motionfunc(int x, int y)
{
    display::motion(_state, x, y);
}


Surface::Surface(VertexClass* vc, EdgeClass* ec, FaceClass* fc)
:   _surface(NULL), _vertex_class(vc), _edge_class(ec), _face_class(fc)
{
    _surface = gts_surface_new(gts_surface_class(),
                                _face_class,
                                _edge_class,
                                _vertex_class);
}


Surface::Surface(const Surface& s)
:   _surface(NULL),
    _vertex_class(s.get_vclass()),
    _edge_class(s.get_eclass()),
    _face_class(s.get_fclass())
{
    _surface = gts_surface_new(gts_surface_class(),
                                s.get_fclass(),
                                s.get_eclass(),
                                s.get_vclass());
    _surface = gts_surface_copy(_surface, s._surface);
}


Surface& Surface::operator=(const Surface& s)
{
    if (this == &s) return *this;
    destroy();
    _surface = gts_surface_new(gts_surface_class(),
                                s.get_fclass(),
                                s.get_eclass(),
                                s.get_vclass());
    _surface = gts_surface_copy(_surface, s._surface);
    return *this;
}


void Surface::add_face(Face* face)
{
    gts_surface_add_face(_surface, GTS_FACE(face));
}


void Surface::add_sphere(gint order)
{
    _surface = gts_surface_generate_sphere(_surface, order);
}


void Surface::add_square(gdouble sl)
{
    add_rectangle(0, 0, 0, sl, 0, 0, sl, sl, 0, 0, sl, 0);
}


void Surface::add_rectangle(
            gdouble x0, gdouble y0, gdouble z0,
            gdouble x1, gdouble y1, gdouble z1,
            gdouble x2, gdouble y2, gdouble z2,
            gdouble x3, gdouble y3, gdouble z3)
{
    Vertex* p1 = vertex_new(get_vclass(), x0, y0, z0);
    Vertex* p2 = vertex_new(get_vclass(), x1, y1, z1);
    Vertex* p3 = vertex_new(get_vclass(), x2, y2, z2);
    Vertex* p4 = vertex_new(get_vclass(), x3, y3, z3);
    Edge* e1 = edge_new(get_eclass(), p1, p2);
    Edge* e2 = edge_new(get_eclass(), p2, p3);
    Edge* e3 = edge_new(get_eclass(), p3, p1);
    Face* f1 = face_new(get_fclass(), e1, e2, e3);
    Edge* e4 = edge_new(get_eclass(), p3, p4);
    Edge* e5 = edge_new(get_eclass(), p4, p1);
    Edge* e6 = edge_new(get_eclass(), p1, p3);
    Face* f2 = face_new(get_fclass(), e4, e5, e6);
    add_face(f1);
    add_face(f2);
}


void Surface::foreach_vertex(GtsFunc func, gpointer data)
{
    gts_surface_foreach_vertex(_surface, func, data);
}


void Surface::foreach_vertex(GtsFunc func, gpointer data) const
{
    gts_surface_foreach_vertex(_surface, func, data);
}


void Surface::foreach_edge(GtsFunc func, gpointer data)
{
    gts_surface_foreach_edge(_surface, func, data);
}


void Surface::foreach_edge(GtsFunc func, gpointer data) const
{
    gts_surface_foreach_edge(_surface, func, data);
}


void Surface::foreach_face(GtsFunc func, gpointer data)
{
    gts_surface_foreach_face(_surface, func, data);
}


void Surface::foreach_face(GtsFunc func, gpointer data) const
{
    gts_surface_foreach_face(_surface, func, data);
}



namespace
{
    void list_build(gpointer data, GList** list)
    {
        *list = g_list_prepend(*list, data);
    }


    void slist_build(gpointer data, GSList** slist)
    {
        *slist = g_slist_prepend(*slist, data);
    }


    gboolean aface_is_downward(Face* f, gpointer)
    {
        return (f->normal_area().z <= 0.0);
    }


    void aface_split(Face* f, gpointer* data)
    {
        VertexClass* vclass = VERTEX_CLASS(data[0]);
        EdgeClass* eclass = EDGE_CLASS(data[1]);
        Edge*   e1 = f->e1(), * e2 = f->e2(), * e3 = f->e3();
        Vertex* v1 = f->v1(), * v2 = f->v2(), * v3 = f->v3();
        GtsPoint* p1 = GTS_POINT(v1), * p2 = GTS_POINT(v2), * p3 = GTS_POINT(v3);
        Vertex* vn1 = vertex_new(vclass, p1->x, p1->y, p1->z);
        Vertex* vn2 = vertex_new(vclass, p2->x, p2->y, p2->z);
        Vertex* vn3 = vertex_new(vclass, p3->x, p3->y, p3->z);
        Edge* en1 = edge_new(eclass, vn1, vn2);
        Edge* en2 = edge_new(eclass, vn2, vn3);
        Edge* en3 = edge_new(eclass, vn3, vn1);
        GtsTriangle* t = GTS_TRIANGLE(f);
        t->e1 = GTS_EDGE(en1); t->e2 = GTS_EDGE(en2); t->e3 = GTS_EDGE(en3);

        en1->faces_prepend(f);
        en2->faces_prepend(f);
        en3->faces_prepend(f);
        e1->faces_remove(f);
        e2->faces_remove(f);
        e3->faces_remove(f);
        if (e1->faces() == NULL) gts_object_destroy(GTS_OBJECT(e1));
        if (e2->faces() == NULL) gts_object_destroy(GTS_OBJECT(e2));
        if (e3->faces() == NULL) gts_object_destroy(GTS_OBJECT(e3));
    }
}


void Surface::vertex_duplicate_remove()
{
    GList* vertices = NULL;
    foreach_vertex((GtsFunc) list_build, &vertices);
    vertices = gts_vertices_merge(vertices, Vertex::EPSILON, NULL);
    g_list_free(vertices);
}


void Surface::edge_duplicate_remove()
{
    GSList* edges = NULL;
    foreach_edge(reinterpret_cast<GtsFunc>(slist_build), &edges);
    gts_allow_floating_edges = TRUE;            // 设置允许浮动边
    GSList* i = edges;
    while (i){
        Edge* e = EDGE(i->data);
        Edge* duplicate = NULL;
        if (e->v1() == e->v2()) {
            gts_object_destroy(GTS_OBJECT(e));
        }
        else if (duplicate = e->duplicate()){
            duplicate->replace(*e);
            gts_object_destroy(GTS_OBJECT(e));
        }
        i = i->next;
    }
    gts_allow_floating_edges = FALSE;           // 设置回不允许浮动边
    g_slist_free(edges);
}


void Surface::face_duplicate_remove()
{
    GSList* faces = NULL;
    gts_surface_foreach_face(_surface, (GtsFunc) slist_build, &faces);
    GSList* i = faces;
    while (i){
        GtsTriangle* t = GTS_TRIANGLE(i->data);
        if (gts_triangle_is_duplicate(t))
            gts_object_destroy(GTS_OBJECT(t));
        i = i->next;
    }
    g_slist_free(faces);
}


void Surface::face_downward_remove()
{
    gts_surface_foreach_face_remove(_surface,
                                    (GtsFunc) aface_is_downward,
                                    NULL);
}


void Surface::face_split()
{
    gpointer data[] = {get_vclass(), get_eclass()};
    foreach_face((GtsFunc) aface_split, data);
}


void Surface::face_offset(gdouble d)
{
    face_split();
    foreach_face((GtsFunc)aface_offset, &d);
}


void Surface::reverse()
{
    foreach_face((GtsFunc) aface_reverse, NULL);
}


void Surface::move(const Vector& mv)
{
    GtsVector t = {mv.x, mv.y, mv.z};
    GtsMatrix* matrix = gts_matrix_translate(NULL, t);
    transform(matrix);
    gts_matrix_destroy(matrix);
}


void Surface::scale(const Vector& sv)
{
    GtsVector s = {sv.x, sv.y, sv.z};
    GtsMatrix* matrix = gts_matrix_scale(NULL, s);
    transform(matrix);
    gts_matrix_destroy(matrix);
}


void Surface::rotate(const Vector& rv, gdouble angle)
{
    GtsVector axis = {rv.x, rv.y, rv.z};
    GtsMatrix* matrix = gts_matrix_rotate(NULL, axis, angle);
    transform(matrix);
    gts_matrix_destroy(matrix);
}


namespace
{
    void avertex_transform(Vertex* v, GtsMatrix* m)
    {
        v->transform(m);
    }
}
void Surface::transform(GtsMatrix* m)
{
    g_return_if_fail(m != NULL);
    foreach_vertex((GtsFunc) avertex_transform, m);
}


namespace
{// Surface::offset
    void vertices_build(Vertex* v, GSList** slist)
    {
        *slist = g_slist_prepend(*slist, v);
    }

    void edges_build(Edge* e, GSList** slist)
    {
        *slist = g_slist_prepend(*slist, e);
    }

    void faces_build(Edge* f, GSList** slist)
    {
        *slist = g_slist_prepend(*slist, f);
    }

    void vertices_vector_init(Vertex* v, gpointer)
    {
        g_return_if_fail(v->reserved == NULL);
        v->reserved = new Vector;
    }

    void edges_vector_init(Edge* e, gpointer)
    {
        g_return_if_fail(e->reserved == NULL);
        e->reserved = new Vector;
    }

    void faces_isolate(Face* face, gpointer* data)
    {
        Surface* sf = static_cast<Surface*>(data[0]);
        gdouble* od = static_cast<gdouble*>(data[1]);
        VertexClass* vclass = static_cast<VertexClass*>(data[2]);
        EdgeClass* eclass = static_cast<EdgeClass*>(data[3]);
        FaceClass* fclass = static_cast<FaceClass*>(data[4]);

        Vertex* v1 = face->v1(), * v2 = face->v2(), * v3 = face->v3();
        GtsPoint *p1 = v1->p(), *p2 = v2->p(), *p3 = v3->p();
        Vertex* new_v1 = vertex_new(vclass, p1->x, p1->y, p1->z);
        Vertex* new_v2 = vertex_new(vclass, p2->x, p2->y, p2->z);
        Vertex* new_v3 = vertex_new(vclass, p3->x, p3->y, p3->z);
        new_v1->reserved = v1;
        new_v2->reserved = v2;
        new_v3->reserved = v3;

        Edge* e1 = face->e1(), * e2 = face->e2(), * e3 = face->e3();
        Edge* new_e1 = edge_new(eclass, new_v1, new_v2);
        Edge* new_e2 = edge_new(eclass, new_v2, new_v3);
        Edge* new_e3 = edge_new(eclass, new_v3, new_v1);
        new_e1->reserved = e1;
        new_e2->reserved = e2;
        new_e3->reserved = e3;

        GtsTriangle* t = face->t();
        t->e1 = GTS_EDGE(new_e1);
        t->e2 = GTS_EDGE(new_e2);
        t->e3 = GTS_EDGE(new_e3);
        new_e1->faces_prepend(face);
        new_e2->faces_prepend(face);
        new_e3->faces_prepend(face);
    }

    void faces_move(Face* face, gpointer* data)
    {
        Surface* sf = static_cast<Surface*>(data[0]);
        gdouble* od = static_cast<gdouble*>(data[1]);
        VertexClass* vclass = static_cast<VertexClass*>(data[2]);
        EdgeClass* eclass = static_cast<EdgeClass*>(data[3]);
        FaceClass* fclass = static_cast<FaceClass*>(data[4]);

        Vector ovec = *od * face->normal_uniform();
        face->v1()->move(ovec);
        face->v2()->move(ovec);
        face->v3()->move(ovec);
    }

    void faces_to_vertices_vector(Face* face, gpointer* data)
    {
        Surface* sf = static_cast<Surface*>(data[0]);
        gdouble* od = static_cast<gdouble*>(data[1]);
        VertexClass* vclass = static_cast<VertexClass*>(data[2]);
        EdgeClass* eclass = static_cast<EdgeClass*>(data[3]);
        FaceClass* fclass = static_cast<FaceClass*>(data[4]);

        Vector ovec = *od * face->normal_uniform();

        Vertex* v1 = VERTEX(face->v1()->reserved);
        Vertex* v2 = VERTEX(face->v2()->reserved);
        Vertex* v3 = VERTEX(face->v3()->reserved);
        *static_cast<Vector*>(v1->reserved) += ovec;
        *static_cast<Vector*>(v2->reserved) += ovec;
        *static_cast<Vector*>(v3->reserved) += ovec;
    }

    void faces_to_edges_vector(Face* face, gpointer* data)
    {
        Surface* sf = static_cast<Surface*>(data[0]);
        gdouble* od = static_cast<gdouble*>(data[1]);
        VertexClass* vclass = static_cast<VertexClass*>(data[2]);
        EdgeClass* eclass = static_cast<EdgeClass*>(data[3]);
        FaceClass* fclass = static_cast<FaceClass*>(data[4]);

        Vector ovec = *od * face->normal_uniform();

        Edge* e1 = EDGE(face->e1()->reserved);
        Edge* e2 = EDGE(face->e2()->reserved);
        Edge* e3 = EDGE(face->e3()->reserved);
        *static_cast<Vector*>(e1->reserved) += ovec;
        *static_cast<Vector*>(e2->reserved) += ovec;
        *static_cast<Vector*>(e3->reserved) += ovec;
    }

    void edges_isolate(Edge* edge, gpointer* data)
    {
        Surface* sf = static_cast<Surface*>(data[0]);
        gdouble* od = static_cast<gdouble*>(data[1]);
        VertexClass* vclass = static_cast<VertexClass*>(data[2]);
        EdgeClass* eclass = static_cast<EdgeClass*>(data[3]);
        FaceClass* fclass = static_cast<FaceClass*>(data[4]);

        Vertex* v1 = edge->v1(), * v2 = edge->v2();
        GtsPoint *p1 = GTS_POINT(v1), *p2 = GTS_POINT(v2);
        Vertex* new_v1 = vertex_new(vclass, p1->x, p1->y, p1->z);
        Vertex* new_v2 = vertex_new(vclass, p2->x, p2->y, p2->z);
        new_v1->reserved = v1;
        new_v2->reserved = v2;

        GtsSegment* seg = edge->s();
        seg->v1 = GTS_VERTEX(new_v1);
        seg->v2 = GTS_VERTEX(new_v2);
        new_v1->edges_prepend(edge);
        new_v2->edges_prepend(edge);
    }

    void edges_move(Edge* edge, gpointer* data)
    {
        Surface* sf = static_cast<Surface*>(data[0]);
        gdouble* od = static_cast<gdouble*>(data[1]);
        VertexClass* vclass = static_cast<VertexClass*>(data[2]);
        EdgeClass* eclass = static_cast<EdgeClass*>(data[3]);
        FaceClass* fclass = static_cast<FaceClass*>(data[4]);

        Vector* vec = static_cast<Vector*>(edge->reserved);
        Vector ovec = *od * vec->uniform();
        edge->v1()->move(ovec);
        edge->v2()->move(ovec);
        delete vec; edge->reserved = NULL;
    }

    void vertices_move(Vertex* vertex, gpointer* data)
    {
        Surface* sf = static_cast<Surface*>(data[0]);
        gdouble* od = static_cast<gdouble*>(data[1]);
        VertexClass* vclass = static_cast<VertexClass*>(data[2]);
        EdgeClass* eclass = static_cast<EdgeClass*>(data[3]);
        FaceClass* fclass = static_cast<FaceClass*>(data[4]);

        Vector* vec = static_cast<Vector*>(vertex->reserved);
        Vector ovec = *od * vec->uniform();
        vertex->move(ovec);
        delete vec; vertex->reserved = NULL;
    }

    void edges_remove_faces(Edge* edge, gpointer)
    {
        edge->faces_free();
    }

    void vertices_remove_edges(Vertex* vertex, gpointer)
    {
        vertex->edges_free();
    }

    void edges_destroy(Edge* edge, gpointer)
    {
        gts_object_destroy(GTS_OBJECT(edge));
    }

    void edges_reserved_reset(Edge* edge, gpointer)
    {
        edge->v1()->reserved = NULL;
        edge->v2()->reserved = NULL;
    }

    void faces_reserved_reset(Face* face, gpointer)
    {
        face->e1()->reserved = NULL;
        face->e2()->reserved = NULL;
        face->e3()->reserved = NULL;
        face->v1()->reserved = NULL;
        face->v2()->reserved = NULL;
        face->v3()->reserved = NULL;
    }

    void vertices_check_no_reserved(Vertex* vertex, gpointer)
    {
        g_return_if_fail(vertex->reserved == NULL);
    }

    void edges_check_no_reserved(Edge* edge, gpointer)
    {
        g_return_if_fail(edge->reserved == NULL);
        g_return_if_fail(edge->v1()->reserved == NULL);
        g_return_if_fail(edge->v2()->reserved == NULL);
    }

    void faces_check_no_reserved(Face* face, gpointer)
    {
        g_return_if_fail(face->e1()->reserved == NULL);
        g_return_if_fail(face->e2()->reserved == NULL);
        g_return_if_fail(face->e3()->reserved == NULL);
        g_return_if_fail(face->v1()->reserved == NULL);
        g_return_if_fail(face->v2()->reserved == NULL);
        g_return_if_fail(face->v3()->reserved == NULL);
    }

    void edges_link(Edge* edge, gpointer* data)
    {
        Surface* sf = static_cast<Surface*>(data[0]);
        gdouble* od = static_cast<gdouble*>(data[1]);
        VertexClass* vclass = static_cast<VertexClass*>(data[2]);
        EdgeClass* eclass = static_cast<EdgeClass*>(data[3]);
        FaceClass* fclass = static_cast<FaceClass*>(data[4]);

        GSList* faces = edge->faces();
        GSList* i = faces;
        GTS_EDGE(edge)->triangles = NULL;
        while (i) {
            Face* face = FACE(i->data);
            Edge* e = face->e1();
            if (EDGE(e->reserved) != edge) e = face->e2();
            if (EDGE(e->reserved) != edge) e = face->e3();
            if (EDGE(e->reserved) != edge) {
                re_error << "no edge point back!\n";
            }

            Vertex* etop1 = e->beg(face);
            Vertex* etop2 = e->opposite(etop1);
            Vertex* vleft = VERTEX(etop1->reserved);
            Vertex* vrght = VERTEX(etop2->reserved);
            Vertex* edwn1 = edge->v1();
            Vertex* edwn2 = edge->v2();
            if (VERTEX(edwn1->reserved) == vrght) {
                edwn1 = edge->v2();
                edwn2 = edge->v1();
            }

            Edge* eleft1 = edge_new(eclass, edwn1, etop1);
            Edge* eleft2 = get_edge(eclass, etop1, vleft);
            Edge* eleft3 = get_edge(eclass, vleft, edwn1);
            Face* fleft = face_new(fclass, eleft1, eleft2, eleft3);
            sf->add_face(fleft);

            Edge* erght1 = edge_new(eclass, etop2, edwn2);
            Edge* erght2 = get_edge(eclass, edwn2, vrght);
            Edge* erght3 = get_edge(eclass, vrght, etop2);
            Face* frght = face_new(fclass, erght1, erght2, erght3);
            sf->add_face(frght);

            Edge* ediag = edge_new(eclass, etop1, edwn2);
            Face* ftop = face_new(fclass, eleft1, edge, ediag);
            Face* fdwn = face_new(fclass, erght1, e, ediag);
            sf->add_face(ftop);
            sf->add_face(fdwn);

            i = i->next;
        }
        g_slist_free(faces);
    }

    void edges_link1(Edge* edge, gpointer* data)
    {
        Surface* sf = static_cast<Surface*>(data[0]);
        gdouble* od = static_cast<gdouble*>(data[1]);
        VertexClass* vclass = static_cast<VertexClass*>(data[2]);
        EdgeClass* eclass = static_cast<EdgeClass*>(data[3]);
        FaceClass* fclass = static_cast<FaceClass*>(data[4]);

        GSList* faces = edge->faces();
        GSList* i = faces;
        GTS_EDGE(edge)->triangles = NULL;
        if (i == NULL) return;

        Face* face = FACE(i->data);
        Edge* e = face->e1();
        if (EDGE(e->reserved) != edge) e = face->e2();
        if (EDGE(e->reserved) != edge) e = face->e3();
        if (EDGE(e->reserved) != edge) {
            re_error << "no edge point back!\n";
        }
        Vertex* etop1 = e->beg(face);
        Vertex* etop2 = e->opposite(etop1);
        Vertex* vleft = VERTEX(etop1->reserved);
        Vertex* vrght = VERTEX(etop2->reserved);
        i = i->next;

        Edge* e1 = NULL;

        if (i) {
            Face* face1 = FACE(i->data);
            e1 = face1->e1();
            if (EDGE(e1->reserved) != edge) e1 = face1->e2();
            if (EDGE(e1->reserved) != edge) e1 = face1->e3();
            if (EDGE(e1->reserved) != edge) {
                re_error << "no edge point back!\n";
            }
        }
        else {
            GtsPoint* p1 = edge->v1()->p();
            GtsPoint* p2 = edge->v2()->p();
            Vertex* v1 = vertex_new(vclass, p1->x, p1->y, p1->z);
            Vertex* v2 = vertex_new(vclass, p2->x, p2->y, p2->z);
            v1->reserved = edge->v1()->reserved;
            v2->reserved = edge->v1()->reserved;
            e1 = edge_new(eclass, v1, v2);
        }

        if (i && i->next) {
            re_warning << "edge used by more than three faces.\n";
        }

        Vertex* edwn1 = e1->v1();
        Vertex* edwn2 = e1->v2();
        if (VERTEX(edwn1->reserved) == vrght) {
            edwn1 = e1->v2();
            edwn2 = e1->v1();
        }

        Edge* eleft1 = edge_new(eclass, edwn1, etop1);
        Edge* eleft2 = get_edge(eclass, etop1, vleft);
        Edge* eleft3 = get_edge(eclass, vleft, edwn1);
        Face* fleft = face_new(fclass, eleft1, eleft2, eleft3);
        sf->add_face(fleft);

        Edge* erght1 = edge_new(eclass, etop2, edwn2);
        Edge* erght2 = get_edge(eclass, edwn2, vrght);
        Edge* erght3 = get_edge(eclass, vrght, etop2);
        Face* frght = face_new(fclass, erght1, erght2, erght3);
        sf->add_face(frght);

        Edge* ediag = edge_new(eclass, etop1, edwn2);
        Face* ftop = face_new(fclass, eleft1, e1, ediag);
        Face* fdwn = face_new(fclass, erght1, e, ediag);
        sf->add_face(ftop);
        sf->add_face(fdwn);

        g_slist_free(faces);
    }

    void edges_link2(Edge* edge, gpointer* data)
    {
        Surface* sf = static_cast<Surface*>(data[0]);
        gdouble* od = static_cast<gdouble*>(data[1]);
        VertexClass* vclass = static_cast<VertexClass*>(data[2]);
        EdgeClass* eclass = static_cast<EdgeClass*>(data[3]);
        FaceClass* fclass = static_cast<FaceClass*>(data[4]);

        GSList* faces = edge->faces();
        GSList* i = faces;
        GTS_EDGE(edge)->triangles = NULL;
        while (i) {
            Face* face = FACE(i->data);
            Edge* e = face->e1();
            if (EDGE(e->reserved) != edge) e = face->e2();
            if (EDGE(e->reserved) != edge) e = face->e3();
            if (EDGE(e->reserved) != edge) {
                re_error << "no edge point back!\n";
            }

            Vertex* etop1 = e->beg(face);
            Vertex* etop2 = e->opposite(etop1);
            Vertex* edwn1 = VERTEX(etop1->reserved);
            Vertex* edwn2 = VERTEX(etop2->reserved);

            Edge* eleft1 = get_edge(eclass, edwn1, etop1);
            Edge* erght1 = get_edge(eclass, etop2, edwn2);
            Edge* ediag = edge_new(eclass, etop1, edwn2);
            Face* ftop = face_new(fclass, eleft1, edge, ediag);
            Face* fdwn = face_new(fclass, erght1, e, ediag);
            sf->add_face(ftop);
            sf->add_face(fdwn);

            i = i->next;
        }
        g_slist_free(faces);
    }
}


void Surface::offset(gdouble off)
{
    GSList* vertices = NULL, * edges = NULL, * faces = NULL;
    foreach_vertex((GtsFunc) vertices_build, &vertices);
    foreach_edge((GtsFunc) edges_build, &edges);
    foreach_face((GtsFunc) faces_build, &faces);

    g_slist_foreach(vertices, (GFunc) vertices_vector_init, NULL);
    g_slist_foreach(edges, (GFunc) edges_vector_init, NULL);

    gts_allow_floating_edges = TRUE;
    gpointer data[] = {this, &off, get_vclass(), get_eclass(), get_fclass()};

    g_slist_foreach(faces, (GFunc) faces_isolate, data);
    g_slist_foreach(faces, (GFunc) faces_move, data);
    g_slist_foreach(faces, (GFunc) faces_to_vertices_vector, data);
    g_slist_foreach(faces, (GFunc) faces_to_edges_vector, data);

    g_slist_foreach(edges, (GFunc) edges_isolate, data);
    g_slist_foreach(edges, (GFunc) edges_move, data);

    g_slist_foreach(vertices, (GFunc) vertices_move, data);
    g_slist_foreach(vertices, (GFunc) vertices_remove_edges, NULL);

    g_slist_foreach(edges, (GFunc) edges_link, data);

    g_slist_foreach(edges, (GFunc) edges_reserved_reset, data);
    g_slist_foreach(faces, (GFunc) faces_reserved_reset, data);

    gts_allow_floating_edges = FALSE;

    g_slist_free(faces);
    g_slist_free(edges);
    g_slist_free(vertices);
}


void Surface::offset1(gdouble off)
{
    GSList* vertices = NULL, * edges = NULL, * faces = NULL;
    foreach_vertex((GtsFunc) vertices_build, &vertices);
    foreach_edge((GtsFunc) edges_build, &edges);
    foreach_face((GtsFunc) faces_build, &faces);

    g_slist_foreach(vertices, (GFunc) vertices_vector_init, NULL);

    gts_allow_floating_edges = TRUE;
    gpointer data[] = {this, &off, get_vclass(), get_eclass(), get_fclass()};

    g_slist_foreach(faces, (GFunc) faces_isolate, data);
    g_slist_foreach(faces, (GFunc) faces_move, data);
    g_slist_foreach(faces, (GFunc) faces_to_vertices_vector, data);

    g_slist_foreach(vertices, (GFunc) vertices_move, data);

    g_slist_foreach(edges, (GFunc) edges_link1, data);
    g_slist_foreach(edges, (GFunc) edges_destroy, NULL);

    g_slist_foreach(faces, (GFunc) faces_reserved_reset, data);

    gts_allow_floating_edges = FALSE;

    g_slist_free(faces);
    g_slist_free(edges);
    g_slist_free(vertices);
}


void Surface::offset2(gdouble off)
{
    GSList* vertices = NULL, * edges = NULL, * faces = NULL;
    foreach_vertex((GtsFunc) vertices_build, &vertices);
    foreach_edge((GtsFunc) edges_build, &edges);
    foreach_face((GtsFunc) faces_build, &faces);

    g_slist_foreach(vertices, (GFunc) vertices_vector_init, NULL);

    gts_allow_floating_edges = TRUE;
    gpointer data[] = {this, &off, get_vclass(), get_eclass(), get_fclass()};

    g_slist_foreach(faces, (GFunc) faces_isolate, data);
    g_slist_foreach(faces, (GFunc) faces_move, data);
    g_slist_foreach(faces, (GFunc) faces_to_vertices_vector, data);

    g_slist_foreach(vertices, (GFunc) vertices_move, data);

    g_slist_foreach(edges, (GFunc) edges_link2, data);

    g_slist_foreach(faces, (GFunc) faces_reserved_reset, data);

    gts_allow_floating_edges = FALSE;

    g_slist_free(faces);
    g_slist_free(edges);
    g_slist_free(vertices);
}

void Surface::read(const std::string& fname)
{
    std::string fformat;
    if (is_ply_filename(fname)) fformat = PLY_ASCII;
    else if (is_gts_filename(fname)) fformat = GTS_ASCII;
    std::ifstream fin(fname.c_str());
    read(fin, fformat);
    fin.close();
}


void Surface::read(std::istream& is, const std::string& fformat)
{
    if (fformat == PLY_ASCII)      read_ply(is);
    else if (fformat == GTS_ASCII) read_gts(is);
}


void Surface::write(const std::string& fname) const
{
    std::string fformat;
    if (is_ply_filename(fname))
        fformat = PLY_ASCII;
    else if (is_gts_filename(fname))
        fformat = GTS_ASCII;
    std::ofstream fout(fname.c_str());
    write(fout, fformat);
    fout.close();
}


void Surface::write(std::ostream& os, const std::string& fformat) const
{
    if (fformat == PLY_ASCII)      write_ply(os);
    else if(fformat == GTS_ASCII)  write_gts(os);
}


void Surface::write_number(std::ostream& os) const
{
    GtsSurfaceStats stats;
    get_stats(stats);
    os  << "model ("
        << stats.edges_per_vertex.n << "v/"
        << stats.faces_per_edge.n << "e/"
        << stats.n_faces << "f/"
        << stats.n_boundary_edges << "b"
        << ")\n"
        << "faces duplicate/incompatible|"
        << std::setw(8) << stats.n_duplicate_faces
        << std::setw(8) << stats.n_incompatible_faces << "\n"
        << "edges duplicate/non-manifold|"
        << std::setw(8) << stats.n_duplicate_edges
        << std::setw(8) << stats.n_non_manifold_edges << "\n"
        << std::flush;
}


void Surface::write_info(std::ostream& os) const
{
    os.setf(std::ios::fixed, std::ios::floatfield);
    os.precision(3);
    write_number(os);

    GtsSurfaceQualityStats qstats;
    get_qstats(qstats);
    os  << "face quality(min/mean/max)  |"
        << std::setw(8) << qstats.face_quality.min
        << std::setw(8) << qstats.face_quality.mean
        << std::setw(8) << qstats.face_quality.max << "\n"
        << "face area(min/mean/max)     |"
        << std::setw(8) << qstats.face_area.min
        << std::setw(8) << qstats.face_area.mean
        << std::setw(8) << qstats.face_area.max << "\n"
        << "edge length(min/mean/max)   |"
        << std::setw(8) << qstats.edge_length.min
        << std::setw(8) << qstats.edge_length.mean
        << std::setw(8) << qstats.edge_length.max << "\n"
        << "edge angle(min/mean/max)    |"
        << std::setw(8) << qstats.edge_angle.min
        << std::setw(8) << qstats.edge_angle.mean
        << std::setw(8) << qstats.edge_angle.max << "\n"
        << std::flush;

    GtsBBox* bbox = gen_bbox();
    os  << "bbox maxlen                 |"
        << std::setw(8) << bbox->x2 - bbox->x1
        << std::setw(8) << bbox->y2 - bbox->y1
        << std::setw(8) << bbox->z2 - bbox->z1 << "\n"
        << "bbox center                 |"
        << " (" << (bbox->x2 + bbox->x1) / 2.0
        << ", " << (bbox->y2 + bbox->y1) / 2.0
        << ", " << (bbox->z2 + bbox->z1) / 2.0 << ")\n"
        << "bbox min point              |"
        << " (" << bbox->x1
        << ", " << bbox->y1
        << ", " << bbox->z1 << ")\n"
        << "bbox max point              |"
        << " (" << bbox->x2
        << ", " << bbox->y2
        << ", " << bbox->z2 << ")\n"
        << std::flush;
    gts_object_destroy(GTS_OBJECT(bbox));
}


void Surface::get_stats(GtsSurfaceStats& stats) const
{
    return gts_surface_stats(_surface, &stats);
}


void Surface::get_qstats(GtsSurfaceQualityStats& qstats) const
{
    return gts_surface_quality_stats(_surface, &qstats);
}


GtsBBox* Surface::gen_bbox(GtsBBoxClass* bbclass) const
{
    return gts_bbox_surface(bbclass, _surface);
}


GNode* Surface::gen_bbtree() const
{
    return gts_bb_tree_surface(_surface);
}


GSList* Surface::gen_intersection(const Surface& s, GNode *t1, GNode *t2) const
{
    return gts_surface_intersection(_surface, s._surface, t1, t2);
}


namespace // edge collapse functions
{
    #define HEAP_INSERT_EDGE(h, e) \
        (GTS_OBJECT (e)->reserved = gts_eheap_insert (h, e))
    #define HEAP_REMOVE_EDGE(h, e) \
        (gts_eheap_remove (h, (GtsEHeapPair*) (GTS_OBJECT (e)->reserved)),\
                GTS_OBJECT (e)->reserved = NULL)

    void update_neighbors(Vertex* v, GtsEHeap* heap)
    {
        GSList* i = v->edges();
        while (i) {
            Edge* e = EDGE(i->data);
            HEAP_REMOVE_EDGE(heap, e);
            HEAP_INSERT_EDGE(heap, e);
            i = i->next;
        }
    }

    void create_heap_coarsen(Edge* e, GtsEHeap* heap)
    {
        HEAP_INSERT_EDGE(heap, e);
    }


    void avertex_new_quadric(Vertex* v, gpointer)
    {
        g_return_if_fail(v->reserved == NULL);
        v->reserved = new Quadric;
    }
    void one_vertex_delete_quadric(Vertex* v, gpointer)
    {
        delete static_cast<Quadric*>(v->reserved);
        v->reserved = NULL;
    }

    void one_edge_new_quadric(Edge* e, gpointer)
    {
        g_return_if_fail(e->reserved == NULL);
        e->reserved = new Quadric;
    }
    void one_edge_delete_quadric(Edge* e, gpointer)
    {
        delete static_cast<Quadric*>(e->reserved);
        e->reserved = NULL;
    }
    void add_face_quadric(Face* f, Qem_params* p)
    {
        Vertex* v1 = f->v1(), * v2 = f->v2(), * v3 = f->v3();
        GtsPoint* p1 = GTS_POINT(v1), *p2 = GTS_POINT(v2), *p3 = GTS_POINT(v3);

        Vector vf = f->normal_area();
        if (p->is_uniform) {
            vf.uniform();
        }
        gdouble d = - (vf * Vector(p1->x, p1->y, p1->z));

        Quadric q;
        q.add_plane(vf, d);

        Quadric* q1 = static_cast<Quadric*>(v1->reserved);
        Quadric* q2 = static_cast<Quadric*>(v2->reserved);
        Quadric* q3 = static_cast<Quadric*>(v3->reserved);
        *q1 += q;
        *q2 += q;
        *q3 += q;
        if (p->is_union) {
            Edge* e1 = f->e1(), * e2 = f->e2(), * e3 = f->e3();
            Quadric* qe1 = static_cast<Quadric*>(e1->reserved);
            Quadric* qe2 = static_cast<Quadric*>(e2->reserved);
            Quadric* qe3 = static_cast<Quadric*>(e3->reserved);
            *qe1 += q;
            *qe2 += q;
            *qe3 += q;
        }
    }

    void add_border_quadric(Edge* e, Qem_params* p)
    {
        Face* f = FACE(gts_edge_is_boundary(GTS_EDGE(e), NULL));
        if (f == NULL) return;

        Vector vbf = f->normal_area();              // vector of border face

        Vertex *ev1 = e->v1(), * ev2 = e->v2();
        GtsPoint* p1 = GTS_POINT(ev1), *p2 = GTS_POINT(ev2);
        Vector vbe(p2->x - p1->x, p2->y - p1->y, p2->z - p1->z);
        vbe.uniform();                              // vector of border edge

        Vector vvf = vbf.cross(vbe);                // vector of vertical face

        if (p->is_uniform) {
            vbf.uniform();
            vvf.uniform();
        }

        Quadric q;
        gdouble d;

        vvf.scale(p->border_weight);
        d = -(vvf * Vector(p1->x, p1->y, p1->z));
        q.add_plane(vvf, d);

        vbf.scale(p->border_weight - 1.0);
        d = -(vbf * Vector(p1->x, p1->y, p1->z));
        q.add_plane(vbf, d);

        Quadric* q1 = static_cast<Quadric*>(ev1->reserved);
        Quadric* q2 = static_cast<Quadric*>(ev2->reserved);
        *q1 += q;
        *q2 += q;
        if (p->is_union) {
            Quadric* qe = static_cast<Quadric*>(e->reserved);
            *qe += q;
        }
    }


    Vertex* qem_subv(GtsEHeap* heap, Edge* e, Qem_params* params)
    {

        Vertex *v1 = e->v1(), *v2 = e->v2();
        if (v1 == v2) {
            gts_object_destroy(GTS_OBJECT (e));
            return NULL;
        }

        if (!gts_edge_collapse_is_valid(GTS_EDGE(e))) {
            GTS_OBJECT(e)->reserved =
                gts_eheap_insert_with_key(heap, e, G_MAXDOUBLE);
            return NULL;
        }

        Quadric* q1 = static_cast<Quadric*>(v1->reserved);
        Quadric* q2 = static_cast<Quadric*>(v2->reserved);
        Quadric quadric = *q1 + *q2;
        Vector minvec;
        if (!quadric.get_min_vector(minvec)) {
            GtsPoint* p1 = GTS_POINT(v1), *p2 = GTS_POINT(v2);
            Vector vec1(p1->x, p1->y, p1->z);
            Vector vec2(p2->x, p2->y, p2->z);
            Vector vec0 = (vec1 + vec2) / 2.0;
            gdouble c1 = quadric.value(vec1);
            gdouble c2 = quadric.value(vec2);
            gdouble c0 = quadric.value(vec0);
            if (c1 < c2 && c1 < c0) minvec = vec1;
            else if (c2 < c1 && c2 < c0) minvec = vec2;
            else minvec = vec0;
        }
        *q1 += *q2;
        v1->replace(*v2);
        delete q2;
        v2->reserved = NULL;
        gts_object_destroy(GTS_OBJECT(e));
        gts_object_destroy(GTS_OBJECT(v2));
        GtsPoint* p1 = GTS_POINT(v1);
        p1->x = minvec.x;
        p1->y = minvec.y;
        p1->z = minvec.z;
        GSList* i = v1->edges();
        while (i) {
            Edge* e1 = EDGE(i->data);
            Edge* duplicate;
            while (duplicate = e1->duplicate()) {
                e1->replace(*duplicate);
                HEAP_REMOVE_EDGE(heap, duplicate);
                gts_object_destroy(GTS_OBJECT(duplicate));
            }
            i = i->next;
            if (!e1->faces()) {
                re_error << "isolated edge found.\n" << e1 << std::endl;
                HEAP_REMOVE_EDGE(heap, e1);
                gts_object_destroy(GTS_OBJECT(e1));
                if (i == NULL)
                    v1 = NULL;
            }
        }
        return v1;
    }

    gdouble qem_cost(Edge* e, Qem_params* params)
    {
        Vertex* v1 = e->v1(), * v2 = e->v2();
        Quadric* q1 = static_cast<Quadric*>(v1->reserved);
        Quadric* q2 = static_cast<Quadric*>(v2->reserved);
        Quadric quadric = *q1 + *q2;
        Vector minvec;
        if (!quadric.get_min_vector(minvec)) {
            GtsPoint* p1 = GTS_POINT(v1), *p2 = GTS_POINT(v2);
            Vector vec1(p1->x, p1->y, p1->z);
            Vector vec2(p2->x, p2->y, p2->z);
            Vector vec0 = (vec1 + vec2) / 2.0;
            gdouble c1 = quadric.value(vec1);
            gdouble c2 = quadric.value(vec2);
            gdouble c0 = quadric.value(vec0);
            if (c1 < c2 && c1 < c0) minvec = vec1;
            else if (c2 < c1 && c2 < c0) minvec = vec2;
            else minvec = vec0;
        }
        gdouble cost = quadric.value(minvec);
        if (e->is_boundary()) cost /= params->border_weight;
        return cost;
    }

    Vertex* qemu_subv(GtsEHeap* heap, Edge* e, Qem_params* params)
    {

        Vertex *v1 = e->v1(), *v2 = e->v2();
        if (v1 == v2) {
            gts_object_destroy(GTS_OBJECT (e));
            return NULL;
        }

        if (!gts_edge_collapse_is_valid(GTS_EDGE(e))) {
            GTS_OBJECT(e)->reserved =
                gts_eheap_insert_with_key(heap, e, G_MAXDOUBLE);
            return NULL;
        }


        Quadric* q1 = static_cast<Quadric*>(v1->reserved);
        Quadric* q2 = static_cast<Quadric*>(v2->reserved);
        Quadric* qe = static_cast<Quadric*>(e->reserved);
        Quadric quadric = *q1 + *q2 - *qe;
        Vector minvec;
        if (!quadric.get_min_vector(minvec)) {
            GtsPoint* p1 = GTS_POINT(v1), *p2 = GTS_POINT(v2);
            Vector vec1(p1->x, p1->y, p1->z);
            Vector vec2(p2->x, p2->y, p2->z);
            Vector vec0 = (vec1 + vec2) / 2.0;
            gdouble c1 = quadric.value(vec1);
            gdouble c2 = quadric.value(vec2);
            gdouble c0 = quadric.value(vec0);
            if (c1 < c2 && c1 < c0) minvec = vec1;
            else if (c2 < c1 && c2 < c0) minvec = vec2;
            else minvec = vec0;
        }
        *q1 += *q2;
        v1->replace(*v2);
        delete q2;
        v2->reserved = NULL;
        delete qe;
        e->reserved = NULL;
        gts_object_destroy(GTS_OBJECT(e));
        gts_object_destroy(GTS_OBJECT(v2));
        GtsPoint* p1 = GTS_POINT(v1);
        p1->x = minvec.x;
        p1->y = minvec.y;
        p1->z = minvec.z;
        GSList* i = v1->edges();
        while (i) {
            Edge* e1 = EDGE(i->data);
            Edge* e2;
            while (e2 = e1->duplicate()) {
                e1->replace(*e2);
                Quadric* qe1 = static_cast<Quadric*>(e1->reserved);
                Quadric* qe2 = static_cast<Quadric*>(e2->reserved);
                *qe1 += *qe2;
                HEAP_REMOVE_EDGE(heap, e2);
                delete qe2;
                e2->reserved = NULL;
                gts_object_destroy(GTS_OBJECT(e2));
            }
            i = i->next;
            if (!e1->faces()) {
                re_error << "isolated edge found.\n" << e1 << std::endl;
                HEAP_REMOVE_EDGE(heap, e1);
                gts_object_destroy(GTS_OBJECT(e1));
                if (i == NULL)
                    v1 = NULL;
            }
        }
        return v1;

    }

    gdouble qemu_cost(Edge* e, Qem_params* params)
    {
        Vertex* v1 = e->v1(), * v2 = e->v2();
        Quadric* q1 = static_cast<Quadric*>(v1->reserved);
        Quadric* q2 = static_cast<Quadric*>(v2->reserved);
        Quadric* qe = static_cast<Quadric*>(e->reserved);
        Quadric quadric = *q1 + *q2 - *qe;
        Vector minvec;
        if (!quadric.get_min_vector(minvec)) {
            GtsPoint* p1 = GTS_POINT(v1), *p2 = GTS_POINT(v2);
            Vector vec1(p1->x, p1->y, p1->z);
            Vector vec2(p2->x, p2->y, p2->z);
            Vector vec0 = (vec1 + vec2) / 2.0;
            gdouble c1 = quadric.value(vec1);
            gdouble c2 = quadric.value(vec2);
            gdouble c0 = quadric.value(vec0);
            if (c1 < c2 && c1 < c0) minvec = vec1;
            else if (c2 < c1 && c2 < c0) minvec = vec2;
            else minvec = vec0;
        }
        gdouble cost = quadric.value(minvec);
        if (e->is_boundary()) cost /= params->border_weight;
        return cost;
    }
} // end namespace for edge collapse


void Surface::edge_collapse(guint nedge, gpointer params,
        SubvFunc fsubv, CostFunc fcost, StopFunc fstop)
{
    g_return_if_fail(fsubv != NULL && fcost != NULL && fstop != NULL);

    GtsEHeap* heap = gts_eheap_new(fcost, params);
    gts_eheap_freeze(heap);
    foreach_edge((GtsFunc) create_heap_coarsen, heap);
    gts_eheap_thaw(heap);

    Edge* e = NULL;
    gdouble top_cost = G_MAXDOUBLE;
    gts_allow_floating_edges = TRUE;
    while ((e = EDGE(gts_eheap_remove_top(heap, &top_cost)))
            && (top_cost < G_MAXDOUBLE)
            && !(*fstop) (top_cost,
                gts_eheap_size(heap) - e->faces_number(), &nedge))
    {
        Vertex* v = fsubv(heap, e, params);
        if (v != NULL)
            update_neighbors(v, heap);
    }
    gts_allow_floating_edges = FALSE;

    if (e) GTS_OBJECT(e)->reserved = NULL;
    gts_eheap_foreach(heap, (GFunc) gts_object_reset_reserved, NULL);
    gts_eheap_destroy(heap);
}


void Surface::qem_coarsen(Qem_params params)
{
    foreach_vertex((GtsFunc) avertex_new_quadric, NULL);
    if (params.is_union) {
        foreach_edge((GtsFunc) one_edge_new_quadric, NULL);
    }
    foreach_face((GtsFunc) add_face_quadric, &params);
    foreach_edge((GtsFunc) add_border_quadric, &params);
    SubvFunc fsubv = (SubvFunc) qem_subv;
    CostFunc fcost = (CostFunc) qem_cost;
    StopFunc fstop = (StopFunc) edge_number_stop;
    if (params.is_union) {
        fsubv = (SubvFunc) qemu_subv;
        fcost = (CostFunc) qemu_cost;
    }
    if (params.is_verbose) {
        fstop = (StopFunc) edge_number_stop_verbose;
    }
    edge_collapse(params.nedge, &params, fsubv, fcost, fstop);
    foreach_vertex((GtsFunc) one_vertex_delete_quadric, NULL);
    if (params.is_union) {
        foreach_edge((GtsFunc) one_edge_delete_quadric, NULL);
    }
}


void Surface::fme_coarsen(Fme_params p)
{
    GtsCoarsenFunc fsubv = (GtsCoarsenFunc) gts_volume_optimized_vertex;
    GtsKeyFunc fcost = (GtsKeyFunc) gts_volume_optimized_cost;
    GtsStopFunc fstop = (GtsStopFunc) edge_number_stop;
    if (p.is_verbose) {
        fstop = (StopFunc) edge_number_stop_verbose;
    }
    gts_surface_coarsen(_surface, fcost, &p, fsubv, &p, fstop, &p.nedge, 0.01);
}


void Surface::show(GLenum mode)
{
    _self = this;
    _state.draw_mode = mode;

    display::show_init(*_self, _state);
    if (_state.draw_mode & display::POINTS)     list_points(_state, *_self);
    if (_state.draw_mode & display::LINES)      list_lines(_state, *_self);
    if (_state.draw_mode & display::TRIANGLES)  list_triangles(_state, *_self);
    //list_axis(_state, 1.0, 1.0, 1.0);

    glutDisplayFunc(displayfunc);
    glutReshapeFunc(reshapefunc);
    glutMouseFunc(mousefunc);
    glutKeyboardFunc(keyboardfunc);
    glutSpecialFunc(specialfunc);
    glutMotionFunc(motionfunc);
    //glutIdleFunc(idlefunc);
    glutMainLoop();
}


void Surface::read_ply(std::istream& is)
{
    std::string line;
    std::stringstream ss;
    get_valid_line(is, line);
    g_return_if_fail(line == PLY_NAME);
    get_valid_line(is, line);
    g_return_if_fail(line == PLY_ASCII);

    guint nv = 0, nf = 0;
    while (get_valid_line(is, line) && (line != PLY_END_HEADER)) {
        if (line.find(PLY_ELEMENT_VERTEX) != std::string::npos) {
            ss.clear(); ss.str(line);
            std::string element, vertex;
            ss >> element >> vertex >> nv;
        }
        else if (line.find(PLY_ELEMENT_FACE) != std::string::npos) {
            ss.clear(); ss.str(line);
            std::string element, face;
            ss >> element >> face >> nf;
        }
    }

    Vertex** vertices = new Vertex*[nv];
    gdouble x, y, z;
    for (size_t i = 0; i != nv; ++i) {
        get_ssline(is, ss);
        ss >> x >> y >> z;
        vertices[i] = vertex_new(get_vclass(), x, y, z);
    }

    guint fn, f1, f2, f3;
    for (size_t i = 0; i != nf; ++i) {
        get_ssline(is, ss);
        ss >> fn >> f1 >> f2 >> f3;
        if (fn == 3) {
            Edge* e1 = get_edge(get_eclass(), vertices[f1], vertices[f2]);
            Edge* e2 = get_edge(get_eclass(), vertices[f2], vertices[f3]);
            Edge* e3 = get_edge(get_eclass(), vertices[f3], vertices[f1]);
            Face* new_face = face_new(get_fclass(), e1, e2, e3);
            add_face(new_face);
        }
        else {
            re_error << "not puraly triangulated surface!\n";
            return;
        }
    }
    delete [] vertices;
}

void Surface::read_gts(std::istream& is)
{
    std::string line;
    std::stringstream ss;

    size_t nv, ne, nf;
    get_ssline(is, ss);
    ss >> nv >> ne >> nf;

    Vertex** vs = new Vertex*[nv];
    gdouble x, y, z;
    for (size_t i = 0; i != nv; ++i) {
        get_ssline(is, ss);
        ss >> x >> y >> z;
        vs[i] = vertex_new(get_vclass(), x, y, z);
    }

    size_t j1, j2, j3;

    Edge** es = new Edge*[ne];
    for (size_t i = 0; i != ne; ++i) {
        get_ssline(is, ss);
        ss >> j1 >> j2;
        es[i] = edge_new(get_eclass(), vs[j1-1], vs[j2-1]);
    }
    delete [] vs;

    for (size_t i = 0; i != nf; ++i) {
        get_ssline(is, ss);
        ss >> j1 >> j2 >> j3;
        Face* face = face_new(get_fclass(), es[j3-1], es[j2-1], es[j1-1]);
        add_face(face);
    }
    delete [] es;
}

namespace
{
    void vertex_new_reserved_write(Vertex* v, gpointer* data)
    {
        std::ostream* os = static_cast<std::ostream*>(data[0]);
        size_t* vid = static_cast<size_t*>(data[1]);
        g_return_if_fail(v->reserved == NULL);
        v->reserved = new size_t(++(*vid));
        GtsPoint* p = v->p();
        *os << p->x << " " << p->y << " " << p->z << "\n";
    }
    void edge_new_reserved_write(Edge* e, gpointer* data)
    {
        std::ostream* os = static_cast<std::ostream*>(data[0]);
        //size_t* vid = static_cast<size_t*>(data[1]);
        size_t* eid = static_cast<size_t*>(data[2]);
        g_return_if_fail(e->reserved == NULL);
        e->reserved = new size_t(++(*eid));
        size_t e1 = *static_cast<size_t*>(e->v1()->reserved);
        size_t e2 = *static_cast<size_t*>(e->v2()->reserved);
        *os << e1 << " " << e2 << "\n";
    }
    void vertex_delete_reserved(Vertex* v, gpointer)
    {
        if (v->reserved != NULL) {
            delete static_cast<size_t*>(v->reserved);
            v->reserved = NULL;
        }
    }
    void edge_delete_reserved(Edge* e, gpointer)
    {
        if (e->reserved != NULL) {
            delete static_cast<size_t*>(e->reserved);
            e->reserved = NULL;
        }
    }
    void face_write_ply(Face* f, gpointer* data)
    {
        std::ostream* os = static_cast<std::ostream*>(data[0]);
        size_t f1 = *static_cast<size_t*>(f->v1()->reserved);
        size_t f2 = *static_cast<size_t*>(f->v2()->reserved);
        size_t f3 = *static_cast<size_t*>(f->v3()->reserved);
        *os << "3 " << f1 << " " << f2 << " " << f3 << "\n";
    }
    void face_write_gts(Face* f, gpointer* data)
    {
        std::ostream* os = static_cast<std::ostream*>(data[0]);
        size_t f1 = *static_cast<size_t*>(f->e1()->reserved);
        size_t f2 = *static_cast<size_t*>(f->e2()->reserved);
        size_t f3 = *static_cast<size_t*>(f->e3()->reserved);
        *os << f1 << " " << f2 << " " << f3 << "\n";
    }
}
void Surface::write_ply(std::ostream& os) const
{
    GtsSurfaceStats stats;
    get_stats(stats);
    guint nv = stats.edges_per_vertex.n;
    guint ne = stats.faces_per_edge.n;
    guint nf = stats.n_faces;
    os << "ply\n"
        "format ascii 1.0\n"
        "comment generated by " << PACKAGE_STRING << "\n"
        "comment developed by " << PACKAGE_BUGREPORT << "\n"
        "element vertex " << nv << "\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face " << nf << "\n"
        "property list uchar int vertex_indices\n"
        "end_header\n";

    size_t vid = -1;
    gpointer data[] = {&os, &vid};
    foreach_vertex((GtsFunc) vertex_new_reserved_write, data);
    foreach_face((GtsFunc) face_write_ply, data);
    foreach_vertex((GtsFunc) vertex_delete_reserved, data);
}

void Surface::write_gts(std::ostream& os) const
{
    GtsSurfaceStats stats;
    get_stats(stats);
    guint nv = stats.edges_per_vertex.n;
    guint ne = stats.faces_per_edge.n;
    guint nf = stats.n_faces;
    os << nv << " " << ne << " " << nf << " "
        << GTS_OBJECT(_surface)->klass->info.name << " "
        << GTS_OBJECT_CLASS(get_fclass())->info.name << " "
        << GTS_OBJECT_CLASS(get_eclass())->info.name << " "
        << GTS_OBJECT_CLASS(get_vclass())->info.name << "\n";

    size_t vid = 0, eid = 0;
    gpointer data[] = {&os, &vid, &eid};
    foreach_vertex((GtsFunc) vertex_new_reserved_write, data);
    foreach_edge((GtsFunc) edge_new_reserved_write, data);
    foreach_face((GtsFunc) face_write_gts, data);
    foreach_vertex((GtsFunc) vertex_delete_reserved, data);
    foreach_edge((GtsFunc) edge_delete_reserved, data);
}

void Surface::destroy()
{
    gts_object_destroy(GTS_OBJECT(_surface));
    _surface = NULL;
}


