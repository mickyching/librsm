#include "ilp.h"
#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <vector>
#include "simstr.h"
#include "config.h"
const std::string Ilp::ILP_ASCII              = "ilp ascii";
const std::string Ilp::ILP_SUFFIX             = ".ilp";

Ilp* Ilp::_self = NULL;
display::State Ilp::_state;
guint Ilp::_nblock = 0;
guint Ilp::_nlayer = 0;


gboolean Ilp::is_ilp_filename(const std::string& fname)
{
    return (fname.rfind(ILP_SUFFIX) == fname.length() - ILP_SUFFIX.length());
}


void Ilp::displayfunc1()
{
    display::display(_state);
}

namespace
{
    void draw_layer(const Ilp& ilp, GLuint mode, guint* nlayer, guint* nblock)
    {
    #define frand() ((double) rand() / (RAND_MAX+1.0))
        srand(0);
        glPointSize(2.0);
        glLineWidth(2.0);
        g_return_if_fail(!ilp.is_empty());

        int layer_size = ilp.layer_number();
        if (*nlayer > layer_size) {
            *nlayer = layer_size - 1;
            *nblock = 0;
        }
        else if (*nlayer == layer_size) {
            *nlayer = 0;
            *nblock = 0;
        }

        int block_size = ilp[*nlayer].size();
        if (*nblock > block_size) {
            *nblock = block_size - 1;
        }
        else if (*nblock == ilp[*nlayer].size()) {
            *nblock = 0;
        }

        if (mode & display::LINES) {
            glBegin(GL_LINES);
            for (guint bi = 0; bi <= *nblock; ++bi) {
                glColor3f(frand(), frand(), frand());
                for (Ilp::edge_const_iterator ei = ilp[*nlayer][bi].begin();
                        ei != ilp[*nlayer][bi].end(); ++ei) {
                    display::draw_line(GTS_SEGMENT(*ei), NULL);
                }
            }
            glEnd();
        }
    #undef frand
    }
}

void Ilp::displayfunc2()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    Vector tv = _state.move - _state.model_center;
    glTranslated(tv.x, tv.y, tv.z);
    glScaled(_state.scale.x, _state.scale.y, _state.scale.z);
    glRotated(_state.rotate.x, 1., 0., 0.);
    glRotated(_state.rotate.y, 0., 1., 0.);
    glRotated(_state.rotate.z, 0., 0., 1.);
    draw_layer(*_self, _state.draw_mode, &_nlayer, &_nblock);
    glCallList(_state.axis_list);
    //glCallList(_state.material_list);
    glPopMatrix();

    glutSwapBuffers();

}


void Ilp::reshapefunc(int width, int height)
{
    display::reshape(_state, width, height);
}


void Ilp::mousefunc(int button, int state, int x, int y)
{
    display::mouse(_state, button, state, x, y);
}


void Ilp::keyboardfunc(unsigned char key, int x, int y)
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
           case 'S':
                std::cout << "save ipc as(input file name): " << std::flush;
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

#define endcase glutPostRedisplay();break
    switch (key) {
        case 'A': _nlayer = 0; endcase;
        case 'Z': _nlayer = -1; endcase;
        case 'a': _nblock = 0; endcase;
        case 'z': _nblock = -1; endcase;
    }
#undef endcase
}


void Ilp::specialfunc(int key, int x, int y)
{
    display::special(_state, key, x, y);
#define endcase glutPostRedisplay();break
    switch (key) {
        case GLUT_KEY_F1: glutDisplayFunc(displayfunc1); endcase;
        case GLUT_KEY_F2: glutDisplayFunc(displayfunc2); endcase;
        case GLUT_KEY_DOWN: ++_nlayer; endcase;
        case GLUT_KEY_UP: --_nlayer; endcase;
        case GLUT_KEY_RIGHT: ++_nblock; endcase;
        case GLUT_KEY_LEFT: --_nblock; endcase;
    }
#undef endcase
}


void Ilp::motionfunc(int x, int y)
{
    display::motion(_state, x, y);
}


namespace
{
    void avertex_into_bbox(Vertex* v, GtsBBox* bbox)
    {
        v->enlarge_bbox(*bbox);
    }
}
GtsBBox* Ilp::gen_bbox(GtsBBoxClass* bbclass) const
{
    GtsBBox* b = gts_bbox_new(bbclass, NULL, 0., 0., 0., 0., 0., 0.);
    b->x1 = b->y1 = b->z1 = G_MAXDOUBLE;
    b->x2 = b->y2 = b->z2 = -G_MAXDOUBLE;
    foreach_vertex((GtsFunc) avertex_into_bbox, b);
    return b;
}


