#pragma once

#include <vector>
#include <string>
#include "Vector3.h"
#include "FrameBuffer.h"

#include <fstream>
#include <sstream>

struct Triangle {
    Vector3 vertices[3];
};

#include <cstdlib>
#include <cstdint>

uint32_t random_u32() {
    return (uint32_t(rand()) << 16) ^ uint32_t(rand());
}

class Mesh {
    private:
        std::vector<Vector3> vertices = {};
        std::vector<int> indices = {};

        std::vector<Triangle> getAllTriangles() {
            std::vector<Triangle> result;

            int n = numFaces();

            for (int face = 0; face < n; face++) {
                int index = face * 3;

                result.push_back({vertices[indices[index]], vertices[indices[index + 1]], vertices[indices[index + 2]]});
            }

            return result;
        }

    public:
        Mesh(const std::string filename) {
            
            std::ifstream file(filename);
            if (!file) return;

            std::string line;
            while (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string type;
                ss >> type;

                if (type == "v") { // vertex
                    float x, y, z;
                    ss >> x >> y >> z;
                    vertices.emplace_back(x, y, z);
                } else if (type == "f") {
                    std::string v1, v2, v3;
                    ss >> v1 >> v2 >> v3;

                    auto posIndex = [](const std::string& s) {
                        size_t slash = s.find('/');
                        return std::stoi(s.substr(0, slash)) - 1;
                    };

                    indices.push_back(posIndex(v1));
                    indices.push_back(posIndex(v2));
                    indices.push_back(posIndex(v3));
                }
            }
        }

        int numVertices() const { return vertices.size(); }
        int numFaces() const { return indices.size() / 3; }

        void draw(FrameBuffer &fb, uint32_t color) {
            auto triangles = getAllTriangles();

            int scale = fb.height / 2;
            int halfWidth = fb.width / 2;

            for (const auto& triangle : triangles) {
                drawTriangle(fb, 
                    triangle.vertices[0].x * scale + halfWidth, triangle.vertices[0].y * scale + scale,
                    triangle.vertices[1].x * scale + halfWidth, triangle.vertices[1].y * scale + scale,
                    triangle.vertices[2].x * scale + halfWidth, triangle.vertices[2].y * scale + scale, argb(127 + triangle.vertices[0].z * 127, 0, 0));
            }
        }
};