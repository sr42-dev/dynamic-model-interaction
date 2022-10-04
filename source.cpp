#include <unistd.h>
#include <GL/glut.h>
#include <bits/stdc++.h>

#ifndef MAIN_H
#define MAIN_H

#include <math.h>
#include <time.h>

#include <GL/freeglut.h>
#include <GL/glext.h>

#include <fstream>
#include <map>
#include <vector>

#include "./library/loadpng.h"
#include "./library/process_image.h"

#include "./library/gl_texture.h"

#define WIDTH 600
#define HEIGHT 600

#define INTERVAL 15

class Model {
  private:
    static int count_char(std::string &str, char ch) {
        int c = 0;
        int length = str.length() - 1;
        for (int i = 0; i < length; i++) {
            if (str[i] == ch)
                c++;
        }
        return c;
    }

    static bool has_double_slash(std::string &str) {
        int length = str.length() - 2;
        for (int i = 0; i < length; i++) {
            if (str[i] == '/' && str[i + 1] == '/')
                return true;
        }
        return false;
    }

    class Material {
      public:
        float *ambient;
        float *diffuse;
        float *specular;
        GLuint texture;

        Material(float *ambient, float *diffuse, float *specular) {
            this->ambient = ambient;
            this->diffuse = diffuse;
            this->specular = specular;
            this->texture = 0;
        }
    };

    class Face {
      public:
        int edge;
        int *vertices;
        int *texcoords;
        int normal;

        Face(int edge, int *vertices, int *texcoords, int normal = -1) {
            this->edge = edge;
            this->vertices = vertices;
            this->texcoords = texcoords;
            this->normal = normal;
        }
    };

    std::string prefix;
    std::vector<Material> materials;
    std::map<std::string, int> map_material;

    std::vector<float *> vertices;
    std::vector<float *> texcoords;
    std::vector<float *> normals;
    std::vector<Face> faces;

    GLuint list;

