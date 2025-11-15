#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glad/glad.h>
using namespace std;

// ================== Vertex Struct ==================
struct Vertex {
    glm::vec3 pos;
    glm::vec2 texCoords;
    glm::vec4 color = glm::vec4(1.0f); // «› —«÷Ì √»Ì÷
    glm::vec3 normal = glm::vec3(0, 0, 1); // <-- ≈÷«›…: ‰Ê—„«· «› —«÷Ì
};


// ================== Mesh Class ==================
class Mesh {
public:
    unsigned int VAO, VBO;
    vector<Vertex> vertices;
    glm::vec4 meshColor = glm::vec4(1.0f); // «··Ê‰ «·«› —«÷Ì
    bool useColor = false;                  // false = texture, true = color

    Mesh(const vector<Vertex>& verts, bool _useColor = false, glm::vec4 _color = glm::vec4(1.0f))
        : vertices(verts), useColor(_useColor), meshColor(_color) {
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

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));


        glBindVertexArray(0);
    }
};

// ================== Specialized Meshes ==================
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

// ================== Shape Generator ==================
class ShapeGenerator {
public:

    // ---------------- Circle ----------------
    static vector<Vertex> generateCircleFan(const glm::vec3& center, float radius, int segments = 36, bool useColor = false, glm::vec4 color = glm::vec4(1.0f)) {
        vector<Vertex> vertices;

        glm::vec3 normal(0, 0, 1); // NORMAL ADDED

        Vertex centerVert;
        centerVert.pos = center;
        centerVert.texCoords = glm::vec2(0.5f, 0.5f);
        centerVert.color = color;
        centerVert.normal = normal; // NORMAL ADDED
        vertices.push_back(centerVert);

        float angleStep = glm::two_pi<float>() / segments;
        for (int i = 0; i <= segments; ++i) {
            float theta = i * angleStep;
            glm::vec3 pos = center + glm::vec3(glm::cos(theta) * radius, glm::sin(theta) * radius, 0.0f);
            glm::vec2 uv = glm::vec2(glm::cos(theta) * 0.5f + 0.5f, glm::sin(theta) * 0.5f + 0.5f);
            vertices.push_back({ pos, uv, color, normal }); // NORMAL ADDED
        }
        return vertices;
    }


    // ---------------- Rectangle ----------------
    static vector<Vertex> generateRectangle(const glm::vec3& center, float width, float height, bool useColor = false, glm::vec4 color = glm::vec4(1.0f)) {
        float hw = width / 2.0f, hh = height / 2.0f;
        glm::vec3 normal(0, 0, 1); // NORMAL ADDED

        return {
            {{center.x - hw, center.y - hh, center.z}, {0.0f,0.0f}, color, normal},
            {{center.x + hw, center.y - hh, center.z}, {1.0f,0.0f}, color, normal},
            {{center.x + hw, center.y + hh, center.z}, {1.0f,1.0f}, color, normal},
            {{center.x - hw, center.y + hh, center.z}, {0.0f,1.0f}, color, normal}
        };
    }


    // ---------------- Cylinder Side ----------------
    static vector<Vertex> generateCylinderSide(const glm::vec3& center, float radius, float height, int segments = 36, bool cone = false, glm::vec4 color = glm::vec4(1.0f)) {
        vector<Vertex> vertices;
        float angleStep = glm::two_pi<float>() / segments;

        for (int i = 0; i <= segments; i++) {
            float theta = i * angleStep;
            glm::vec3 dir = glm::normalize(glm::vec3(glm::cos(theta), glm::sin(theta), 0)); // NORMAL ADDED

            glm::vec3 bottomPos = center + glm::vec3(glm::cos(theta) * radius, glm::sin(theta) * radius, 0.0f);
            glm::vec3 topPos = cone ? center + glm::vec3(0, 0, height) : bottomPos + glm::vec3(0, 0, height);
            glm::vec2 bottomUV = glm::vec2((float)i / segments, 0.0f);
            glm::vec2 topUV = glm::vec2((float)i / segments, 1.0f);

            vertices.push_back({ bottomPos, bottomUV, color, dir }); // NORMAL ADDED
            vertices.push_back({ topPos, topUV, color, dir });       // NORMAL ADDED
        }
        return vertices;
    }


    // ---------------- Cylinder ----------------
    static CylinderMeshes generateCylinder(const glm::vec3& center, float radius, float height, bool useColor = false, glm::vec4 color = glm::vec4(1.0f)) {
        return CylinderMeshes(
            Mesh(generateCircleFan(center + glm::vec3(0, 0, height), radius, 100, useColor, color), useColor, color),
            Mesh(generateCylinderSide(center, radius, height, 100, false, color), useColor, color),
            Mesh(generateCircleFan(center, radius, 36, useColor, color), useColor, color)
        );
    }

    // ---------------- Cone ----------------
    static ConeMeshes generateCone(const glm::vec3& center, float radius, float height, bool useColor = false, glm::vec4 color = glm::vec4(1.0f)) {
        return ConeMeshes(
            Mesh(generateCircleFan(center, radius, 100, useColor, color), useColor, color),
            Mesh(generateCylinderSide(center, radius, height, 100, true, color), useColor, color)
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


    // ---------------- Pyramid ----------------
    static Mesh generatePyramid(const glm::vec3& center, float baseSize, float height, bool useColor = false, glm::vec4 color = glm::vec4(1.0f)) {
        float h = baseSize / 2.0f;
        glm::vec3 top = center + glm::vec3(0, 0, height);

        vector<Vertex> vertices;

        auto addTri = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec2 uva, glm::vec2 uvb, glm::vec2 uvc) {
            glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a)); // NORMAL ADDED
            vertices.push_back({ a, uva, color, normal });
            vertices.push_back({ b, uvb, color, normal });
            vertices.push_back({ c, uvc, color, normal });
            };

