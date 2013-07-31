#include "arrow.hh"


arrow::arrow(float rx, float ry, float rz) : _rx(rx), _ry(ry), _rz(rz), _v(glGenLists(1)) {
    glMatrixMode(GL_MODELVIEW);
    glNewList(_v, GL_COMPILE);
    glRotatef(90, 1,0,0);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0,0.75,0);


    glColor3f(0.5f, 0.0f, 1.0f);
    glVertex3f(-0.75, -0.75, 0);
    glColor3f(0.5f, 0.0f, 1.0f);
    glVertex3f(0.75, -0.75, 0);
    glEnd();
    glEndList();
}

void arrow::setRx(float r) {
    _rx = r;
}

void arrow::setRy(float r) {
    _ry = r;
}

void arrow::setRz(float r) {
    _rz = r;
}

void arrow::addRx(float r) {
    _rx += r;
}

void arrow::addRy(float r) {
    _ry += r;
}

void arrow::addRz(float r) {
    _rz +=r;
}

void arrow::reset() {
    _rx = 0;
    _ry = 0;
    _rz = 0;
}

void arrow::draw() {
    glPushMatrix();
    glRotatef(_rx, 1,0,0);
    glRotatef(_ry, 0,1,0);
    glRotatef(_rz, 0,0,1);
    glCallList(_v);
    glPopMatrix();
}
