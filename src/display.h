
#ifndef  display_h_INC
#define  display_h_INC

#include <GL/freeglut.h>
#include <cstring>

namespace display { // beg namespace display

class State
{
public:
    State():
        window_width(700), window_height(700),
        scale(Vector(1.0, 1.0, 1.0))
    {
        glut_argc = 1;
        glut_argv = new char*[glut_argc];
        glut_argv[0] = new char[20];
        strncpy(glut_argv[0], "reviewer", 20);
    }
    ~State()
    {
        for (int i = 0; i != glut_argc; ++i)
            delete [] glut_argv[i];
    }
    int glut_argc;
    char** glut_argv;

    GLuint window_width, window_height;

    GLuint draw_mode;

    GLuint axis_list, point_list, line_list, triangle_list;

    GLboolean mouse_left, mouse_right, mouse_middle;
    GLfloat   mouse_xpos, mouse_ypos;

    Vector scale, move, rotate;
    Vector model_move, model_scale, model_rotate, model_center;
    gdouble model_rangle, model_maxlen;

};

enum Attrenum
{
    OFF = 0, ON = 1 << 8,
    POINTS = 1, LINES = 1 << 1, TRIANGLES = 1 << 2,
    BRASS = 1, RUBY = 1 << 1, EMERALD = 1 << 2,
    RED = 1, GREEN = 1 << 1, BLUE = 1 << 2, GRAY = 1 << 3,
    WHITE = RED|GREEN|BLUE
};


const GLfloat light0_pos[] = {0.0, 0.0, -1.0, 0.0};
const GLfloat light1_pos[] = {1.0, 1.0, -1.0, 1.0};
const GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
const GLfloat red[]   = {1.0, 0.0, 0.0, 1.0};
const GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
const GLfloat blue[]  = {0.0, 0.0, 1.0, 1.0};
const GLfloat gray1[] = {0.1, 0.1, 0.1, 1.0};
const GLfloat gray[]  = {0.3, 0.3, 0.3, 1.0};
const GLfloat dark[]  = {0.0, 0.0, 0.0, 1.0};
const GLfloat brass_ambient[] =    {0.33, 0.22, 0.03, 1.0};
const GLfloat brass_diffuse[] =    {0.78, 0.57, 0.11, 1.0};
const GLfloat brass_specular[] =   {0.99, 0.91, 0.81, 1.0};
const GLfloat brass_shininess =    27.8;
const GLfloat ruby_ambient[] =     {0.10, 0.10, 0.01, 1.0};
const GLfloat ruby_diffuse[] =     {0.75, 0.10, 0.10, 1.0};
const GLfloat ruby_specular[] =    {0.73, 0.56, 0.56, 1.0};
const GLfloat ruby_shininess =     32.0;
const GLfloat emerald_ambient[] =  {0.02, 0.17, 0.02, 1.0};
const GLfloat emerald_diffuse[] =  {0.07, 0.61, 0.07, 1.0};
const GLfloat emerald_specular[] = {0.63, 0.72, 0.63};
const GLfloat emerald_shininess =  76.8;


void set_light(GLenum light, GLenum mode);
void display(State& stt);
void reshape(State& stt, int width, int height);
void mouse(State& stt, int button, int state, int x, int y);
void keyboard(State& stt, unsigned char key, int x, int y);
void special(State& stt, int key, int x, int y);
void motion(State& stt, int x, int y);

inline void light0_select(int select) {set_light(GL_LIGHT0, select);}
inline void light1_select(int select) {set_light(GL_LIGHT1, select);}
inline void menu_select(int select) {glutPostRedisplay();}
void shade_select(int select);
void material_select(State& stt, int mode);
void polygon_select(int mode);
void menu_key(int key);
void list_axis(State& stt, GLfloat x, GLfloat y, GLfloat z);
void list_material(const GLfloat *ambient, const GLfloat *diffuse,
                   const GLfloat *specular, const GLfloat shininess);

inline void draw_point(GtsPoint* p, gpointer)
{
    glVertex3f(p->x, p->y, p->z);
}

inline void draw_line(GtsSegment* s, gpointer)
{
    GtsPoint* p1 = GTS_POINT(s->v1);
    GtsPoint* p2 = GTS_POINT(s->v2);
    glVertex3f(p1->x, p1->y, p1->z);
    glVertex3f(p2->x, p2->y, p2->z);
}

inline void draw_triangle(GtsTriangle* t, gpointer)
{
    Face* f = FACE(t);
    GtsPoint* p1 = f->p1(), * p2 = f->p2(), * p3 = f->p3();
    Vector n = f->normal_uniform();
    glNormal3d(n.x, n.y, n.z);
    glVertex3f(p1->x, p1->y, p1->z);
    glVertex3f(p2->x, p2->y, p2->z);
    glVertex3f(p3->x, p3->y, p3->z);
}

template <typename T>
void list_points(State& stt, const T& obj)
{
    glNewList(stt.point_list, GL_COMPILE);

    glPointSize(2.0);
    glLineWidth(2.0);
    glColor3f(0.5, 0.0, 0.0);
    glBegin(GL_POINTS);
    obj.foreach_vertex((GtsFunc) draw_point, NULL);
    glEnd();

    glEndList();
}

template <typename T>
void list_lines(State& stt, const T& obj)
{
    glNewList(stt.line_list, GL_COMPILE);

    glLineWidth(2.0);
    glColor3f(0.0, 0.0, 0.5);
    glBegin(GL_LINES);
    obj.foreach_edge((GtsFunc) draw_line, NULL);
    glEnd();

    glEndList();
}

template <typename T>
void list_triangles(State& stt, const T& obj)
{
    glNewList(stt.triangle_list, GL_COMPILE);

    glPointSize(2.0);
    glLineWidth(2.0);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLES);
    obj.foreach_face((GtsFunc) draw_triangle, NULL);
    glEnd();