        // «·ﬁ«⁄œ… (ÊÃÂÌ‰)
        addTri(
            { center.x - h, center.y - h, center.z },
            { center.x + h, center.y - h, center.z },
            { center.x + h, center.y + h, center.z },
            { 0,0 }, { 1,0 }, { 1,1 }
        );

        addTri(
            { center.x - h, center.y - h, center.z },
            { center.x + h, center.y + h, center.z },
            { center.x - h, center.y + h, center.z },
            { 0,0 }, { 1,1 }, { 0,1 }
        );

        // «·ÊÃÊÂ «·√—»⁄…
        addTri(
            { center.x - h, center.y - h, center.z },
            { center.x + h, center.y - h, center.z },
            top,
            { 0,0 }, { 1,0 }, { 0.5,1 }
        );

        addTri(
            { center.x + h, center.y - h, center.z },
            { center.x + h, center.y + h, center.z },
            top,
            { 0,0 }, { 1,0 }, { 0.5,1 }
        );

        addTri(
            { center.x + h, center.y + h, center.z },
            { center.x - h, center.y + h, center.z },
            top,
            { 0,0 }, { 1,0 }, { 0.5,1 }
        );

        addTri(
            { center.x - h, center.y + h, center.z },
            { center.x - h, center.y - h, center.z },
            top,
            { 0,0 }, { 1,0 }, { 0.5,1 }
        );

        return Mesh(vertices, useColor, color);
    }



    // ---------------- Prism ----------------
    static Mesh generatePrism(glm::vec3 center, float radius, float height, int sides, bool useColor = false, glm::vec4 color = glm::vec4(1.0f)) {
        vector<Vertex> vertices;
        float angleStep = glm::two_pi<float>() / sides;
        float halfH = height * 0.5f;
        glm::vec3 topCenter = center + glm::vec3(0, 0, halfH);
        glm::vec3 bottomCenter = center + glm::vec3(0, 0, -halfH);

        glm::vec3 topNormal(0, 0, 1);     // NORMAL ADDED
        glm::vec3 bottomNormal(0, 0, -1); // NORMAL ADDED

        // top
        for (int i = 0; i < sides; i++) {
            float t1 = i * angleStep;
            float t2 = (i + 1) % sides * angleStep;
            glm::vec3 p1 = topCenter + glm::vec3(glm::cos(t1) * radius, glm::sin(t1) * radius, 0);
            glm::vec3 p2 = topCenter + glm::vec3(glm::cos(t2) * radius, glm::sin(t2) * radius, 0);

            vertices.push_back({ topCenter,{0.5,0.5}, color, topNormal });
            vertices.push_back({ p1,{ glm::cos(t1) * 0.5f + 0.5f, glm::sin(t1) * 0.5f + 0.5f }, color, topNormal });
            vertices.push_back({ p2,{ glm::cos(t2) * 0.5f + 0.5f, glm::sin(t2) * 0.5f + 0.5f }, color, topNormal });
        }

        // bottom
        for (int i = 0; i < sides; i++) {
            float t1 = i * angleStep;
            float t2 = (i + 1) % sides * angleStep;
            glm::vec3 p1 = bottomCenter + glm::vec3(glm::cos(t1) * radius, glm::sin(t1) * radius, 0);
            glm::vec3 p2 = bottomCenter + glm::vec3(glm::cos(t2) * radius, glm::sin(t2) * radius, 0);

            vertices.push_back({ bottomCenter,{0.5,0.5}, color, bottomNormal });
            vertices.push_back({ p2,{ glm::cos(t2) * 0.5f + 0.5f, glm::sin(t2) * 0.5f + 0.5f }, color, bottomNormal });
            vertices.push_back({ p1,{ glm::cos(t1) * 0.5f + 0.5f, glm::sin(t1) * 0.5f + 0.5f }, color, bottomNormal });
        }

        // sides
        for (int i = 0; i < sides; i++) {
            float t1 = i * angleStep;
            float t2 = (i + 1) % sides * angleStep;
            glm::vec3 b1 = bottomCenter + glm::vec3(glm::cos(t1) * radius, glm::sin(t1) * radius, 0);
            glm::vec3 b2 = bottomCenter + glm::vec3(glm::cos(t2) * radius, glm::sin(t2) * radius, 0);
            glm::vec3 t1p = topCenter + glm::vec3(glm::cos(t1) * radius, glm::sin(t1) * radius, 0);
            glm::vec3 t2p = topCenter + glm::vec3(glm::cos(t2) * radius, glm::sin(t2) * radius, 0);

            glm::vec3 normal = glm::normalize(glm::cross(t1p - b1, t2p - b1)); // NORMAL ADDED

            vertices.push_back({ b1,{0,0}, color, normal });
            vertices.push_back({ t1p,{0,1}, color, normal });
            vertices.push_back({ t2p,{1,1}, color, normal });

            vertices.push_back({ b1,{0,0}, color, normal });
            vertices.push_back({ t2p,{1,1}, color, normal });
            vertices.push_back({ b2,{1,0}, color, normal });
        }

        return Mesh(vertices, useColor, color);
    }


};