void Ilp::foreach_vertex(GtsFunc f, gpointer data) const
{
    for (const_iterator li = _ilp.begin(); li != _ilp.end(); ++li) {
        for (block_const_iterator bi = li->begin(); bi != li->end(); ++bi) {
            for (edge_const_iterator ei = bi->begin(); ei != bi->end(); ++ei) {
                f(GTS_SEGMENT(*ei)->v1, data);
                f(GTS_SEGMENT(*ei)->v2, data);
            }
        }
    }
}


void Ilp::foreach_vertex(GtsFunc f, gpointer data)
{
    for (iterator li = _ilp.begin(); li != _ilp.end(); ++li) {
        for (block_iterator bi = li->begin(); bi != li->end(); ++bi) {
            for (edge_iterator ei = bi->begin(); ei != bi->end(); ++ei) {
                f(GTS_SEGMENT(*ei)->v1, data);
                f(GTS_SEGMENT(*ei)->v2, data);
            }
        }
    }
}


void Ilp::foreach_edge(GtsFunc f, gpointer data) const
{
    for (const_iterator li = _ilp.begin(); li != _ilp.end(); ++li) {
        for (block_const_iterator bi = li->begin(); bi != li->end(); ++bi) {
            for (edge_const_iterator ei = bi->begin(); ei != bi->end(); ++ei) {
                f(*ei, data);
            }
        }
    }
}


void Ilp::foreach_edge(GtsFunc f, gpointer data)
{
    for (iterator li = _ilp.begin(); li != _ilp.end(); ++li) {
        for (block_iterator bi = li->begin(); bi != li->end(); ++bi) {
            for (edge_iterator ei = bi->begin(); ei != bi->end(); ++ei) {
                f(*ei, data);
            }
        }
    }
}


Ilp& Ilp::operator=(const Ilp& ilp)
{
    destroy();
    for (const_iterator li = ilp._ilp.begin(); li != ilp._ilp.end(); ++li) {
        layer bc;
        for (block_const_iterator bi = li->begin(); bi != li->end(); ++bi) {
            block ec;
            for (edge_const_iterator ei = bi->begin(); ei != bi->end(); ++ei) {
                GtsPoint* p1 = GTS_POINT(GTS_SEGMENT(*ei)->v1);
                GtsPoint* p2 = GTS_POINT(GTS_SEGMENT(*ei)->v2);
                Vertex* v1 = vertex_new(vertex_class(), p1->x, p1->y, p1->z);
                Vertex* v2 = vertex_new(vertex_class(), p2->x, p2->y, p2->z);
                Edge* edge = edge_new(edge_class(), v1, v2);
                ec.push_back(edge);
            }
            bc.push_back(ec);
        }
        _ilp.push_back(bc);
    }
    return *this;
}


void Ilp::read(const std::string& filename)
{
    g_return_if_fail(is_ilp_filename(filename));
    std::ifstream fin(filename.c_str());
    read(fin);
    fin.close();
}


void Ilp::read(std::istream& is)
{
    destroy();
    std::string line;
    std::stringstream ss;

    get_valid_line(is, line);
    g_return_if_fail(line == Ilp::ILP_ASCII);

    guint nlayer = 0;
    get_ssline(is, ss);
    ss >> nlayer;

    char c;
    gdouble z, x1, y1, x2, y2;
    for (guint i = 0; i != nlayer; ++i)
    {
        get_valid_line(is, line);
        g_return_if_fail(line[0] == '{');
        ss.clear();
        ss.str(line);
        ss >> c >> z;

        layer vec_block;
        while (true) {
            get_valid_line(is, line);
            if (line[0] == '}') break;
            g_return_if_fail(line[2] == '[');

            block vec_edge;
            while (true) {
                get_valid_line(is, line);
                if (line[2] == ']') break;
                ss.clear();
                ss.str(line);
                ss >> x1 >> y1 >> x2 >> y2;
                Vertex* v1 = vertex_new(vertex_class(), x1, y1, z);
                Vertex* v2 = vertex_new(vertex_class(), x2, y2, z);
                Edge* edge = edge_new(edge_class(), v1, v2);
                vec_edge.push_back(edge);
            }
            vec_block.push_back(vec_edge);
        }
        _ilp.push_back(vec_block);
    }
}


void Ilp::write(const std::string& filename) const
{
    g_return_if_fail(is_ilp_filename(filename));
    std::ofstream fout(filename.c_str());
    write(fout);
    fout.close();
}


