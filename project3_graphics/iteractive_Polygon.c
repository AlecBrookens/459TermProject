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


int mode = GL_LINE; // defalt mode 
bool mouseDown = false;

float xrot = 0.0f, yrot = 0.0f; // Rotation angles
float scale = 0.1;  // scaling factor / initial size 
float translateX = 0, translateY = -1, translateZ = -5.0; // Translation factors / initial translation 
float lastX = 0.0f, lastY = 0.0f;

int action = 0;   // 0 = nothing, 1 = translate, 2 = rotate, 3 = scale

SurFaceMesh *surfmesh = NULL;

// Read OFF file mesh (From proj3_mesh-parser but modified to return surfmesh)
SurFaceMesh* loadOFFMesh(const char *filename) {   
    int num,n,m;
    int a,b,c,d,e;
    float x,y,z;
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
    fscanf(fin,"%d %d %d\n",&m,&n,&num);

    surfmesh = (SurFaceMesh *)malloc(sizeof(SurFaceMesh));
    surfmesh->nv = m;
    surfmesh->nf = n;
    surfmesh->vertex = (FLTVECT *)malloc(sizeof(FLTVECT) * surfmesh->nv);
    surfmesh->face = (INT3VECT *)malloc(sizeof(INT3VECT) * surfmesh->nf);

    for (n = 0; n < surfmesh->nv; n++) {
    fscanf(fin,"%f %f %f\n",&x,&y,&z);
    surfmesh->vertex[n].x = x;
    surfmesh->vertex[n].y = y;
    surfmesh->vertex[n].z = z;
  }
  
  for (n = 0; n < surfmesh->nf; n++) {
    fscanf(fin,"%d %d %d %d\n",&a,&b,&c,&d);
    surfmesh->face[n].a = b;
    surfmesh->face[n].b = c;
    surfmesh->face[n].c = d;
    if(a != 3)
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
        
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
}

// Initialization function (From proj3_rotation_example but modified) 
bool init() {
    // glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0f);

    // Load mesh
    surfmesh = loadOFFMesh("Apple.off"); // Change file name here, if there is another OFF file to load. 
    if (surfmesh == NULL) 
        return false;

    return true;
}

// Display function (From proj3_rotation_example but modified) 
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (mode == 4) { // for both polygon fill and line
        // first render mode 
        glColor3f(1.0f, 0.0f, 0.0f);  // Red color
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        gluLookAt(
        0.0f, 0.0f, 50.0f, // eye point
        0.0f, 0.0f, 0.0f, // reference point
        0.0f, 1.0f, 0.0f); // up vector

        glTranslatef(translateX, translateY, translateZ);
        glScalef(scale, scale, scale);

        glRotatef(xrot, 1.0f, 0.0f, 0.0f);
        glRotatef(yrot, 0.0f, 1.0f, 0.0f);
        

        drawMesh(surfmesh);

        //-------------------------------------------------------

        // Second render mode
        glLoadIdentity();
        glColor3f(1.0f, 1.0f, 0.0f);  // Yellow color
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        gluLookAt(
        0.0f, 0.0f, 50.0f, // eye point
        0.0f, 0.0f, 0.0f, // reference point
        0.0f, 1.0f, 0.0f); // up vector

        glTranslatef(translateX, translateY, translateZ);
        glScalef(scale, scale, scale);

        glRotatef(xrot, 1.0f, 0.0f, 0.0f);
        glRotatef(yrot, 0.0f, 1.0f, 0.0f);
        

        drawMesh(surfmesh);

    } else { // if both is not selected then run/render normally 
        glColor3f(1.0f, 0.0f, 0.0f); // Red color
        glPolygonMode(GL_FRONT_AND_BACK, mode);
        gluLookAt(         // camera initializer?
        0.0f, 0.0f, 50.0f, // eye point
        0.0f, 0.0f, 0.0f, // reference point
        0.0f, 1.0f, 0.0f); // up vector

        glTranslatef(translateX, translateY, translateZ);
        glScalef(scale, scale, scale);

        glRotatef(xrot, 1.0f, 0.0f, 0.0f);
        glRotatef(yrot, 0.0f, 1.0f, 0.0f);
        

        drawMesh(surfmesh);
    }
    glFlush();
    glutSwapBuffers();
}

// Resize function (No changes from proj3_rotation_example)
void resize(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, w, h);

    gluPerspective(45.0f, 1.0f * w / h, 1.0f, 100.0f);
    // glOrtho(-1.0, 1.0, -1.0, 1.0, -10.0, 10.0);

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

// Controls translation, rotation, and scaling
void mouseMotion(int x, int y) {
    if (mouseDown) {
        float dx = x - lastX;
        float dy = y - lastY;

        if (action == 1) {           // Translate
            translateX += dx * 0.01f;
            translateY -= dy * 0.01f;
        } else if (action == 2) {    // Rotate
            xrot += dy;
            yrot += dx;
        } else if (action == 3) {    // Scale (Note: Hold left mouse button and drag down to increase size)
            scale += dy * 0.01f;
            if (scale < 0.1f) scale = 0.1f; // Prevent the scale from going too small
        }

        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

// Select transformation modes (One press will stay on mode until another is selected)
void keyboard(unsigned char key, int x, int y) {
    if (key == 't') 
        action = 1;         // translate
    else if (key == 'r') 
        action = 2;         // rotate
    else if (key == 's') 
        action = 3;         // scale
    glutPostRedisplay();
}

// Display modes for  POINT, LINE, FILL, BOTH or EXIT
void menu(int option) {
    switch (option) {
        case 1: 
            mode = GL_POINT; // Draw the polygon in point mode
            break;  
        case 2: 
            mode = GL_LINE; // Draw the polygon in line mode
            break;   
        case 3: 
            mode = GL_FILL; // Draw the polygon in fill mode
            break; 
        case 4:
            mode = 4;
            break;
        case 5: 
            free(surfmesh->vertex);     // free allocated memory of mesh mallocs
            free(surfmesh->face);
            free(surfmesh);
            exit(0);                   // Exit the program
    }
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    // Main window
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("3D Interactive Mesh Viewer");

    if (!init()) return 1; // check to see if file was read successfully or not

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

    // Submenu to switch between display modes
    glutCreateMenu(menu);
    glutAddMenuEntry("POINT", 1);
    glutAddMenuEntry("LINE", 2);
    glutAddMenuEntry("FILL", 3);
    glutAddMenuEntry("BOTH", 4);
    glutAddMenuEntry("EXIT", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop();
    return 0;
}
