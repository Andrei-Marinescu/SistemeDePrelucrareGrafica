#include <freeglut.h>
#include <iostream>
#include <cmath>

#include "stb_image.h"


GLuint grassTexture;
GLuint skyTexture;    
GLuint mountainTexture; 
const float BOX_SIZE = 250.0f;

float cameraPosX = 0.0f, cameraPosY = 15.0f, cameraPosZ = 25.0f;
float cameraFrontX = 0.0f, cameraFrontY = -0.2f, cameraFrontZ = -1.0f;
float cameraUpX = 0.0f, cameraUpY = 1.0f, cameraUpZ = 0.0f;
float yaw = -90.0f;
float pitch = -10.0f;

GLuint loadTexture(const char* filename) {
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        std::cout << "Texture " << filename << " loaded successfully!\n";
    }
    else {
        std::cout << "Error! Could not load texture: " << filename << "\n";
        unsigned char errorTex[] = { 255, 0, 0 };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, errorTex);
    }

    stbi_image_free(data);
    return texID;
}

void drawSceneCube() {
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    // 1. Grass
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-BOX_SIZE, 0.0f, -BOX_SIZE);
    glTexCoord2f(15.0f, 0.0f);  glVertex3f(BOX_SIZE, 0.0f, -BOX_SIZE);
    glTexCoord2f(15.0f, 15.0f); glVertex3f(BOX_SIZE, 0.0f, BOX_SIZE);
    glTexCoord2f(0.0f, 15.0f);  glVertex3f(-BOX_SIZE, 0.0f, BOX_SIZE);
    glEnd();

    // 2. Skybox Walls
    glBindTexture(GL_TEXTURE_2D, skyTexture);
    glBegin(GL_QUADS);
    // Back
    glTexCoord2f(0.25f, 0.333f); glVertex3f(-BOX_SIZE, 0.0f, -BOX_SIZE);
    glTexCoord2f(0.50f, 0.333f); glVertex3f(BOX_SIZE, 0.0f, -BOX_SIZE);
    glTexCoord2f(0.50f, 0.666f); glVertex3f(BOX_SIZE, BOX_SIZE, -BOX_SIZE);
    glTexCoord2f(0.25f, 0.666f); glVertex3f(-BOX_SIZE, BOX_SIZE, -BOX_SIZE);
    // Right
    glTexCoord2f(0.50f, 0.333f); glVertex3f(BOX_SIZE, 0.0f, -BOX_SIZE);
    glTexCoord2f(0.75f, 0.333f); glVertex3f(BOX_SIZE, 0.0f, BOX_SIZE);
    glTexCoord2f(0.75f, 0.666f); glVertex3f(BOX_SIZE, BOX_SIZE, BOX_SIZE);
    glTexCoord2f(0.50f, 0.666f); glVertex3f(BOX_SIZE, BOX_SIZE, -BOX_SIZE);
    // Front
    glTexCoord2f(0.75f, 0.333f); glVertex3f(BOX_SIZE, 0.0f, BOX_SIZE);
    glTexCoord2f(1.00f, 0.333f); glVertex3f(-BOX_SIZE, 0.0f, BOX_SIZE);
    glTexCoord2f(1.00f, 0.666f); glVertex3f(-BOX_SIZE, BOX_SIZE, BOX_SIZE);
    glTexCoord2f(0.75f, 0.666f); glVertex3f(BOX_SIZE, BOX_SIZE, BOX_SIZE);
    // Left
    glTexCoord2f(0.00f, 0.333f); glVertex3f(-BOX_SIZE, 0.0f, BOX_SIZE);
    glTexCoord2f(0.25f, 0.333f); glVertex3f(-BOX_SIZE, 0.0f, -BOX_SIZE);
    glTexCoord2f(0.25f, 0.666f); glVertex3f(-BOX_SIZE, BOX_SIZE, -BOX_SIZE);
    glTexCoord2f(0.00f, 0.666f); glVertex3f(-BOX_SIZE, BOX_SIZE, BOX_SIZE);
    // Top
    glTexCoord2f(0.25f, 0.666f); glVertex3f(-BOX_SIZE, BOX_SIZE, -BOX_SIZE);
    glTexCoord2f(0.25f, 1.000f); glVertex3f(-BOX_SIZE, BOX_SIZE, BOX_SIZE);
    glTexCoord2f(0.50f, 1.000f); glVertex3f(BOX_SIZE, BOX_SIZE, BOX_SIZE);
    glTexCoord2f(0.50f, 0.666f); glVertex3f(BOX_SIZE, BOX_SIZE, -BOX_SIZE);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void drawTerrain() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, grassTexture);

    glColor3f(0.85f, 0.85f, 0.85f);

    const float STEP = 4.0f;           
    const float MAX_SIZE = BOX_SIZE;

    const float BIG_HEIGHT = 12.0f;     
    const float SMALL_HEIGHT = 3.0f;    
    const float BIG_FREQ = 0.03f;     
    const float SMALL_FREQ = 0.15f;  

    for (float z = -MAX_SIZE; z < MAX_SIZE; z += STEP) {
        glBegin(GL_TRIANGLE_STRIP);

        for (float x = -MAX_SIZE; x <= MAX_SIZE; x += STEP) {

            float y1 =
                BIG_HEIGHT * sin(x * BIG_FREQ) +
                BIG_HEIGHT * cos(z * BIG_FREQ) +
                SMALL_HEIGHT * sin(x * SMALL_FREQ) * cos(z * SMALL_FREQ);

            glTexCoord2f((x + MAX_SIZE) * 0.08f, (z + MAX_SIZE) * 0.08f);
            glVertex3f(x, y1, z);

          
            float y2 =
                BIG_HEIGHT * sin(x * BIG_FREQ) +
                BIG_HEIGHT * cos((z + STEP) * BIG_FREQ) +
                SMALL_HEIGHT * sin(x * SMALL_FREQ) * cos((z + STEP) * SMALL_FREQ);

            glTexCoord2f((x + MAX_SIZE) * 0.08f, (z + STEP + MAX_SIZE) * 0.08f);
            glVertex3f(x, y2, z + STEP);
        }

        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}