    void load_material(const char *filename) {
        std::string line;
        std::vector<std::string> lines;
        std::ifstream in(filename);
        if (!in.is_open()) {
            printf("Cannot load material %s\n", filename);
            return;
        }

        while (!in.eof()) {
            std::getline(in, line);
            lines.push_back(line);
        }
        in.close();

        Material *m;
        int count_material = 0;
        char str[40];
        std::string material;
        float *a, *d, *s;

        for (std::string &line : lines) {
            if (line[0] == 'n' && line[1] == 'e') {
                sscanf(line.c_str(), "newmtl %s", str);
                material = str;
                map_material[material] = count_material;
                count_material++;
                a = new float[4]{0.2f, 0.2f, 0.2f, 1.0f};
                d = new float[4]{0.8f, 0.8f, 0.8f, 1.0f};
                s = new float[4]{0.0f, 0.0f, 0.0f, 1.0f};
                materials.push_back(Material(a, d, s));
                m = &materials[materials.size() - 1];
            } else if (line[0] == 'K') {
                switch (line[1]) {
                case 'a':
                    sscanf(line.c_str(), "Ka %f %f %f", &a[0], &a[1], &a[2]);
                    break;
                case 'd':
                    sscanf(line.c_str(), "Kd %f %f %f", &d[0], &d[1], &d[2]);
                    break;
                case 's':
                    sscanf(line.c_str(), "Ks %f %f %f", &s[0], &s[1], &s[2]);
                    break;
                }
            } else if (line[0] == 'm' && line[1] == 'a') {
                sscanf(line.c_str(), "map_Kd %s", str);
                std::string file = prefix + str;
                Image img;
                Load_Texture_Swap(&img, file.c_str());
                glGenTextures(1, &(m->texture));
                glBindTexture(GL_TEXTURE_2D, m->texture);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.w, img.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.img);
                glBindTexture(GL_TEXTURE_2D, 0);
                Delete_Image(&img);
            }
        }
    }

    void add_face_3v(std::string &line) {
        int v0, v1, v2;
        sscanf(line.c_str(), "f %d %d %d", &v0, &v1, &v2);
        int *v = new int[3]{v0 - 1, v1 - 1, v2 - 1};
        faces.push_back(Face(3, v, NULL));
    }

    void add_face_3vt(std::string &line) {
        int v0, v1, v2, t0, t1, t2;
        sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &v0, &t0, &v1, &t1, &v2, &t2);
        int *v = new int[3]{v0 - 1, v1 - 1, v2 - 1};
        int *t = new int[3]{t0 - 1, t1 - 1, t2 - 1};
        faces.push_back(Face(3, v, t));
    }

    void add_face_3vn(std::string &line) {
        int v0, v1, v2, n;
        sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &v0, &n, &v1, &n, &v2, &n);
        int *v = new int[3]{v0 - 1, v1 - 1, v2 - 1};
        faces.push_back(Face(3, v, NULL, n - 1));
    }

    void add_face_3vtn(std::string &line) {
        int v0, v1, v2, t0, t1, t2, n;
        sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &t0, &n, &v1, &t1, &n, &v2, &t2, &n);
        int *v = new int[3]{v0 - 1, v1 - 1, v2 - 1};
        int *t = new int[3]{t0 - 1, t1 - 1, t2 - 1};
        faces.push_back(Face(3, v, t, n - 1));
    }

    void add_face_4v(std::string &line) {
        int v0, v1, v2, v3;
        sscanf(line.c_str(), "f %d %d %d %d", &v0, &v1, &v2, &v3);
        int *v = new int[4]{v0 - 1, v1 - 1, v2 - 1, v3 - 1};
        faces.push_back(Face(4, v, NULL));
    }

    void add_face_4vt(std::string &line) {
        int v0, v1, v2, v3, t0, t1, t2, t3;
        sscanf(line.c_str(), "f %d/%d %d/%d %d/%d %d/%d", &v0, &t0, &v1, &t1, &v2, &t2, &v3, &t3);
        int *v = new int[4]{v0 - 1, v1 - 1, v2 - 1, v3 - 1};
        int *t = new int[4]{t0 - 1, t1 - 1, t2 - 1, t3 - 1};
        faces.push_back(Face(4, v, t));
    }

    void add_face_4vn(std::string &line) {
        int v0, v1, v2, v3, n;
        sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d", &v0, &n, &v1, &n, &v2, &n, &v3, &n);
        int *v = new int[4]{v0 - 1, v1 - 1, v2 - 1, v3 - 1};
        faces.push_back(Face(4, v, NULL, n - 1));
    }

    void add_face_4vtn(std::string &line) {
        int v0, v1, v2, v3, t0, t1, t2, t3, n;
        sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &t0, &n, &v1, &t1, &n, &v2, &t2, &n, &v3,
               &t3, &n);
        int *v = new int[4]{v0 - 1, v1 - 1, v2 - 1, v3 - 1};
        int *t = new int[4]{t0 - 1, t1 - 1, t2 - 1, t3 - 1};
        faces.push_back(Face(4, v, t, n - 1));
    }

  public:
    float pos_x, pos_y, pos_z;

    void load(const char *filename) {
        std::string tmp = filename;
        prefix = "";
        int n = tmp.find_last_of('/') + 1;
        if (n > 0)
            prefix = tmp.substr(0, n);

        std::string line;
        std::vector<std::string> lines;
        std::ifstream in(filename);
        if (!in.is_open()) {
            printf("Cannot load model %s\n", filename);
            return;
        }

        while (!in.eof()) {
            std::getline(in, line);
            lines.push_back(line);
        }
        in.close();

        float a, b, c;
        char str[40];

        pos_x = pos_y = 0.0f;

        float sum_x = 0.0f, sum_y = 0.0f, sum_z = 0.0f;

        for (std::string &line : lines) {
            if (line[0] == 'v') {
                if (line[1] == ' ') {
                    sscanf(line.c_str(), "v %f %f %f", &a, &b, &c);
                    if (a > 0.0f)
                        sum_x += a;
                    else
                        sum_x -= a;
                    if (b > 0.0f)
                        sum_y += b;
                    else
                        sum_y -= b;
                    if (c > 0.0f)
                        sum_z += c;
                    else
                        sum_z -= c;
                    pos_x += a;
                    pos_y += b;
                    vertices.push_back(new float[3]{a, b, c});
                } else if (line[1] == 't') {
                    sscanf(line.c_str(), "vt %f %f", &a, &b);
                    texcoords.push_back(new float[2]{a, b});
                } else {
                    sscanf(line.c_str(), "vn %f %f %f", &a, &b, &c);
                    normals.push_back(new float[3]{a, b, c});
                }
            } else if (line[0] == 'f') {
                int edge = count_char(line, ' ');
                int count_slash = count_char(line, '/');
                if (count_slash == 0) {
                    if (edge == 3)
                        add_face_3v(line);
                    else
                        add_face_4v(line);
                } else if (count_slash == edge) {
                    if (edge == 3)
                        add_face_3vt(line);
                    else
                        add_face_4vt(line);
                } else if (count_slash == edge * 2) {
                    if (has_double_slash(line)) {
                        if (edge == 3)
                            add_face_3vn(line);
                        else
                            add_face_4vn(line);
                    } else {
                        if (edge == 3)
                            add_face_3vtn(line);
                        else
                            add_face_4vtn(line);
                    }
                }
            } else if (line[0] == 'm' && line[1] == 't') {
                sscanf(line.c_str(), "mtllib %s", &str);
                std::string file = prefix + str;
                load_material(file.c_str());
            } else if (line[0] == 'u' && line[1] == 's') {
                sscanf(line.c_str(), "usemtl %s", &str);
                std::string material = str;
                if (map_material.find(material) != map_material.end())
                    faces.push_back(Face(-1, NULL, NULL, map_material[material]));
            }
        }

        bool has_texcoord = false;

        list = glGenLists(1);
        glNewList(list, GL_COMPILE);
        for (Face &face : faces) {
            glColor3ub( rand()%255, rand()%255, rand()%255 );
            if (face.edge == -1) {
                has_texcoord = false;
                glLightfv(GL_LIGHT0, GL_AMBIENT, materials[face.normal].ambient);
                glLightfv(GL_LIGHT0, GL_DIFFUSE, materials[face.normal].diffuse);
                glLightfv(GL_LIGHT0, GL_SPECULAR, materials[face.normal].specular);
                if (materials[face.normal].texture != 0) {
                    has_texcoord = true;
                    glBindTexture(GL_TEXTURE_2D, materials[face.normal].texture);
                }
                continue;
            }
            if (face.normal != -1)
                glNormal3fv(normals[face.normal]);
            else
                glDisable(GL_LIGHTING);
            if (has_texcoord) {
                glBegin(GL_POLYGON);
                for (int i = 0; i < face.edge; i++) {
                    glTexCoord2fv(texcoords[face.texcoords[i]]);
                    glVertex3fv(vertices[face.vertices[i]]);
                }
                glEnd();
            } else {
                glBegin(GL_POLYGON);
                for (int i = 0; i < face.edge; i++)
                    glVertex3fv(vertices[face.vertices[i]]);
                glEnd();
            }
            if (face.normal == -1)
                glEnable(GL_LIGHTING);
        }
        glEndList();

        // printf("Model: %s\n", filename);
        // printf("Vertices: %d\n", vertices.size());
        // printf("Texcoords: %d\n", texcoords.size());
        // printf("Normals: %d\n", normals.size());
        // printf("Faces: %d\n", faces.size());
        // printf("Materials: %d\n", materials.size());

        sum_x /= vertices.size();
        sum_y /= vertices.size();
        sum_z /= vertices.size();
        pos_x /= vertices.size();
        pos_x = -pos_x;
        pos_y /= vertices.size();
        pos_y = -pos_y;
        pos_z = -sqrt(sum_x * sum_x + sum_y * sum_y + sum_z * sum_z) * 15;

        printf("Pos_X: %f\n", pos_x);
        printf("Pos_Y: %f\n", pos_y);
        printf("Pos_Z: %f\n", pos_z);

        for (Material &material : materials) {
            delete material.ambient;
            delete material.diffuse;
            delete material.specular;
        }

        materials.clear();
        map_material.clear();

        for (float *f : vertices)
            delete f;
        vertices.clear();
        for (float *f : texcoords)
            delete f;
        texcoords.clear();
        for (float *f : normals)
            delete f;
        normals.clear();
        faces.clear();
    }

    void draw() { glCallList(list); }
};