    glEndList();
}

template <typename T>
void show_init(const T& obj, State& stt)
{
    // 窗口系统
    glutInit(&stt.glut_argc, stt.glut_argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(stt.window_width, stt.window_height);
    glutCreateWindow(stt.glut_argv[0]);
    glClearColor(1.0, 1.0, 1.0, 0.0);

    // 坐标系统
    GtsBBox* bbox = obj.gen_bbox();
    stt.model_maxlen = bbox->x2 - bbox->x1;
    stt.model_maxlen = stt.model_maxlen > bbox->y2 - bbox->y1 ?
        stt.model_maxlen : bbox->y2 - bbox->y1;
    stt.model_maxlen = stt.model_maxlen > bbox->z2 - bbox->z1 ?
        stt.model_maxlen : bbox->z2 - bbox->z1;
    stt.model_center.x = 0.5 * (bbox->x1 + bbox->x2);
    stt.model_center.y = 0.5 * (bbox->y1 + bbox->y2);
    stt.model_center.z = 0.5 * (bbox->z1 + bbox->z2);
    gts_object_destroy(GTS_OBJECT(bbox));

    glOrtho(stt.model_center.x - stt.model_maxlen,
            stt.model_center.x + stt.model_maxlen,
            stt.model_center.y - stt.model_maxlen,
            stt.model_center.y + stt.model_maxlen,
            stt.model_center.z - stt.model_maxlen,
            stt.model_center.z + stt.model_maxlen);

    // 光照系统
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, gray1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, gray);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, dark);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, gray);

    // 状态控制
    glFrontFace(GL_CCW);
    glShadeModel(GL_FLAT);
    glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
    glCullFace(GL_BACK);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    //glEnable(GL_CULL_FACE);

    // 显示列表
    stt.point_list = glGenLists(1);
    stt.line_list = glGenLists(1);
    stt.triangle_list = glGenLists(1);
    stt.axis_list = glGenLists(1);

    // 选择列表
    light0_select(GRAY);
    light1_select(GRAY);
    menu_key(GLUT_RIGHT_BUTTON);
}
}// end namespace display
#endif   // ----- #ifndef display_h_INC  -----
