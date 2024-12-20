#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <math.h>

// Structures for mesh
typedef struct {
    float x;
    float y;
    float z;
} FLTVECT;

typedef struct {
    int a;
    int b;
    int c;
} INT3VECT;

typedef struct {
    int nv;
    int nf;
    FLTVECT *vertex;
    INT3VECT *face;
} SurFaceMesh;

int mode = GL_LINE; // default mode
bool mouseDown = false;

float lastX = 0.0f; 
float lastY = 0.0f;

SurFaceMesh *surfmesh = NULL;

float light0Pos[] = {2.0f, 2.0f, 2.0f, 1.0f}; // Light 0 Starting position
float light1Pos[] = {-2.0f, -2.0f, 2.0f, 1.0f}; // Light 1 Starting position


GLfloat light0Colors[][4] = {
    {0.1f, 0.1f, 0.1f, 1.0f},  // White
    {0.1f, 0.0f, 0.0f, 1.0f},  // Red
    {0.0f, 0.1f, 0.0f, 1.0f},  // Green
    {0.0f, 0.0f, 0.1f, 1.0f},  // Blue
};
GLfloat light1Colors[][3] = {
    {0.1f, 0.1f, 0.0f, 1.0f},  // Yellow
    {0.0f, 0.1f, 0.1f, 1.0f},  // Cyan
    {0.1f, 0.0f, 0.1f, 1.0f},  // Magenta
};

int light0ColorIndex = 0;
int light1ColorIndex = 0; 

// Compute the normal vectoors
FLTVECT computeNormal(FLTVECT v1, FLTVECT v2, FLTVECT v3) {
    FLTVECT normal;
    FLTVECT u, v;

    // Calculate vectors
    u.x = v2.x - v1.x;
    u.y = v2.y - v1.y;
    u.z = v2.z - v1.z;

    v.x = v3.x - v1.x;
    v.y = v3.y - v1.y;
    v.z = v3.z - v1.z;

    // Compute cross product
    // gives a vector perpendicular to the plane
    normal.x = u.y * v.z - u.z * v.y;
    normal.y = u.z * v.x - u.x * v.z;
    normal.z = u.x * v.y - u.y * v.x;

    // Normalize the vector
    float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    if (length != 0.0f) {
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;
    }

    return normal;
}

SurFaceMesh* loadOFFMesh(const char *filename) {
    int num, n, m;
    int a, b, c, d, e;
    float x, y, z;
    SurFaceMesh *surfmesh;
    char line[256];
    FILE *fin;

    if ((fin = fopen(filename, "r")) == NULL) {
        surfmesh = NULL;
        exit(0);
    }

    while (fgets(line, 256, fin) != NULL) {
        if (line[0] == 'O' && line[1] == 'F' && line[2] == 'F')
            break;
    }
    fscanf(fin, "%d %d %d\n", &m, &n, &num);

    surfmesh = (SurFaceMesh *)malloc(sizeof(SurFaceMesh));
    surfmesh->nv = m;
    surfmesh->nf = n;
    surfmesh->vertex = (FLTVECT *)malloc(sizeof(FLTVECT) * surfmesh->nv);
    surfmesh->face = (INT3VECT *)malloc(sizeof(INT3VECT) * surfmesh->nf);

    for (n = 0; n < surfmesh->nv; n++) {
        fscanf(fin, "%f %f %f\n", &x, &y, &z);
        surfmesh->vertex[n].x = x;
        surfmesh->vertex[n].y = y;
        surfmesh->vertex[n].z = z;
    }

    for (n = 0; n < surfmesh->nf; n++) {
        fscanf(fin, "%d %d %d %d\n", &a, &b, &c, &d);
        surfmesh->face[n].a = b;
        surfmesh->face[n].b = c;
        surfmesh->face[n].c = d;
        if (a != 3)
            printf("Errors: reading mesh .... \n");
    }

    fclose(fin);
    return surfmesh;
}

void drawMesh(SurFaceMesh *mesh) {
    if (mesh == NULL) return;

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < mesh->nf; i++) {
        FLTVECT v1 = mesh->vertex[mesh->face[i].a];
        FLTVECT v2 = mesh->vertex[mesh->face[i].b];
        FLTVECT v3 = mesh->vertex[mesh->face[i].c];

        // Compute and set normal
        FLTVECT normal = computeNormal(v1, v2, v3);
        glNormal3f(normal.x, normal.y, normal.z);

        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
}

bool init() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1); 

    GLfloat lightColor0[] = {0.1f, 0.1f, 0.1f, 1.0f}; 
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);

    GLfloat lightColor1[] = {0.1f, 0.1f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);

    glShadeModel(GL_FLAT); 

    surfmesh = loadOFFMesh("sample_polygon.off");
    if (surfmesh == NULL)
        return false;

    return true;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Apply scene transformations
    glTranslatef(0.0, -1.0, -10.0);
    glScalef(0.1, 0.1, 0.1);
    glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f);

    drawMesh(surfmesh);

    glFlush();
    glutSwapBuffers();
}

void resize(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, w, h);

    gluPerspective(45.0f, 1.0f * w / h, 1.0f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseDown = true;
        lastX = x;
        lastY = y;
    } else {
        mouseDown = false;
    }
}

void mouseMotion(int x, int y) {
    //light 1 movement
    if (mouseDown) {
        float dx = x - lastX;
        float dy = y - lastY;

        light1Pos[0] += dx * 0.1f;
        light1Pos[1] -= dy * 0.1f;

        glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);

        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    // Light 0 movement
    if (key == 'w') light0Pos[1] += 10.0f;  // Move up
    if (key == 's') light0Pos[1] -= 10.0f;  // Move down
    if (key == 'a') light0Pos[0] -= 10.0f;  // Move left
    if (key == 'd') light0Pos[0] += 10.0f;  // Move right
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos); 
    
    // Light 0 color change
    if (key == '0') {
        light0ColorIndex = (light0ColorIndex + 1) % 4; // Cycle through colors
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Colors[light0ColorIndex]);
    }

    // Light 1 color change
    if (key == '1') {
        light1ColorIndex = (light1ColorIndex + 1) % 3; // Cycle through colors
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Colors[light1ColorIndex]);
    }

    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Lighting Term Project Demo");

    if (!init()) return -1;

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}
