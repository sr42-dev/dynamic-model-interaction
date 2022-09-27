#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <bits/stdc++.h>

using namespace std;

constexpr auto PI = 3.14;
GLuint index = -1;

float theta = 0,phi=0;
float camX = 0, camY =0, camZ = 10;
float prev_mx =0, curr_mx = 0;
float prev_my = 0, curr_my = 0;

float sphx = 0.6, sphy = 0, sphz = 0;
float pyrx = 1.8, pyry = -0.25, pyrz = 0;
float hexpx = -0.6, hexpy = -0.25, hexpz = 0;

//Variables to handle rotation and scaling
float sphscale = 0.55;
float pyrscale = 1;
float hexpscale = 1;

float sphrot = 0;
float pyrrot = 0;
float hexaprot = 0;
//Use the same axis as there position

//For rendering Sphere
#define NFACE 20
#define NVERTEX 12
#define X .525731112119133606 
#define Z .850650808352039932

//Handling the dynamic backgroung
float red = 1, green = 1, blue = 1;

//Handling robotic rotations
long timer = 0;

//normalize given vector
void normalize(GLfloat v[3]) {
    GLfloat d = sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
    if (d == 0.0) return;
    v[0] /= d;
    v[1] /= d;
    v[2] /= d;

}


void drawTriangle(GLfloat* v1, GLfloat* v2, GLfloat* v3)
{
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glBegin(GL_TRIANGLES);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glEnd();
}

void subDivide(GLfloat* v1, GLfloat* v2, GLfloat* v3, int depth) {
    if (depth == 0) {
        drawTriangle(v1, v2, v3);
        return;
    }
    GLfloat v12[3];
    GLfloat v23[3];
    GLfloat v31[3];
    for (int i = 0; i < 3; i++) {
        v12[i] = v1[i] + v2[i];
        v23[i] = v2[i] + v3[i];
        v31[i] = v3[i] + v1[i];
    }
    normalize(v12);
    normalize(v23);
    normalize(v31);

    subDivide(v1, v12, v31, depth - 1);
    subDivide(v2, v23, v12, depth - 1);
    subDivide(v3, v31, v23, depth - 1);
    subDivide(v12, v23, v31, depth - 1);
}

static GLfloat vdata[NVERTEX][3] = {
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};