#endif

int POS_X, POS_Y;

std::string model_name1 = "./models/snowflake.obj";
std::string model_name2 = "./models/flower.obj";
std::string model_name3 = "./models/monkey.obj";

GLfloat light_pos[] = {-10.0f, 10.0f, 100.00f, 1.0f};

float pos_x, pos_y, pos_z;
float angle_x = 30.0f, angle_y = 0.0f;

int x_old = 0, y_old = 0;
int current_scroll = 5;
float zoom_per_scroll;

bool is_holding_mouse = false;
bool is_updated = false;

Model model1, model2, model3; // model declarations



using namespace std;

constexpr auto PI = 3.14;
GLuint index1 = -1;

float theta = 0,phi=0;
float camX = 0, camY =0, camZ = 10;
float prev_mx =0, curr_mx = 0;
float prev_my = 0, curr_my = 0;

float sphx = 0.6, sphy = 0, sphz = 0;
float pyrx = 1.8, pyry = -0.25, pyrz = 0;
float hexpx = -0.6, hexpy = -0.25, hexpz = 0;

// variables to handle rotation and scaling
float sphscale = 0.55;
float pyrscale = 1;
float hexpscale = 1;

float sphrot = 0;
float pyrrot = 0;
float hexaprot = 0;
// use the same axis as there position

//For rendering Sphere
#define NFACE 20
#define NVERTEX 12
#define X .525731112119133606 
#define Z .850650808352039932

// handling the dynamic background
//float red = 1, green = 1, blue = 1;
float red = 0, green = 0, blue = 0;

