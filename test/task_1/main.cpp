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
        camera.ProcessMouseScroll((float)yoffset);
    }
}

// ----------------- Main -----------------
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello", nullptr, nullptr);
    if (!window) { cout << "Failed to create window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { cout << "Failed to initialize GLAD\n"; return -1; }

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);

    Shader shader("Shaders/vertex.glsl", "Shaders/fragment.glsl");

    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/paper.png", &width, &height, &nrChannels, 0);
    if (!data) {
        cout << "Failed to load texture 'assets/paper.png'!\n";
    }


    unsigned int diffuseTexID = 0;
    if (data) {
        glGenTextures(1, &diffuseTexID);
        glBindTexture(GL_TEXTURE_2D, diffuseTexID);
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }

    unsigned int specularTexID;
    glGenTextures(1, &specularTexID);
    glBindTexture(GL_TEXTURE_2D, specularTexID);
    unsigned char whitePixel[3] = { 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    CylinderMeshes myCylinder = ShapeGenerator::generateCylinder(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 2.0f);

    glm::vec4 pyramidColor = glm::vec4(1.0f, 0.5f, 0.31f, 1.0f);
    Mesh coloredPyramid = ShapeGenerator::generatePyramid(
        glm::vec3(3.0f, 0.0f, 0.0f),
        2.0f,
        2.0f,
        true,
        pyramidColor
    );

    glm::vec3 lightPos(0.0f, 2.0f, 4.0f);

    vector<Vertex> lightRectVerts = ShapeGenerator::generateRectangle(lightPos, 0.5f, 0.5f, true, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)); 
    Mesh lightRect(lightRectVerts, true, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // ---------- Callbacks ----------
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (diffuseTexID != 0) {
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, diffuseTexID);
    }

    glActiveTexture(GL_TEXTURE1); 
    glBindTexture(GL_TEXTURE_2D, specularTexID);



    shader.use();
    

    // ---------- Main loop ----------
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

        shader.setVec3("viewPos", camera.Position);

        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        shader.setFloat("ambientStrength", 1.3f);
        shader.setFloat("specularStrength", 0.5f);
        shader.setFloat("shininess", 32.0f);

        shader.setVec3("materialAmbient", glm::vec3(0.7f, 0.7f, 0.7f));
        shader.setVec3("materialDiffuse", glm::vec3(0.7f, 0.7f, 0.7f));
        shader.setVec3("materialSpecular", glm::vec3(1.0f, 1.0f, 1.0f));

        shader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        shader.setVec3("dirLight.ambient", glm::vec3(0.1f));
        shader.setVec3("dirLight.diffuse", glm::vec3(1.0f));
        shader.setVec3("dirLight.specular", glm::vec3(1.0f));

        shader.setInt("diffuseMap", 0);
        shader.setInt("specularMap", 1);


        // ============ Enable ONE Point Light ============
        shader.setVec3("pointLights[0].position", lightPos);
        shader.setVec3("pointLights[0].ambient", glm::vec3(0.1f));
        shader.setVec3("pointLights[0].diffuse", glm::vec3(1.0f));
        shader.setVec3("pointLights[0].specular", glm::vec3(1.0f));

        shader.setFloat("pointLights[0].constant", 1.0f);
        shader.setFloat("pointLights[0].linear", 0.09f);
        shader.setFloat("pointLights[0].quadratic", 0.032f);
        
        // ============ Enable Spot Light ============

        shader.setVec3("spotLight.position", camera.Position);
        shader.setVec3("spotLight.direction", camera.Front);
        shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

        shader.setFloat("spotLight.constant", 1.0f);
        shader.setFloat("spotLight.linear", 0.09f);
        shader.setFloat("spotLight.quadratic", 0.032f);

        shader.setVec3("spotLight.ambient", glm::vec3(0.0f));
        shader.setVec3("spotLight.diffuse", glm::vec3(1.0f));
        shader.setVec3("spotLight.specular", glm::vec3(1.0f));




        // ---- matrices ----
        glm::mat4 view = useOrbit ? camera.GetOrbitViewMatrix(orbitTarget, distanceToTarget, yawOrbit, pitchOrbit)
            : camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // ----------------- Draw Cylinder with Texture -----------------
        shader.setBool("useVertexColor", false);
        shader.setBool("useTexture", true);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexID);
        ShapeGenerator::drawCylinder(myCylinder);

        // ----------------- Draw Pyramid with Color -----------------
        shader.setBool("useTexture", false);
        shader.setBool("useVertexColor", true);
        shader.setVec4("objectColor", pyramidColor);
        coloredPyramid.Draw(GL_TRIANGLES);

        // ----------------- Draw Light Rect -----------------
        shader.setBool("useVertexColor", true);
        shader.setBool("useTexture", false);
        shader.setVec4("objectColor", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        lightRect.Draw(GL_TRIANGLE_FAN);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
