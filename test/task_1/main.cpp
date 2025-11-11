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


// ----------------- Vertex Structs -----------------
struct Vertex {
    glm::vec3 pos;
    glm::vec2 texCoords;
};

// ---------- Geometry Generators ----------
vector<Vertex> generateCircleFan(const glm::vec3& center, float radius, int segments = 36) {
    vector<Vertex> vertices;
    vertices.push_back({ center, glm::vec2(0.5f, 0.5f) });
    float angleStep = glm::two_pi<float>() / segments;
    for (int i = 0; i <= segments; ++i) {
        float theta = i * angleStep;
        glm::vec3 pos = center + glm::vec3(glm::cos(theta) * radius, glm::sin(theta) * radius, 0.0f);
        glm::vec2 texCoords = glm::vec2(glm::cos(theta) * 0.5f + 0.5f, glm::sin(theta) * 0.5f + 0.5f);
        vertices.push_back({ pos, texCoords });
    }
    return vertices;
}

vector<Vertex> generateCylinderSide(const glm::vec3& center, float radius, float height, int segments = 36) {
    vector<Vertex> vertices;
    float angleStep = glm::two_pi<float>() / segments;
    for (int i = 0; i <= segments; ++i) {
        float theta = i * angleStep;
        glm::vec3 bottomPos = center + glm::vec3(glm::cos(theta) * radius, glm::sin(theta) * radius, 0.0f);
        glm::vec3 topPos = bottomPos + glm::vec3(0.0f, 0.0f, height);
        glm::vec2 bottomUV = glm::vec2(static_cast<float>(i) / segments, 0.0f);
        glm::vec2 topUV = glm::vec2(static_cast<float>(i) / segments, 1.0f);
        vertices.push_back({ bottomPos, bottomUV });
        vertices.push_back({ topPos, topUV });
    }
    return vertices;
}


vector<Vertex> generateCylinderSide2(const glm::vec3& center, float radius, float height, int segments = 36) {
    vector<Vertex> vertices;
    float angleStep = glm::two_pi<float>() / segments;
    for (int i = 0; i <= segments; i+=2) {
        float theta = i * angleStep;
        glm::vec3 bottomPos = center + glm::vec3(glm::cos(theta) * radius, glm::sin(theta) * radius, 0.0f);
        glm::vec3 topPos = center + glm::vec3(0.0f, 0.0f, height);
        glm::vec2 bottomUV = glm::vec2(static_cast<float>(i) / segments, 0.0f);
        glm::vec2 topUV = glm::vec2(static_cast<float>(i) / segments, 1.0f);
        vertices.push_back({ bottomPos, bottomUV });
        vertices.push_back({ topPos, topUV });
    }
    return vertices;
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

    vector<Vertex> circleVertices = generateCircleFan(glm::vec3(0.0f, 1.5f, 0.0f), 1.0f, 100);
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(Vertex), circleVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(1);

    vector<Vertex> sideVertices = generateCylinderSide(glm::vec3(0.0f, 1.5f, 0.0f), 1.0f, 2.0f, 36);
    unsigned int VAO2, VBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sideVertices.size() * sizeof(Vertex), sideVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    vector<Vertex> circleVertices2 = generateCircleFan(glm::vec3(0.0f, 1.5f, 2.0f), 1.0f, 100);
    unsigned int VAO3, VBO3;
    glGenVertexArrays(1, &VAO3);
    glGenBuffers(1, &VBO3);
    glBindVertexArray(VAO3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, circleVertices2.size() * sizeof(Vertex), circleVertices2.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(1);



    vector<Vertex> circleVertices3 = generateCircleFan(glm::vec3(0.0f, -1.0f, 0.0f), 1.0f, 100);
    unsigned int VAO4, VBO4;
    glGenVertexArrays(1, &VAO4);
    glGenBuffers(1, &VBO4);
    glBindVertexArray(VAO4);
    glBindBuffer(GL_ARRAY_BUFFER, VBO4);
    glBufferData(GL_ARRAY_BUFFER, circleVertices3.size() * sizeof(Vertex), circleVertices3.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(1);
    


    vector<Vertex> sideVertices2 = generateCylinderSide2(glm::vec3(0.0f, -1.0f, 0.0f), 1.0f, 2.0f, 36);
    unsigned int VAO5, VBO5;
    glGenVertexArrays(1, &VAO5);
    glGenBuffers(1, &VBO5);
    glBindVertexArray(VAO5);
    glBindBuffer(GL_ARRAY_BUFFER, VBO5);
    glBufferData(GL_ARRAY_BUFFER, sideVertices2.size() * sizeof(Vertex), sideVertices2.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


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

        glm::mat4 view = useOrbit ? camera.GetOrbitViewMatrix(orbitTarget, distanceToTarget, yawOrbit, pitchOrbit)
            : camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertices.size());
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.setInt("texture1", 0);

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sideVertices.size());
        glBindVertexArray(0);


        glBindVertexArray(VAO3);
        glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertices2.size());
        glBindVertexArray(0);


        glBindVertexArray(VAO4);
        glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertices3.size());
        glBindVertexArray(0);


        glBindVertexArray(VAO5);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sideVertices2.size());
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteVertexArrays(1, &VAO3);
    glDeleteBuffers(1, &VBO3);

    glfwTerminate();
    return 0;
}