void Ilp::write(std::ostream& out) const
{
    out << ILP_ASCII << "\n"
        << RE_COMMENT << " iso layer path\n"
        << RE_COMMENT << " generated by " << PACKAGE_STRING << "\n"
        << RE_COMMENT << " developed by " << PACKAGE_BUGREPORT << "\n"
        << _ilp.size() << std::endl;

    for (const_iterator iter = begin(); iter != end(); ++iter)
    {
        out << "{ ";    // block number
        if (!(*iter)[0].empty()) {
            out << ((*iter)[0][0])->v1()->p()->z << "\n";
        }
        else {
            std::cerr << "error write layer @ write\n";
        }
        for (block_const_iterator li = iter->begin(); li != iter->end(); ++li)
        {
            out << "  [ " << "\n";  // edge number
            for (edge_const_iterator ei = li->begin(); ei != li->end(); ++ei)
            {
                GtsPoint* p1 = EDGE(*ei)->v1()->p();
                GtsPoint* p2 = EDGE(*ei)->v2()->p();
                out << "    " << p1->x << " " << p1->y << " "
                    << p2->x << " " << p2->y << "\n";
            }
            out << "  ]\n";
        }
        out << "}\n";
    }
}


void Ilp::write_gcode(const std::string& fname) const
{
    std::ofstream fout(fname.c_str());
    write_gcode(fout);
    fout.close();
}


void Ilp::write_gcode(std::ostream& out) const
{
    g_return_if_fail(_np.method == Ncparams::ISO_LAYER);
    out.setf(std::ios::fixed, std::ios::floatfield);
    out.precision(3);
    guint line_num = 0;

    out << "N" << ++line_num << " G71 G90\n";
    write_ilp_gcode(out, &line_num);
    out << "N" << ++line_num << " G01 Z" << _np.zmax + _np.rise_height << "\n";
}


void Ilp::write_ilp_gcode(std::ostream& osm, guint* nline) const
{
    gdouble zmax = _np.zmax, zmin = _np.zmin;
    gdouble feed = _np.ilpfeed, rize = _np.ilprize;
    gboolean isback = false;
    for (const_iterator liter = begin(); liter != end(); ++liter) {
        for (block_const_iterator biter = liter->begin();
                biter != liter->end(); ++biter) {
            g_return_if_fail(!biter->empty());
            edge_const_iterator eiter = biter->begin();
            GtsPoint* p1 = GTS_POINT(GTS_SEGMENT(*eiter)->v1);
            if (p1->z + _np.ilprize >= _np.ilpzmax) continue;

            write_G0(osm, nline) << "Z" << zmax + _np.rise_height << "\n";
            write_G0(osm, nline) << "X" << p1->x << " Y" << p1->y << "\n";
            write_G0(osm, nline) << "Z" << p1->z + rize + feed << "\n";

            for (++eiter; eiter != biter->end(); ++eiter) {
                GtsPoint* p1 = GTS_POINT(GTS_SEGMENT(*eiter)->v1);
                GtsPoint* p2 = GTS_POINT(GTS_SEGMENT(*eiter)->v2);
                if (!isback) {
                    write_G1(osm, nline, p1->x, p1->y, p1->z + rize);
                    write_G1(osm, nline, p2->x, p2->y, p2->z + rize);
                    isback = true;
                }
                else {
                    write_G1(osm, nline, p2->x, p2->y, p2->z + rize);
                    write_G1(osm, nline, p1->x, p1->y, p1->z + rize);
                    isback = false;
                }
            }
        }
    }
}

namespace
{
    Vertex* get_vbeg(Vertex* v1, Vertex* v2, gdouble zpos)
    {
        GtsPoint* p1 = v1->p();
        GtsPoint* p2 = v2->p();
        g_return_val_if_fail(p1->z >= zpos, NULL);
        if (p2->z < zpos) {
            if (ABS(p2->z - p1->z) < Vertex::EPSILON) {
                return vertex_new(vertex_class(), p2->x, p2->y, zpos);
            }
            else {
                gdouble k = (zpos - p1->z) / (p2->z - p1->z);
                gdouble xpos = p1->x + k * (p2->x - p1->x);
                return vertex_new(vertex_class(), xpos, p2->y, zpos);
            }
        }
        return NULL;
    }

    Vertex* get_vend(Vertex* v1, Vertex* v2, gdouble zpos)
    {
        GtsPoint* p1 = v1->p();
        GtsPoint* p2 = v2->p();
        g_return_val_if_fail(p1->z <= zpos, NULL);
        if (p2->z > zpos) {
            if (ABS(p2->z - p1->z) < Vertex::EPSILON) {
                return vertex_new(vertex_class(), p1->x, p1->y, zpos);
            }
            else {
                gdouble k = (zpos - p1->z) / (p2->z - p1->z);
                gdouble xpos = p1->x + k * (p2->x - p1->x);
                return vertex_new(vertex_class(), xpos, p2->y, zpos);
            }
        }
        return NULL;
    }

