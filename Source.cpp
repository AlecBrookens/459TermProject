#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>


float translateX = 0.0f, translateY = 0.0f, translateZ = 0.0f;
float rotateX = 0.0f, rotateY = 0.0f, rotateZ = 0.0f;
float scale = 1.0f;

enum DrawMode { POINT, LINE, FILL, BOTH };
DrawMode drawMode = FILL;

int lastMouseX = 0, lastMouseY = 0;
enum TransformMode { NONE, TRANSLATE, ROTATE, SCALE };
TransformMode currentMode = NONE;

struct Vertex {
    float x, y, z;
};

struct Face {
    int v1, v2, v3; 
};

// hello

std::vector<Vertex> vertices;
std::vector<Face> faces;

void loadPolygon(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line);
    if (line != "OFF") {
        std::cerr << "Error: Not a valid OFF file" << std::endl;
        return;
    }

    int numVertices, numFaces, numEdges;
    file >> numVertices >> numFaces >> numEdges;

    vertices.reserve(numVertices);
    for (int i = 0; i < numVertices; ++i) {
        Vertex vertex;
        file >> vertex.x >> vertex.y >> vertex.z;
        vertices.push_back(vertex);
    }

    faces.reserve(numFaces);
    for (int i = 0; i < numFaces; ++i) {
        int numVerticesInFace;
        Face face;
        file >> numVerticesInFace >> face.v1 >> face.v2 >> face.v3;
        faces.push_back(face);
    }

    file.close();
}

void renderPolygon() {
    glBegin(GL_TRIANGLES);
    for (const auto& face : faces) {
        const Vertex& v1 = vertices[face.v1];
        const Vertex& v2 = vertices[face.v2];
        const Vertex& v3 = vertices[face.v3];

        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(translateX, translateY, translateZ);
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotateZ, 0.0f, 0.0f, 1.0f);
    glScalef(scale, scale, scale);

    switch (drawMode) {
    case POINT:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glColor3f(0.0f, 1.0f, 0.0f);
        break;
    case LINE:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(0.0f, 0.0f, 1.0f);
        break;
    case FILL:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor3f(0.0f, 1.0f, 1.0f);
        break;
    case BOTH:
        glColor3f(1.0f, 1.0f, 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        renderPolygon();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  
        glColor3f(0.0f, 0.0f, 1.0f);  
        renderPolygon();  
        break;
    }

    renderPolygon();  

    glutSwapBuffers();
}


void menu(int option) {
    switch (option) {
    case 1: drawMode = POINT; break;
    case 2: drawMode = LINE; break;
    case 3: drawMode = FILL; break;
    case 4: drawMode = BOTH; break;
    case 5: exit(0); break;
    }
    glutPostRedisplay();
}

void createMenu() {
    glutCreateMenu(menu);
    glutAddMenuEntry("POINT", 1);
    glutAddMenuEntry("LINE", 2);
    glutAddMenuEntry("FILL", 3);
    glutAddMenuEntry("BOTH", 4);
    glutAddMenuEntry("EXIT", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 't': currentMode = TRANSLATE; break;
    case 'r': currentMode = ROTATE; break;
    case 's': currentMode = SCALE; break;
    }
    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    int dx = x - lastMouseX;
    int dy = y - lastMouseY;

    switch (currentMode) {
    case TRANSLATE:
        translateX += dx * 0.002f; 
        translateY -= dy * 0.002f;
        break;

    case ROTATE:
        rotateY += dx * 0.5f; 
        rotateX += dy * 0.5f;
        break;

    case SCALE:
        scale += dy * 0.001f;
        if (scale < 0.01f) scale = 0.01f;
        break;

    case NONE:
        break;
    }

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        lastMouseX = x;
        lastMouseY = y;
    }
}


void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    loadPolygon("sample_polygon.off");
    scale = .1;
 

    createMenu();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Polygon Viewer");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(mouseMotion);

    glutMainLoop();
    return 0;
}
