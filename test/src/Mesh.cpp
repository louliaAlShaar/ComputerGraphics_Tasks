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



struct CylinderMeshes {
    Mesh topCircle;
    Mesh side;
    Mesh bottomCircle;

    CylinderMeshes(const Mesh& top, const Mesh& s, const Mesh& bottom)
        : topCircle(top), side(s), bottomCircle(bottom) {
    }
};



struct ConeMeshes {
    Mesh bottomCircle;
    Mesh side;

    ConeMeshes(const Mesh& bottom, const Mesh& s)
        : bottomCircle(bottom), side(s) {
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



     static CylinderMeshes generateCylinder(const glm::vec3& center, float radius, float height) {
         return CylinderMeshes(
             Mesh(generateCircleFan(center + glm::vec3(0, 0, height), radius, 100)),
             Mesh(generateCylinderSide(center, radius, height, 100, false)),
             Mesh(generateCircleFan(center, radius, 36))
         );
     }

     // (Cone)
     static ConeMeshes generateCone(const glm::vec3& center, float radius, float height) {
         return ConeMeshes(
             Mesh(generateCircleFan(center, radius, 100)),
             Mesh(generateCylinderSide(center, radius, height, 100, true))
         );
     }



     static void drawCylinder(const CylinderMeshes& cyl) {
         cyl.topCircle.Draw(GL_TRIANGLE_FAN);
         cyl.side.Draw(GL_TRIANGLE_STRIP);
         cyl.bottomCircle.Draw(GL_TRIANGLE_FAN);
     }


     static void drawCone(const ConeMeshes& cone) {
         cone.bottomCircle.Draw(GL_TRIANGLE_FAN);
         cone.side.Draw(GL_TRIANGLE_STRIP);
     }


     // (Prism)
     static Mesh generatePrism(glm::vec3 center, float radius, float height, int sides)
     {
         vector<Vertex> vertices;

         float angleStep = glm::two_pi<float>() / sides;
         float halfH = height * 0.5f;

         glm::vec3 topCenter = center + glm::vec3(0, 0, halfH);
         glm::vec3 bottomCenter = center + glm::vec3(0, 0, -halfH);

         for (int i = 0; i < sides; i++)
         {
             float theta1 = i * angleStep;
             float theta2 = (i + 1) % sides * angleStep;

             glm::vec3 p1 = topCenter + glm::vec3(glm::cos(theta1) * radius, glm::sin(theta1) * radius, 0);
             glm::vec3 p2 = topCenter + glm::vec3(glm::cos(theta2) * radius, glm::sin(theta2) * radius, 0);

             vertices.push_back({ topCenter, {0.5f, 0.5f} });
             vertices.push_back({ p1, {glm::cos(theta1) * 0.5f + 0.5f, glm::sin(theta1) * 0.5f + 0.5f} });
             vertices.push_back({ p2, {glm::cos(theta2) * 0.5f + 0.5f, glm::sin(theta2) * 0.5f + 0.5f} });
         }

         for (int i = 0; i < sides; i++)
         {
             float theta1 = i * angleStep;
             float theta2 = (i + 1) % sides * angleStep;

             glm::vec3 p1 = bottomCenter + glm::vec3(glm::cos(theta1) * radius, glm::sin(theta1) * radius, 0);
             glm::vec3 p2 = bottomCenter + glm::vec3(glm::cos(theta2) * radius, glm::sin(theta2) * radius, 0);

             vertices.push_back({ bottomCenter, {0.5f, 0.5f} });
             vertices.push_back({ p2, {glm::cos(theta2) * 0.5f + 0.5f, glm::sin(theta2) * 0.5f + 0.5f} });
             vertices.push_back({ p1, {glm::cos(theta1) * 0.5f + 0.5f, glm::sin(theta1) * 0.5f + 0.5f} });
         }

         for (int i = 0; i < sides; i++)
         {
             float theta1 = i * angleStep;
             float theta2 = (i + 1) % sides * angleStep;

             glm::vec3 b1 = bottomCenter + glm::vec3(glm::cos(theta1) * radius, glm::sin(theta1) * radius, 0);
             glm::vec3 b2 = bottomCenter + glm::vec3(glm::cos(theta2) * radius, glm::sin(theta2) * radius, 0);

             glm::vec3 t1 = topCenter + glm::vec3(glm::cos(theta1) * radius, glm::sin(theta1) * radius, 0);
             glm::vec3 t2 = topCenter + glm::vec3(glm::cos(theta2) * radius, glm::sin(theta2) * radius, 0);

             vertices.push_back({ b1, {0,0} });
             vertices.push_back({ t1, {0,1} });
             vertices.push_back({ t2, {1,1} });

             vertices.push_back({ b1, {0,0} });
             vertices.push_back({ t2, {1,1} });
             vertices.push_back({ b2, {1,0} });
         }

         return Mesh(vertices);
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

        vector<Vertex> vertices = {
            {{center.x - h, center.y - h, center.z}, {0.0f, 0.0f}},
            {{center.x + h, center.y - h, center.z}, {1.0f, 0.0f}},
            {{center.x + h, center.y + h, center.z}, {1.0f, 1.0f}},

            {{center.x - h, center.y - h, center.z}, {0.0f, 0.0f}},
            {{center.x + h, center.y + h, center.z}, {1.0f, 1.0f}},
            {{center.x - h, center.y + h, center.z}, {0.0f, 1.0f}}
        };

        // Front
        vertices.push_back({ {center.x - h, center.y - h, center.z}, {0.0f, 0.0f} });
        vertices.push_back({ top, {0.5f, 1.0f} });
        vertices.push_back({ {center.x + h, center.y - h, center.z}, {1.0f, 0.0f} });

        // Right
        vertices.push_back({ {center.x + h, center.y - h, center.z}, {0.0f, 0.0f} });
        vertices.push_back({ top, {0.5f, 1.0f} });
        vertices.push_back({ {center.x + h, center.y + h, center.z}, {1.0f, 0.0f} });
                       
        // Back
        vertices.push_back({ {center.x + h, center.y + h, center.z}, {0.0f, 0.0f} });
        vertices.push_back({ top, {0.5f, 1.0f} });
        vertices.push_back({ {center.x - h, center.y + h, center.z}, {1.0f, 0.0f} });

        // Left
        vertices.push_back({ {center.x - h, center.y + h, center.z}, {0.0f, 0.0f} });
        vertices.push_back({ top, {0.5f, 1.0f} });
        vertices.push_back({ {center.x - h, center.y - h, center.z}, {1.0f, 0.0f} });

        return vertices;
    }

};
