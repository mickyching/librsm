// =====================================================================================
//
//       Filename:  display.cpp
//
//    Description:
//
//        Version:  1.0
//        Created:  01/02/2013 04:27:54 AM
//       Revision:  none
//       Compiler:  gcc
//
//         Author:  Micky Ching (MC), mickyching@gmail.com
//   Organization:
//
// =====================================================================================
#include "remc.h"
#include "display.h"
namespace display { // beg namespace display

void list_axis(State& stt, GLfloat x, GLfloat y, GLfloat z)
{
    glNewList(stt.axis_list, GL_COMPILE);

    glLineWidth(3.0);

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(x, 0.0f, 0.0f);
    glEnd();
    glPushMatrix();
    glTranslatef(x, 0.0f, 0.0f);
    glRotatef(90.0f,0.0f,1.0f,0.0f);
    glutSolidCone(0.027,0.09,10,10);
    glPopMatrix();

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, y, 0.0f);
    glEnd();
    glPushMatrix();
    glTranslatef(0.0f, y, 0.0f);
    glRotatef(-90.0f,1.0f,0.0f,0.0f);
    glutSolidCone(0.027,0.09,10,10);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, z);
    glEnd();
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, z);
    glutSolidCone(0.027,0.09,10,10);
    glPopMatrix();

    glEndList();
}
void list_material(const GLfloat *ambient, const GLfloat *diffuse,
                   const GLfloat *specular, const GLfloat shininess)
{
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}


void shade_select(int mode)
{
    glShadeModel(mode);
    glutPostRedisplay();
}

void set_light(GLenum light, GLenum mode)
{
    glEnable(light);
    switch (mode) {
        case OFF:   glDisable(light); break;
        case RED:   glLightfv(light, GL_DIFFUSE, red); break;
        case GREEN: glLightfv(light, GL_DIFFUSE, green); break;
        case BLUE:  glLightfv(light, GL_DIFFUSE, blue); break;
        case GRAY:  glLightfv(light, GL_DIFFUSE, gray); break;
        case WHITE: glLightfv(light, GL_DIFFUSE, white); break;
    }
    glutPostRedisplay();
}

void display(State& stt)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    Vector tv = stt.move - stt.model_center;
    glTranslated(tv.x, tv.y, tv.z);
    glScaled(stt.scale.x, stt.scale.y, stt.scale.z);
    glRotated(stt.rotate.x, 1., 0., 0.);
    glRotated(stt.rotate.y, 0., 1., 0.);
    glRotated(stt.rotate.z, 0., 0., 1.);
    if (stt.draw_mode & POINTS)    glCallList(stt.point_list);
    if (stt.draw_mode & LINES)     glCallList(stt.line_list);
    if (stt.draw_mode & TRIANGLES) glCallList(stt.triangle_list);
    glCallList(stt.axis_list);
    glPopMatrix();

    glutSwapBuffers();
}
void reshape(State& stt, int width, int height)
{
    GLfloat w = width, h = height;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, w/h, stt.model_maxlen, 10*stt.model_maxlen);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(stt.model_center.x, stt.model_center.y,
            stt.model_center.z + 3 * stt.model_maxlen,
            stt.model_center.x, stt.model_center.y, stt.model_center.z,
            0, 1, 0);
}

void mouse(State& stt, int button, int state, int x, int y)
{
#define GLUT_WHEEL_UP 3
#define GLUT_WHEEL_DOWN 4
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) stt.mouse_left = GL_TRUE;
        if (button == GLUT_RIGHT_BUTTON) stt.mouse_right = GL_TRUE;
        if (button == GLUT_MIDDLE_BUTTON) stt.mouse_middle = GL_TRUE;
    }

    if (state == GLUT_UP && button == GLUT_WHEEL_UP) {
        stt.scale *= 1.11;
    }
    if (state == GLUT_UP && button == GLUT_WHEEL_DOWN) {
        stt.scale /= 1.11;
    }

    if (state == GLUT_UP) {
        if (button == GLUT_LEFT_BUTTON) stt.mouse_left = GL_FALSE;
        if (button == GLUT_RIGHT_BUTTON) stt.mouse_right = GL_FALSE;
        if (button == GLUT_MIDDLE_BUTTON) stt.mouse_middle = GL_FALSE;
    }
    stt.mouse_xpos = x;
    stt.mouse_ypos = y;

    glutPostRedisplay();
#undef GLUT_WHEEL_DOWN
#undef GLUT_WHEEL_UP
}