// handling robotic rotations
long timer = 0;

// normalize given vector
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


    if (index1 != 2)
    {
        sphx = sphx + mulX * ((double)rand() / (RAND_MAX)) / 50;
        sphy = sphy + mulX * ((double)rand() / (RAND_MAX)) / 50;
        sphz = sphz + mulX * ((double)rand() / (RAND_MAX)) / 50;
        sphrot += ((double)rand() / (RAND_MAX)) * 3;
    }

    if (index1 != 1)
    {
        pyrx = pyrx + mulY * ((double)rand() / (RAND_MAX)) / 50;
        pyry = pyry + mulY * ((double)rand() / (RAND_MAX)) / 50;
        pyrz = pyrz + mulY * ((double)rand() / (RAND_MAX)) / 50;
        pyrrot += ((double)rand() / (RAND_MAX)) * 3;
    }

    if (index1 != 3)
    {
        hexpx = hexpx + mulZ * ((double)rand() / (RAND_MAX)) / 50;
        hexpy = hexpy + mulZ * ((double)rand() / (RAND_MAX)) / 50;
        hexpz = hexpz + mulZ * ((double)rand() / (RAND_MAX)) / 50;
        hexaprot += ((double)rand() / (RAND_MAX)) * 3;
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
    if (index1 != 1 && index1 != 2 && index1 != 3)
    {
        if (timer >= 20)
        {
            // uncomment rotation updation to eliminate total randomness
            //hexaprot += 10;
            //pyrrot += 10;
            //sphrot += 10;
            timer = 1;
        }
    }
    usleep(10000); // 1 second expressed in microseconds -> 1000000

    // update background
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    // red += ((double)rand() / (RAND_MAX))/500;
    // green += ((double)rand() / (RAND_MAX))/500;
    // blue += ((double)rand() / (RAND_MAX))/500;

    if (red > 1) red -= 1;
    if (green >= 1) green -= 1;
    if (blue >= 1) blue -= 1;
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
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20.0, 1.0, 1.0, 2000.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    model1.load(model_name1.c_str());
    model2.load(model_name2.c_str());
    model3.load(model_name3.c_str());

    pos_x = model1.pos_x;
    pos_y = model1.pos_y;
    pos_z = model1.pos_z - 1.0f;

    zoom_per_scroll = -model1.pos_z / 10.0f;

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


void mySpecialFunc(int key, int x, int y)
{
    // triggers only when object is selected and arrow keys are pressed
    // translation, rotation and scaling should happen

    if (index1 == 1)
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
    else if (index1 == 2)
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
    else if(index1==3){
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
    if (index1 == 1)
    {
        // Z for zoom in and X for zoom out
        if (key == 'z')
        {
            pyrscale += 0.1;
        }
        else if (key == 'x')
        {
            pyrscale -= 0.1;    
        }
        else if (key == 'j') // rotation
        {
            pyrrot -= 10;
        }
        else if (key == 'l')
        {
            pyrrot += 10;
        }

    }
    else if(index1 == 2)
    {
        if (key == 'z')
        {
            sphscale += 0.1;
        }
        else if (key == 'x')
        {
            sphscale -= 0.1;
        }
        else if (key == 'j') // rotation
        {
            sphrot -= 10;
        }
        else if (key == 'l')
        {
            sphrot += 10;
        }
    }
    else if(index1==3){
        if (key == 'z')
        {
            hexpscale += 0.1;
        }
        else if (key == 'x')
        {
            hexpscale -= 0.1;
        }
        else if (key == 'j') // rotation
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

        // using stencil buffer part
        float window_width = glutGet(GLUT_WINDOW_WIDTH);
        float window_height = glutGet(GLUT_WINDOW_HEIGHT);
        glReadPixels(x, window_height - y - 1, window_width, window_height, GL_STENCIL_BITS, GL_UNSIGNED_INT, &index1);
        cout << "Index :" << " " << index1 << "\n";
        if (index1 == 0) {
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
    glScalef(2, 2, 2);
    model1.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(sphx, sphy, sphz);
    glScalef(sphscale, sphscale, sphscale);
    glRotatef(sphrot, sphx, sphy, sphz);
    glStencilFunc(GL_ALWAYS, 2, -1);
    glScalef(10, 10, 10);
    model2.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(hexpx, hexpy, hexpz);
    glScalef(hexpscale, hexpscale, hexpscale);
    glRotatef(hexaprot, hexpx, hexpy, hexpz);
    glStencilFunc(GL_ALWAYS, 3, -1);
    model3.draw();
    glPopMatrix();
    
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitWindowSize(700, 700);
    glutInitWindowPosition(10, 10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);

    glutCreateWindow("Assignment");

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