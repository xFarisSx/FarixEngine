#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/math/vec3.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace farixEngine {

std::shared_ptr<Mesh> Mesh::createSphere(float radius, int latSegments,
                                         int lonSegments) {
  auto mesh = std::make_shared<Mesh>();
  mesh->sphereData = {radius, (float)latSegments, (float)lonSegments};
  // Generate vertices
  for (int lat = 0; lat <= latSegments; ++lat) {
    float theta = lat * M_PI / latSegments; // [0, π]
    float sinTheta = sin(theta);
    float cosTheta = cos(theta);

    for (int lon = 0; lon <= lonSegments; ++lon) {
      float phi = lon * 2.0f * M_PI / lonSegments; // [0, 2π]
      float sinPhi = sin(phi);
      float cosPhi = cos(phi);

      float x = radius * sinTheta * cosPhi;
      float y = radius * cosTheta;
      float z = radius * sinTheta * sinPhi;

      mesh->vertices.push_back(Vec3(x, y, z));
    }
  }

  for (int lat = 0; lat < latSegments; ++lat) {
    for (int lon = 0; lon < lonSegments; ++lon) {
      int current = lat * (lonSegments + 1) + lon;
      int next = current + lonSegments + 1;

      mesh->triangles.push_back({current, current + 1, next});
      mesh->triangles.push_back({next + 1, next, current + 1});
    }
  }

  mesh->type = "Sphere";
  mesh->textureMap.resize(mesh->vertices.size(), Vec3(0)); // dummy UV

  return mesh;
}
std::shared_ptr<Mesh> Mesh::createBox(float width, float height, float depth) {
  auto mesh = std::make_shared<Mesh>();
  mesh->type = "Box";

  mesh->size = {width, height, depth};

  float w = width / 2.0f;
  float h = height / 2.0f;
  float d = depth / 2.0f;

  mesh->vertices = {
      Vec3(-w, -h, -d), // 0
      Vec3(w, -h, -d),  // 1
      Vec3(w, h, -d),   // 2
      Vec3(-w, h, -d),  // 3
      Vec3(-w, -h, d),  // 4
      Vec3(w, -h, d),   // 5
      Vec3(w, h, d),    // 6
      Vec3(-w, h, d)    // 7
  };

  mesh->triangles = {// Back face
                     {2, 1, 0},
                     {3, 2, 0},
                     // Front face
                     {4, 5, 6},
                     {4, 6, 7},
                     // Left face
                     {0, 4, 7},
                     {0, 7, 3},
                     // Right face
                     {6, 5, 1},
                     {6, 1, 2},
                     // Top face
                     {3, 7, 6},
                     {3, 6, 2},
                     // Bottom face
                     {0, 1, 5},
                     {0, 5, 4}};

  mesh->textureMap.resize(mesh->vertices.size(), Vec3(0.0f, 0.0f, 0.0f));

  return mesh;
}
std::shared_ptr<Mesh> Mesh::loadFromObj(const std::string &filename) {
  auto mesh = std::make_shared<Mesh>();
  mesh->path = filename;
  std::ifstream file(filename);
  mesh->type = "Obj";

  if (!file.is_open()) {
    return mesh;
  }

  std::string line;
  double scale = 1;
  while (std::getline(file, line)) {
    std::istringstream iss(line);

    std::string type, a, b, c;

    iss >> type >> a >> b >> c;

    if (type == "v") {
      mesh->vertices.push_back(
          Vec3(stof(a) * scale, stof(b) * scale, stof(c) * scale));
    } else if (type == "vt") {
      mesh->textureMap.push_back(Vec3(stof(a), stof(b), stof(c)));

    }

    else if (type == "f") {
      std::string af = a;
      std::string bf = b;
      std::string cf = c;
      std::string av = "-1";
      std::string bv = "-1";
      std::string cv = "-1";

      size_t as = a.find('/');
      if (as != std::string::npos) {
        af = a.substr(0, as);
        av = a.substr(as + 1);
      }
      size_t bs = b.find('/');
      if (bs != std::string::npos) {
        bf = b.substr(0, bs);
        bv = b.substr(bs + 1);
      }
      size_t cs = c.find('/');
      if (cs != std::string::npos) {
        cf = c.substr(0, cs);
        cv = c.substr(cs + 1);
      }

      mesh->triangles.push_back({stoi(af) - 1, stoi(bf) - 1, stoi(cf) - 1,
                                 stoi(av) - 1, stoi(bv) - 1, stoi(cv) - 1});
    }
  }

  return mesh;
}
} // namespace farixEngine