    GSList* get_projection_edges(const Ipp::planar& vlist, gdouble zpos)
    {
        GSList* edges = NULL;
        Vertex* v1 = NULL, * v2 = NULL;
        Ipp::planar::const_iterator vpos = vlist.begin(), next = vlist.begin();
        ++next;
        while (vpos != vlist.end() && next != vlist.end()) {
            GtsPoint* p1 = (*vpos)->p(), * p2 = (*next)->p();
            if (v1 == NULL) {
                if (vpos == vlist.begin()) {
                    if (p1->z < zpos)
                        v1 = vertex_new(vertex_class(), p1->x, p1->y, zpos);
                }
                else {
                    v1 = get_vbeg(*vpos, *next, zpos);
                    ++vpos;
                    ++next;
                    continue;
                }
            }
            if (v1 != NULL) {
                Ipp::planar::const_iterator nnext = next;
                ++nnext;
                if (nnext == vlist.end()) {
                    if (p2->z < zpos)
                        v2 = vertex_new(vertex_class(), p2->x, p2->y, zpos);
                }
                else v2 = get_vend(*vpos, *next, zpos);
            }

            if (v2 != NULL) {
                Edge* e = edge_new(edge_class(), v1, v2);
                edges = g_slist_prepend(edges, e);
                v1 = NULL; v2 = NULL;
            }
            ++vpos;
            ++next;
        }
        return g_slist_reverse(edges);
    }

    GSList* get_projections(const Ipp& ipp, gdouble zpos)
    {
        GSList* ps = NULL;
        for (Ipp::const_iterator pi = ipp.begin(); pi != ipp.end(); ++pi)
        {
            ps = g_slist_prepend(ps, get_projection_edges(*pi, zpos));
        }
        return g_slist_reverse(ps);
    }

    bool is_same_block(Edge* e1, Edge* e2)
    {
        GtsPoint* pa = e1->v1()->p();
        GtsPoint* pb = e1->v2()->p();
        GtsPoint* p1 = e2->v1()->p();
        GtsPoint* p2 = e2->v2()->p();
        return (p1->x - Vertex::EPSILON < pb->x) &&
            (p2->x + Vertex::EPSILON > pa->x);
    }

    void get_layer(Ilp::layer& vec_block, GSList* projections)
    {
        while (projections != NULL) {
            GSList* ps = projections, * elist = NULL;
            Edge* e1 = NULL;
            Ilp::block vec_edge;
            while (ps != NULL) {
                if (ps->data == NULL) {
                    projections = g_slist_delete_link(projections, ps);
                    break;
                }
                if (e1 == NULL) {
                    elist = static_cast<GSList*>(ps->data);
                    e1 = EDGE(elist->data);
                    vec_edge.push_back(e1);
                    ps->data = g_slist_delete_link(elist, elist);
                }
                else {
                    elist = static_cast<GSList*>(ps->data);
                    Edge* e2 = EDGE(elist->data);
                    if (is_same_block(e1, e2)) {
                        vec_edge.push_back(e2);
                        ps->data = g_slist_delete_link(elist, elist);
                        e1 = e2;
                    }
                    else break;
                }
                ps = ps->next;
            }
            if (!vec_edge.empty()) vec_block.push_back(vec_edge);
        }
        std::cerr << "nblock: " << vec_block.size() << "\n";
    }

}


void Ilp::make_path(const Surface& s)
{
    destroy();
    Ipp ipp(_np);
    ipp.make_path(s);

    bool is_last_layer = false;
    for (gdouble zpos = _np.ilpzmax; !is_last_layer; zpos -= _np.ilpfeed) {
        if (zpos <= _np.ilpzmin) {
            is_last_layer = true;
            zpos = _np.ilpzmin + Vertex::EPSILON;
        }
        GSList* projections = get_projections(ipp, zpos);
        layer vec_block;
        get_layer(vec_block, projections);
        _ilp.push_back(vec_block);
    }
}

void Ilp::show(GLenum mode)
{
    _self = this;
    _state.draw_mode = mode;
    display::show_init(*_self, _state);
    if (_state.draw_mode & display::POINTS) list_points(_state, *_self);
    if (_state.draw_mode & display::LINES)  list_lines(_state, *_self);
    //list_axis(_state, 1.0, 1.0, 1.0);

    glutDisplayFunc(displayfunc1);
    glutReshapeFunc(reshapefunc);
    glutMouseFunc(mousefunc);
    glutKeyboardFunc(keyboardfunc);
    glutSpecialFunc(specialfunc);
    glutMotionFunc(motionfunc);
    //glutIdleFunc(idlefunc);
    glutMainLoop();

}


void Ilp::destroy()
{
    for (iterator li = _ilp.begin(); li != _ilp.end(); ++li) {
        for (block_iterator bi = li->begin(); bi != li->end(); ++bi) {
            for (edge_iterator ei = bi->begin(); ei != bi->end(); ++ei) {
                gts_object_destroy(GTS_OBJECT(*ei));
            }
        }
    }
    _ilp.clear();
}


