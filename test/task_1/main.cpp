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
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Changing Color Triangle", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);

    Shader shader("Shaders/vertex.glsl", "Shaders/fragment.glsl");

    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/wall.jpg", &width, &height, &nrChannels, 0);
    if (!data)
        std::cout << "Failed to load texture!\n";

    // Texture 1
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



    /*
    * test
    float vertices[] = {
        // positions         // texture coords
         0.0f,  0.0f, 0.0f,   1.0f, 1.0f,   // v0 - center (ãÑßÒ ÇáãÑæÍÉ)
         0.5f,  0.0f, 0.0f,   2.0f, 1.0f,   // v1 - íãíä
         0.35f, 0.35f, 0.0f,  1.8f, 1.8f,   // v2 - ÝæÞ íãíä
         0.0f,  0.5f, 0.0f,   1.0f, 2.0f,   // v3 - ÝæÞ
        -0.35f, 0.35f, 0.0f,  0.2f, 1.8f,   // v4 - ÝæÞ íÓÇÑ
        -0.5f,  0.0f, 0.0f,   0.0f, 1.0f,   // v5 - íÓÇÑ
        -0.35f, -0.35f, 0.0f, 0.2f, 0.2f,   // v6 - ÊÍÊ íÓÇÑ
         0.35f, -0.35f, 0.0f, 1.8f, 0.2f,   // v7 - ÊÍÊ íãíä
         0.5f,  0.0f, 0.0f,   2.0f, 1.0f    // v8 - äÑÌÚ ááäÞØÉ ÇáÃæáì áäÛáÞ ÇáãÑæÍÉ
    };*/

    float vertices[] = {
        // positions        // texture coords
         0.0f,  0.5f, 0.0f,  1.0f, 2.0f,  // top 
         0.5f, -0.5f, 0.0f,  2.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // bottom left
    };


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);




    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    while (!glfwWindowShouldClose(window))
    {


        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);



        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
        model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //(float)glfwGetTime()
        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

        unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.setInt("texture1", 0);

        glBindVertexArray(VAO);
        //glPointSize(10.0f);
        glDrawArrays(GL_LINE_LOOP, 0, 3); //GL_TRIANGLES  GL_LINES  GL_POINTS  GL_LINE_STRIP  GL_LINE_LOOP  GL_TRIANGLE_STRIP  GL_TRIANGLE_FAN
        //glDrawArrays(GL_TRIANGLE_FAN, 0, 9);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
