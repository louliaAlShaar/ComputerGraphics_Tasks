#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "lodepng.h"
#include "Shader.cpp"
#include "../lib/stb_image.h"
#include "Camera.cpp"
#include <Mesh.cpp>
using namespace std;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


// ----------------- Variables -----------------

float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;


glm::vec3 orbitTarget(0.0f, 0.0f, 0.0f);
float distanceToTarget = 5.0f;

float yawOrbit = 0.0f;
float pitchOrbit = 0.0f;

float lastMouseX = 400.0f;
float lastMouseY = 300.0f;
bool firstMouseOrbit = true;
bool mouseHeldOrbit = false;
float mouseSensitivityOrbit = 0.3f;

// ----------------- Mode Switch -----------------
bool useOrbit = true; // false = FPS, true = Orbit
bool firstMouseFPS = true;

// ----------------- Callbacks -----------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS) mouseHeldOrbit = true;
        else if (action == GLFW_RELEASE) mouseHeldOrbit = false;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (useOrbit) {
        if (!mouseHeldOrbit) {
            firstMouseOrbit = true;
            return;
        }

        if (firstMouseOrbit) {
            lastMouseX = (float)xpos;
            lastMouseY = (float)ypos;
            firstMouseOrbit = false;
        }

        float xoffset = (float)xpos - lastMouseX;
        float yoffset = lastMouseY - (float)ypos;

        lastMouseX = (float)xpos;
        lastMouseY = (float)ypos;

        yawOrbit += xoffset * mouseSensitivityOrbit;
        pitchOrbit += yoffset * mouseSensitivityOrbit;

        if (pitchOrbit > 89.0f) pitchOrbit = 89.0f;
        if (pitchOrbit < -89.0f) pitchOrbit = -89.0f;
    }
    else {
        if (firstMouseFPS) {
            lastX = (float)xpos;
            lastY = (float)ypos;
            firstMouseFPS = false;
        }

        float xoffset = (float)xpos - lastX;
        float yoffset = lastY - (float)ypos;

        lastX = (float)xpos;
        lastY = (float)ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (useOrbit) {
        distanceToTarget -= (float)yoffset;
        if (distanceToTarget < 1.0f) distanceToTarget = 1.0f;
        if (distanceToTarget > 20.0f) distanceToTarget = 20.0f;
    }
    else {
        camera.ProcessMouseScroll((float)yoffset); // FPS zoom
    }
}

// ----------------- Main -----------------
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Camera Orbit / FPS", nullptr, nullptr);
    if (!window) { cout << "Failed to create window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { cout << "Failed to initialize GLAD\n"; return -1; }

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);

    Shader shader("Shaders/vertex.glsl", "Shaders/fragment.glsl");

    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/paper.png", &width, &height, &nrChannels, 0);
    if (!data) cout << "Failed to load texture!\n";

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);



    // ==== ÅäÔÇÁ Meshes ====
    CylinderMeshes myCylinder = ShapeGenerator::generateCylinder(glm::vec3(0.0f, 1.5f, 0.0f), 1.0f, 2.0f);
    ConeMeshes myCone = ShapeGenerator::generateCone(glm::vec3(0.0f, -1.0f, 0.0f), 1.0f, 2.0f);
    Mesh pyramid = ShapeGenerator::generatePyramid(
        glm::vec3(-2.5f, -1.0f, 0.0f),   // center
        2.0f,                          // base size
        2.0f                           // height
    );


    // center = ãæÞÚ ÇáãæÔæÑ Ýí ÇáÚÇáã
    glm::vec3 center = glm::vec3(-2.5f, 1.5f, 1.0f);
    float radius = 1.0f;   // äÕÝ ÞØÑ ÇáÞÇÚÏÉ

    // ãËÇá ãæÔæÑ ÑÈÇÚí
    Mesh prism = ShapeGenerator::generatePrism(center, radius, 2.0f, 3);




    

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // FPS mode cursor

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            useOrbit = !useOrbit;
            firstMouseOrbit = true;
            firstMouseFPS = true;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
  
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        shader.setInt("texture1", 0);

 
        glm::mat4 view = useOrbit ? camera.GetOrbitViewMatrix(orbitTarget, distanceToTarget, yawOrbit, pitchOrbit)
            : camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));


        // ÑÓã ÇáÇÓØæÇäÉ æÇáãÎÑæØ
        ShapeGenerator::drawCylinder(myCylinder);
        ShapeGenerator::drawCone(myCone);
        pyramid.Draw(GL_TRIANGLES);
        prism.Draw(GL_TRIANGLES);


        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