static GLint tindices[NFACE][3] = {
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

int mulX = 1,mulY = 1, mulZ = 1;

float r = 3.0;

void timerCallBack() {
   
    float rad = theta * PI / 100;

    srand(time(0));


    if (index != 2)
    {
        sphx = sphx + mulX * ((double)rand() / (RAND_MAX)) / 50;
        sphy = sphy + mulX * ((double)rand() / (RAND_MAX)) / 50;
        sphz = sphz + mulX * ((double)rand() / (RAND_MAX)) / 50;
        //sphrot += ((double)rand() / (RAND_MAX))*3;
    }

    if (index != 1)
    {
        pyrx = pyrx + mulY * ((double)rand() / (RAND_MAX)) / 50;
        pyry = pyry + mulY * ((double)rand() / (RAND_MAX)) / 50;
        pyrz = pyrz + mulY * ((double)rand() / (RAND_MAX)) / 50;
        //pyrrot += ((double)rand() / (RAND_MAX)) * 3;
    }

    if (index != 3)
    {
        hexpx = hexpx + mulZ * ((double)rand() / (RAND_MAX)) / 50;
        hexpy = hexpy + mulZ * ((double)rand() / (RAND_MAX)) / 50;
        hexpz = hexpz + mulZ * ((double)rand() / (RAND_MAX)) / 50;
        //hexaprot += ((double)rand() / (RAND_MAX)) * 3;
    }

    float dist1 = pow(sphx, 2.0f) + pow(sphy, 2.0f) + pow(sphz, 2.0f);
    float dist2 = pow(pyrx, 2.0f) + pow(pyry, 2.0f) + pow(pyrz, 2.0f);
    float dist3 = pow(hexpx, 2.0f) + pow(hexpy, 2.0f) + pow(hexpz, 2.0f);
    if (dist1 >= r*r) {
        mulX = -1 * mulX;
    }
    if (dist2 >= r*r) {
        mulY = -1 * mulY;
    }
    if (dist3 >= r*r) {
        mulZ = -1 * mulZ;
    }

    ++timer;
    if (index != 1 && index != 2 && index != 3)
    {
        if (timer >= 20)
        {
            hexaprot += 30;
            pyrrot += 45;
            sphrot += 60;
            timer = 1;
        }
    }
    delay(500);



    //Update background
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    red += ((double)rand() / (RAND_MAX))/500;
    green += ((double)rand() / (RAND_MAX))/500;
    blue += ((double)rand() / (RAND_MAX))/500;

    if (red > 1) red -= 1;
    if (green >= 1) green -= 1;
    if (blue >= 1) blue -= 1;
    //cout << red << " " << green << " " << blue << "\n";
    glClearColor(red, green, blue, 0);
    
    glutPostRedisplay();
}

void initLight() {
    float ambient[] = { 0.5, 0.5, 0.5, 1 };
    float diffuse[] = { 0.2, 0.2, 0.2, 1 };
    float specular[] = { 0, 0, 0, 1 };
    float position[] = { camX, camY, camZ, 1 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

}

void setMaterial(GLfloat ambientR, GLfloat ambientG, GLfloat ambientB,
    GLfloat diffuseR, GLfloat diffuseG, GLfloat diffuseB,
    GLfloat specularR, GLfloat specularG, GLfloat specularB,
    int shine) {

    float ambient[] = { ambientR, ambientG, ambientB };
    float diffuse[] = { diffuseR, diffuseG, diffuseB };
    float specular[] = { specularR, specularG, specularB };

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shine);

}

void hexagonalPrism(void) {
    srand(time(NULL));
    //First Side of prism (Hexagon)
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glVertex3f(0.0, 0.0, 0);
    glVertex3f(0.0, 0.4, 0);
    glVertex3f(-0.3, 0.2, 0);
    glVertex3f(-0.3, -0.2, 0);
    glVertex3f(0.3, 0.2, 0);
    glVertex3f(0.0, 0.4, 0);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glVertex3f(0.0, 0.0, 0);
    glVertex3f(-0.3, -0.2, 0);
    glVertex3f(0.0, -0.4, 0);
    glVertex3f(0.3, -0.2, 0);
    glVertex3f(0.3, 0.2, 0);
    glVertex3f(-0.3, -0.2, 0);
    glEnd();

    ///Second Side of prism (Hexagon)
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glVertex3f(0.0, 0.0, 1);
    glVertex3f(0.0, 0.4, 1);
    glVertex3f(-0.3, 0.2, 1);
    glVertex3f(-0.3, -0.2, 1);
    glVertex3f(0.3, 0.2, 1);
    glVertex3f(0.0, 0.4, 1);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glVertex3f(0.0, 0.0, 1);
    glVertex3f(-0.3, -0.2, 1);
    glVertex3f(0.0, -0.4, 1);
    glVertex3f(0.3, -0.2, 1);
    glVertex3f(0.3, 0.2, 1);
    glVertex3f(-0.3, -0.2, 1);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glVertex3f(0.0, 0.4, 0);
    glVertex3f(-0.3, 0.2, 0);
    glVertex3f(-0.3, 0.2, 1);
    glVertex3f(0.0, 0.4, 1);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glVertex3f(-0.3, 0.2, 0);
    glVertex3f(-0.3, -0.2, 0);
    glVertex3f(-0.3, -0.2, 1);
    glVertex3f(-0.3, 0.2, 1);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glVertex3f(-0.3, -0.2, 0);
    glVertex3f(0.0, -0.4, 0);
    glVertex3f(0.0, -0.4, 1);
    glVertex3f(-0.3, -0.2, 1);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glVertex3f(0.0, -0.4, 0);
    glVertex3f(0.3, -0.2, 0);
    glVertex3f(0.3, -0.2, 1);
    glVertex3f(0.0, -0.4, 1);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glVertex3f(0.3, -0.2, 0);
    glVertex3f(0.3, 0.2, 0);
    glVertex3f(0.3, 0.2, 1);
    glVertex3f(0.3, -0.2, 1);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glVertex3f(0.0, 0.4, 0);
    glVertex3f(0.3, 0.2, 0);
    glVertex3f(0.3, 0.2, 1);
    glVertex3f(0.0, 0.4, 1);
    glEnd();

    glutPostRedisplay();

}


void sqpyramid(float size)
{
    //glColor3f((GLfloat)184 / 255, (GLfloat)115 / 255, (GLfloat)51 / 255);
    srand(time(NULL));
    //4 triangles emerging from the same top vertex
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glBegin(GL_TRIANGLES);
    glVertex3f(-size, -size, -size);
    glVertex3f(size, -size, -size);
    glVertex3f(0.0, size, 0.0);
    glEnd();

    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glBegin(GL_TRIANGLES);
    glVertex3f(-size, -size, size);
    glVertex3f(size, -size, size);
    glVertex3f(0.0, size, 0.0);
    glEnd();

    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glBegin(GL_TRIANGLES);
    glVertex3f(-size, -size, size);
    glVertex3f(-size, -size, -size);
    glVertex3f(0.0, size, 0.0);
    glEnd();

    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glBegin(GL_TRIANGLES);
    glVertex3f(size, -size, size);
    glVertex3f(size, -size, -size);
    glVertex3f(0.0, size, 0.0);
    glEnd();

    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glBegin(GL_TRIANGLES);
    glVertex3f(-size, -size, size);
    glVertex3f(-size, -size, -size);
    glVertex3f(0.0, size, 0.0);
    glEnd();

    //Bottom Two Triangles that form the square base
    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glBegin(GL_TRIANGLES);
    glVertex3f(-size, -size, -size);
    glVertex3f(size, -size, -size);
    glVertex3f(size, -size, size);
    glEnd();

    glColor3f(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
    glBegin(GL_TRIANGLES);
    glVertex3f(-size, -size, -size);
    glVertex3f(-size, -size, size);
    glVertex3f(size, -size, size);
    glEnd();

    glutPostRedisplay();
}

void drawSphere(float maxDepth) {
    for (int i = 0; i < NFACE; i++) {
        srand(i);
        subDivide(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0], maxDepth);
    }
    glutPostRedisplay();
}

void mySpecialFunc(int key, int x, int y)
{
    //Triggers only when object is selected and arrow keys are pressed
    //Translation , Rotation and Scaling should happen

    if (index == 1)
    {
        //Sq pyramid
        if (key == GLUT_KEY_RIGHT)
        {
            pyrx += mulY*0.1;
        }
        else if (key == GLUT_KEY_LEFT)
        {
           pyrx -= mulY*0.1;
        }
        else if (key == GLUT_KEY_DOWN)
        {
           pyry -= mulY*0.1;
        }
        else if (key == GLUT_KEY_UP)
        {
            pyry += mulY*0.1;
        }
    }
    else if (index == 2)
    {
        if (key == GLUT_KEY_RIGHT)
        {
            sphx += mulX * 0.1;
        }
        else if (key == GLUT_KEY_LEFT)
        {
            sphx -= mulX * 0.1;
        }
        else if (key == GLUT_KEY_DOWN)
        {
            sphy -= mulX * 0.1;
        }
        else if (key == GLUT_KEY_UP)
        {
            sphy += mulX * 0.1;
        }
    }
    else if(index==3){
        if (key == GLUT_KEY_RIGHT)
        {
            hexpx += mulZ * 0.1;


        }
        else if (key == GLUT_KEY_LEFT)
        {
            hexpx -= mulZ * 0.1;
        }
        else if (key == GLUT_KEY_DOWN)
        {
            hexpy -= mulZ * 0.1;
        }
        else if (key == GLUT_KEY_UP)
        {
            hexpy += mulZ * 0.1;
        }

    }
    glutPostRedisplay();
}

void myKeyboardFunc(unsigned char key, int x, int y)
{
    if (index == 1)
    {
        //Z for zoom in and X for zoom out
        if (key == 'z')
        {
            pyrscale += 0.1;
        }
        else if (key == 'x')
        {
            pyrscale -= 0.1;    
        }
        else if (key == 'j') //Rotation
        {
            pyrrot -= 10;
        }
        else if (key == 'l')
        {
            pyrrot += 10;
        }

    }
    else if(index == 2)
    {
        if (key == 'z')
        {
            sphscale += 0.1;
        }
        else if (key == 'x')
        {
            sphscale -= 0.1;
        }
        else if (key == 'j') //Rotation
        {
            sphrot -= 10;
        }
        else if (key == 'l')
        {
            sphrot += 10;
        }
    }
    else if(index==3){
        if (key == 'z')
        {
            hexpscale += 0.1;
        }
        else if (key == 'x')
        {
            hexpscale -= 0.1;
        }
        else if (key == 'j') //Rotation
        {
            hexaprot -= 10;
        }
        else if (key == 'l')
        {
            hexaprot += 10;
        }
    }
}

void myMouseMotion(int x, int y)
{
    float rad_theta = theta * PI / 180;
    float rad_phi = phi * PI / 180;
    float radius = 9.0;
    curr_mx = x;
    curr_my = y;


    float eyeZ = radius * cos(rad_theta) * cos(rad_phi);
    float eyeX = radius * sin(rad_theta) * cos(rad_phi);
    float eyeY = radius * sin(rad_phi);
    float dtheta = (curr_mx - prev_mx) / 50;
    float dphi = (curr_my - prev_my) / 50;

    theta -= dtheta;
    phi -= dphi;
    camX = eyeX;
    camY = eyeY;
    camZ = eyeZ;

    glutPostRedisplay();
}

void myMouseFunction(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        prev_mx = curr_mx = x;
        prev_my = curr_my = y;

        //Using stencil buffer part
        float window_width = glutGet(GLUT_WINDOW_WIDTH);
        float window_height = glutGet(GLUT_WINDOW_HEIGHT);
        glReadPixels(x, window_height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
        //cout << "Clicked on Index :" << " " << index << "\n";
        if (index == 0) {
            glutSpecialFunc(NULL);
            glutKeyboardFunc(NULL);
        }
        else
        {
            glutSpecialFunc(mySpecialFunc);
            glutKeyboardFunc(myKeyboardFunc);
        }
    }
}


void display() {
    glClearStencil(0); // this is the default value
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);

    //Camera Adjusments
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camX,camY,camZ, 0, 0, 0, 0, 1, 0);
    setMaterial(0.2, 0.2, 0.2, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 70);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glPushMatrix();
    glTranslatef(pyrx, pyry, pyrz);
    glScalef(pyrscale, pyrscale, pyrscale);
    glRotatef(pyrrot, pyrx, pyry, pyrz);
    glStencilFunc(GL_ALWAYS, 1, -1);
    sqpyramid(0.45);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(sphx, sphy, sphz);
    glScalef(sphscale, sphscale, sphscale);
    glRotatef(sphrot, sphx, sphy, sphz);
    glStencilFunc(GL_ALWAYS, 2, -1);
    drawSphere(2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(hexpx, hexpy, hexpz);
    glScalef(hexpscale, hexpscale, hexpscale);
    glRotatef(hexaprot, hexpx, hexpy, hexpz);
    glStencilFunc(GL_ALWAYS, 3, -1);
    hexagonalPrism();
    glPopMatrix();

    
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitWindowSize(700, 700);
    glutInitWindowPosition(10, 10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);

    glutCreateWindow("Assignment-1");

    glEnable(GL_DEPTH_TEST);
    initLight();

    glEnable(GL_COLOR_MATERIAL);

    glClearColor(red, green, blue, 0);

    glutDisplayFunc(display);


    glutIdleFunc(timerCallBack);
    glutMotionFunc(myMouseMotion);
    glutMouseFunc(myMouseFunction);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // gluOrtho2D(0, 640, 0, 640);
    //glOrtho(-3, 3, -3, 3, -3, 4);
    //    glPointSize(10);
    //gluPerspective(40, 1, -25, 1000);
    gluPerspective(50, 1, 0.5, 1000);

    glutMainLoop();
}