void drawMountain() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mountainTexture);
    glColor3f(0.9f, 0.9f, 0.9f);

    float baseX = 0.0f;
    float baseZ = -40.0f;

    float height = 80.0f;      
    float baseWidth = 100.0f; 
    float halfWidth = baseWidth / 2.0f;

    float peakX = baseX;
    float peakY = height;
    float peakZ = baseZ;

    float v1x = baseX - halfWidth; float v1z = baseZ + halfWidth;
    float v2x = baseX + halfWidth; float v2z = baseZ + halfWidth;
    float v3x = baseX + halfWidth; float v3z = baseZ - halfWidth;
    float v4x = baseX - halfWidth; float v4z = baseZ - halfWidth;

    glBegin(GL_TRIANGLES);

    // Face 1
    glTexCoord2f(1.0f, 2.0f); glVertex3f(peakX, peakY, peakZ);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(v1x, 0.0f, v1z);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(v2x, 0.0f, v2z);

    // Face 2
    glTexCoord2f(1.0f, 2.0f); glVertex3f(peakX, peakY, peakZ);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(v2x, 0.0f, v2z);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(v3x, 0.0f, v3z);

    // Face 3
    glTexCoord2f(1.0f, 2.0f); glVertex3f(peakX, peakY, peakZ);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(v3x, 0.0f, v3z);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(v4x, 0.0f, v4z);

    // Face 4
    glTexCoord2f(1.0f, 2.0f); glVertex3f(peakX, peakY, peakZ);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(v4x, 0.0f, v4z);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(v1x, 0.0f, v1z);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void handleMouseMove(int xpos, int ypos) {
    int centerX = 400;
    int centerY = 300;
    if (xpos == centerX && ypos == centerY) return;

    float xoffset = (float)(xpos - centerX);
    float yoffset = (float)(centerY - ypos);
    float sensitivity = 0.1f;

    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    float frontX = cos(yaw * 3.14159f / 180.0f) * cos(pitch * 3.14159f / 180.0f);
    float frontY = sin(pitch * 3.14159f / 180.0f);
    float frontZ = sin(yaw * 3.14159f / 180.0f) * cos(pitch * 3.14159f / 180.0f);

    float length = sqrt(frontX * frontX + frontY * frontY + frontZ * frontZ);
    cameraFrontX = frontX / length;
    cameraFrontY = frontY / length;
    cameraFrontZ = frontZ / length;

    glutWarpPointer(centerX, centerY);
    glutPostRedisplay();
}

