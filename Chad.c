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

float xrot = 0.0f, yrot = 0.0f; // Rotation angles
float scale = 0.1;  // scaling factor / initial size
float translateX = 0, translateY = -1, translateZ = -10.0; // Translation factors / initial translation
float lastX = 0.0f, lastY = 0.0f;

int action = 0;   // 0 = nothing, 1 = translate, 2 = rotate, 3 = scale

SurFaceMesh *surfmesh = NULL;

float light1Pos[] = {2.0f, 2.0f, 2.0f, 1.0f}; // Light 1 position
float light2Pos[] = {-2.0f, -2.0f, 2.0f, 1.0f}; // Light 2 position

// Compute the normal for a triangle
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

// Read OFF file mesh
SurFaceMesh* loadOFFMesh(const char *filename) {
    int num, n, m;
    int a, b, c, d, e;
    float x, y, z;
    SurFaceMesh *surfmesh;
    char line[256];
    FILE *fin;

    if ((fin = fopen(filename, "r")) == NULL) {
        printf("Error reading mesh file...\n");
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

// Render the mesh
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

// Initialization function
bool init() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1); // Enable second light

    GLfloat lightColor1[] = {1.0f, 1.0f, 1.0f, 1.0f}; // White light
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT0, GL_POSITION, light1Pos);

    GLfloat lightColor2[] = {0.0f, 0.0f, 1.0f, 1.0f}; // Blue light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor2);
    glLightfv(GL_LIGHT1, GL_POSITION, light2Pos);

    glShadeModel(GL_FLAT); // Set flat shading

    surfmesh = loadOFFMesh("sample_polygon.off");
    if (surfmesh == NULL)
        return false;

    return true;
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Apply scene transformations
    glTranslatef(translateX, translateY, translateZ);
    glScalef(scale, scale, scale);
    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);

    drawMesh(surfmesh);

    glFlush();
    glutSwapBuffers();
}

// Resize function
void resize(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, w, h);

    gluPerspective(45.0f, 1.0f * w / h, 1.0f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Mouse interaction
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
    if (mouseDown) {
        float dx = x - lastX;
        float dy = y - lastY;

        // Update light2 position
        light2Pos[0] += dx * 0.01f;
        light2Pos[1] -= dy * 0.01f;

        glLightfv(GL_LIGHT1, GL_POSITION, light2Pos); // Update light position

        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

// Keyboard interaction
void keyboard(unsigned char key, int x, int y) {
    if (key == 't')
        action = 1;
    else if (key == 'r')
        action = 2;
    else if (key == 's')
        action = 3;

    // Light 1 movement
    if (key == 'w') light1Pos[1] += 0.5f;  // Move up
    if (key == 's') light1Pos[1] -= 0.5f;  // Move down
    if (key == 'a') light1Pos[0] -= 0.5f;  // Move left
    if (key == 'd') light1Pos[0] += 0.5f;  // Move right
    glLightfv(GL_LIGHT0, GL_POSITION, light1Pos); // Update light position

    glutPostRedisplay();
}

// Main function
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Static Mesh with Multiple Lights");

    if (!init()) return -1;

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}