void keyboard(State& stt, unsigned char key, int x, int y)
{
#define endcase glutPostRedisplay();break
    switch (key) {
        case 'i': stt.rotate.x -= 10; endcase;
        case 'I': stt.rotate.x += 10; endcase;
        case 'j': stt.rotate.y -= 10; endcase;
        case 'J': stt.rotate.y += 10; endcase;
        case 'k': stt.rotate.z -= 10; endcase;
        case 'K': stt.rotate.z += 10; endcase;
    }
#undef endcase
}
void special(State& stt, int key, int x, int y)
{
#define endcase glutPostRedisplay();break
    switch (key) {
        case GLUT_KEY_HOME:
            stt.scale = Vector(1.0, 1.0, 1.0);
            stt.move = Vector(0.0, 0.0, 0.0);
            stt.rotate = Vector(0.0, 0.0, 0.0);
            endcase;
    }
#undef endcase
}

void motion(State& stt, int x, int y)
{
    if (stt.mouse_left) {
        stt.rotate.y = x;
        stt.rotate.x = y;
    }

    if (stt.mouse_middle) {
        stt.move.x += (x - stt.mouse_xpos) * stt.model_maxlen/stt.window_width;
        stt.move.y -= (y - stt.mouse_ypos) * stt.model_maxlen/stt.window_height;
        stt.mouse_xpos = x;
        stt.mouse_ypos = y;
    }

    glutPostRedisplay();
}

void material_select(int mode)
{
    glDisable(GL_COLOR_MATERIAL);
    switch (mode) {
        case OFF:
            glEnable(GL_COLOR_MATERIAL);
            break;
        case BRASS:
            list_material(brass_ambient, brass_diffuse,
                    brass_specular, brass_shininess);
            break;
        case RUBY:
            list_material(ruby_ambient, ruby_diffuse,
                    ruby_specular, ruby_shininess);
            break;
        case EMERALD:
            list_material(emerald_ambient, emerald_diffuse,
                    emerald_specular, emerald_shininess);
            break;
    }
    glutPostRedisplay();
}
void polygon_select(int mode)
{
    switch (mode) {
        case ON:  glDisable(GL_CULL_FACE); break;
        case OFF: glEnable(GL_CULL_FACE); break;
        case POINTS:    glPolygonMode(GL_FRONT, GL_POINT); break;
        case LINES:     glPolygonMode(GL_FRONT, GL_LINE); break;
        case TRIANGLES: glPolygonMode(GL_FRONT, GL_FILL); break;
    }
    glutPostRedisplay();
}

void menu_key(int key)
{
#define SHADE_ENTRIES() \
    glutAddMenuEntry("Flat", GL_FLAT); \
    glutAddMenuEntry("Smooth", GL_SMOOTH);
    int menu_shade = glutCreateMenu(shade_select);
    SHADE_ENTRIES();
#undef SHADE_ENTRIES

#define LIGHT_ENTRIES() \
    glutAddMenuEntry("Disable", OFF); \
    glutAddMenuEntry("Red", RED); \
    glutAddMenuEntry("Green", GREEN); \
    glutAddMenuEntry("Blue", BLUE); \
    glutAddMenuEntry("Gray", GRAY); \
    glutAddMenuEntry("White", WHITE);
    int menu_light0 = glutCreateMenu(light0_select);
    LIGHT_ENTRIES();
    int menu_light1 = glutCreateMenu(light1_select);
    LIGHT_ENTRIES();
#undef LIGHT_ENTRIES

#define MATERIAL_ENTRIES() \
    glutAddMenuEntry("None", OFF); \
    glutAddMenuEntry("Brass", BRASS); \
    glutAddMenuEntry("Emerald", EMERALD); \
    glutAddMenuEntry("Ruby", RUBY);
    int menu_material = glutCreateMenu(material_select);
    MATERIAL_ENTRIES();
#undef MATERIAL_ENTRIES

#define FACE_ENTRIES() \
    glutAddMenuEntry("Face", TRIANGLES); \
    glutAddMenuEntry("Line", LINES); \
    glutAddMenuEntry("Point", POINTS); \
    glutAddMenuEntry("Noback", OFF); \
    glutAddMenuEntry("Withback", ON);
    int menu_face = glutCreateMenu(polygon_select);
    FACE_ENTRIES();
#undef FACE_ENTRIES

    glutCreateMenu(menu_select);
    glutAddSubMenu("Shade", menu_shade);
    glutAddSubMenu("Light0", menu_light0);
    glutAddSubMenu("Light1", menu_light1);
    glutAddSubMenu("Material", menu_material);
    glutAddSubMenu("Surface", menu_face);
    glutAttachMenu(key);
}

}//end namespace display
