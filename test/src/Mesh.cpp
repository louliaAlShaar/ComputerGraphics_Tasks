#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glad/glad.h>
using namespace std;

// ================== Vertex Struct ==================
struct Vertex {
    glm::vec3 pos;
    glm::vec2 texCoords;
};

// ================== Mesh Class ==================
class Mesh {
public:
    unsigned int VAO, VBO;
    vector<Vertex> vertices;

    Mesh(const vector<Vertex>& verts) {
        vertices = verts;
        setupMesh();
    }

    void Draw(GLenum mode = GL_TRIANGLES) const {
        glBindVertexArray(VAO);
        glDrawArrays(mode, 0, (GLsizei)vertices.size());
        glBindVertexArray(0);
    }

private:
    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

        glBindVertexArray(0);
    }
};

// ================== ShapeGenerator ==================
class ShapeGenerator {
public:

    // (Circle)
    static vector<Vertex> generateCircleFan(const glm::vec3& center, float radius, int segments = 36) {
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

    // (Rectangle)
    static vector<Vertex> generateRectangle(const glm::vec3& center, float width, float height) {
        float hw = width / 2.0f;
        float hh = height / 2.0f;
        return {
            {{center.x - hw, center.y - hh, center.z}, {0.0f, 0.0f}},
            {{center.x + hw, center.y - hh, center.z}, {1.0f, 0.0f}},
            {{center.x + hw, center.y + hh, center.z}, {1.0f, 1.0f}},
            {{center.x - hw, center.y + hh, center.z}, {0.0f, 1.0f}}
        };
    }

    // (Cylinder)
     static vector<Vertex> generateCylinderSide(const glm::vec3& center, float radius, float height, int segments = 36 , bool cone = false) {
        vector<Vertex> vertices;
        float angleStep = glm::two_pi<float>() / segments;
        for (int i = 0; i <= segments; i++) {
            float theta = i * angleStep;
            glm::vec3 bottomPos = center + glm::vec3(glm::cos(theta) * radius, glm::sin(theta) * radius, 0.0f);
            glm::vec3 topPos;
            if (cone)
                topPos = center + glm::vec3(0.0f, 0.0f, height);
            else
                topPos = bottomPos + glm::vec3(0.0f, 0.0f, height);
            glm::vec2 bottomUV = glm::vec2(static_cast<float>(i) / segments, 0.0f);
            glm::vec2 topUV = glm::vec2(static_cast<float>(i) / segments, 1.0f);
            vertices.push_back({ bottomPos, bottomUV });
            vertices.push_back({ topPos, topUV });
        }
        return vertices;
    }

    // (Cube)
    static vector<Vertex> generateCube(const glm::vec3& center, float size) {
        float h = size / 2.0f;
        return {
            // Front face
            {{center.x - h, center.y - h, center.z + h}, {0.0f, 0.0f}},
            {{center.x + h, center.y - h, center.z + h}, {1.0f, 0.0f}},
            {{center.x + h, center.y + h, center.z + h}, {1.0f, 1.0f}},
            {{center.x - h, center.y + h, center.z + h}, {0.0f, 1.0f}},
            // Back face
            {{center.x - h, center.y - h, center.z - h}, {0.0f, 0.0f}},
            {{center.x + h, center.y - h, center.z - h}, {1.0f, 0.0f}},
            {{center.x + h, center.y + h, center.z - h}, {1.0f, 1.0f}},
            {{center.x - h, center.y + h, center.z - h}, {0.0f, 1.0f}},
        };
    }

    // (Pyramid)
    static vector<Vertex> generatePyramid(const glm::vec3& center, float baseSize, float height) {
        float h = baseSize / 2.0f;
        glm::vec3 top = center + glm::vec3(0.0f, 0.0f, height);
        return {
            // Base
            {{center.x - h, center.y - h, center.z}, {0.0f, 0.0f}},
            {{center.x + h, center.y - h, center.z}, {1.0f, 0.0f}},
            {{center.x + h, center.y + h, center.z}, {1.0f, 1.0f}},
            {{center.x - h, center.y + h, center.z}, {0.0f, 1.0f}},
            // Sides
            {{center.x - h, center.y - h, center.z}, {0.0f, 0.0f}}, {top, {0.5f, 1.0f}}, {{center.x + h, center.y - h, center.z}, {1.0f, 0.0f}},
            {{center.x + h, center.y - h, center.z}, {0.0f, 0.0f}}, {top, {0.5f, 1.0f}}, {{center.x + h, center.y + h, center.z}, {1.0f, 0.0f}},
            {{center.x + h, center.y + h, center.z}, {0.0f, 0.0f}}, {top, {0.5f, 1.0f}}, {{center.x - h, center.y + h, center.z}, {1.0f, 0.0f}},
            {{center.x - h, center.y + h, center.z}, {0.0f, 0.0f}}, {top, {0.5f, 1.0f}}, {{center.x - h, center.y - h, center.z}, {1.0f, 0.0f}},
        };
    }
};