void handleKeyboard(unsigned char key, int x, int y) {
    float cameraSpeed = 0.5f;
    if (key == 'w' || key == 'W') {
        cameraPosX += cameraSpeed * cameraFrontX;
        cameraPosY += cameraSpeed * cameraFrontY;
        cameraPosZ += cameraSpeed * cameraFrontZ;
    }
    if (key == 's' || key == 'S') {
        cameraPosX -= cameraSpeed * cameraFrontX;
        cameraPosY -= cameraSpeed * cameraFrontY;
        cameraPosZ -= cameraSpeed * cameraFrontZ;
    }
    if (key == 'a' || key == 'A') {
        float rightX = cameraFrontY * cameraUpZ - cameraFrontZ * cameraUpY;
        float rightY = cameraFrontZ * cameraUpX - cameraFrontX * cameraUpZ;
        float rightZ = cameraFrontX * cameraUpY - cameraFrontY * cameraUpX;
        float length = sqrt(rightX * rightX + rightY * rightY + rightZ * rightZ);
        cameraPosX -= (rightX / length) * cameraSpeed;
        cameraPosY -= (rightY / length) * cameraSpeed;
        cameraPosZ -= (rightZ / length) * cameraSpeed;
    }
    if (key == 'd' || key == 'D') {
        float rightX = cameraFrontY * cameraUpZ - cameraFrontZ * cameraUpY;
        float rightY = cameraFrontZ * cameraUpX - cameraFrontX * cameraUpZ;
        float rightZ = cameraFrontX * cameraUpY - cameraFrontY * cameraUpX;
        float length = sqrt(rightX * rightX + rightY * rightY + rightZ * rightZ);
        cameraPosX += (rightX / length) * cameraSpeed;
        cameraPosY += (rightY / length) * cameraSpeed;
        cameraPosZ += (rightZ / length) * cameraSpeed;
    }
    if (key == 27) exit(0); 
    glutPostRedisplay();
}


void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(cameraPosX, cameraPosY, cameraPosZ,
        cameraPosX + cameraFrontX, cameraPosY + cameraFrontY, cameraPosZ + cameraFrontZ,
        cameraUpX, cameraUpY, cameraUpZ);

    drawSceneCube();  
    drawTerrain();    
    drawMountain();   

    glutSwapBuffers();
}


void init() {
    glClearColor(0.6f, 0.7f, 0.8f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    grassTexture = loadTexture("grass1.bmp");
    mountainTexture = loadTexture("grass1.bmp");
    skyTexture = loadTexture("skybox.jpg");
}

void handleResize(int w, int h) {
    if (h == 0) h = 1;
    float ratio = w * 1.0 / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(60.0f, ratio, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Game");

    init();

    glutDisplayFunc(renderScene);
    glutReshapeFunc(handleResize);

    glutSetCursor(GLUT_CURSOR_NONE);
    glutPassiveMotionFunc(handleMouseMove);
    glutKeyboardFunc(handleKeyboard);

    glutMainLoop();
    return 0;
}