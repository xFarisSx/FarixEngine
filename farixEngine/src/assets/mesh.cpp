#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/math/vec3.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace farixEngine {

std::shared_ptr<Mesh> Mesh::createQuad(Vec3 size) {
  auto mesh = std::make_shared<Mesh>();
  mesh->type = "Sprite";

  mesh->size = {size[0], size[1], 0};

  float w = size[0] / 2.0f;
  float h = size[1] / 2.0f;

  mesh->vertices = {Vec3(-w, -h, 0), Vec3(w, -h, 0), Vec3(w, h, 0),
                    Vec3(-w, h, 0)};

  mesh->normals = {Vec3(0, 0, 1)};

  mesh->textureMap = {Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(1, 1, 0),
                      Vec3(0, 1, 0)};

  mesh->triangles = {

      {0, 1, 2, 0, 1, 2, 0, 0, 0}, {0, 2, 3, 0, 2, 3, 0, 0, 0}};

  return mesh;
}

std::shared_ptr<Mesh> Mesh::createSphere(float radius, int latSegments,
                                         int lonSegments) {
  auto mesh = std::make_shared<Mesh>();
  mesh->sphereData = {radius, (float)latSegments, (float)lonSegments};

  for (int lat = 0; lat <= latSegments; ++lat) {
    float theta = lat * M_PI / latSegments;
    float sinTheta = sin(theta);
    float cosTheta = cos(theta);

    for (int lon = 0; lon <= lonSegments; ++lon) {
      float phi = lon * 2.0f * M_PI / lonSegments;
      float sinPhi = sin(phi);
      float cosPhi = cos(phi);

      float x = sinTheta * cosPhi;
      float y = cosTheta;
      float z = sinTheta * sinPhi;

      Vec3 normal(x, y, z);
      mesh->vertices.push_back(normal * radius);
      mesh->normals.push_back(normal);
    }
  }

  for (int lat = 0; lat < latSegments; ++lat) {
    for (int lon = 0; lon < lonSegments; ++lon) {
      int current = lat * (lonSegments + 1) + lon;
      int next = current + lonSegments + 1;

      mesh->triangles.push_back({current, current + 1, next, current,
                                 current + 1, next, current, current + 1,
                                 next});
      mesh->triangles.push_back({next, current + 1, next + 1, next, current + 1,
                                 next + 1, next, current + 1, next + 1});
    }
  }

  mesh->type = "Sphere";
  mesh->textureMap.resize(mesh->vertices.size(), Vec3(0));

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
      // Front
      Vec3(-w, -h, d),
      Vec3(w, -h, d),
      Vec3(w, h, d),
      Vec3(-w, h, d),
      // Back
      Vec3(w, -h, -d),
      Vec3(-w, -h, -d),
      Vec3(-w, h, -d),
      Vec3(w, h, -d),
      // Left
      Vec3(-w, -h, -d),
      Vec3(-w, -h, d),
      Vec3(-w, h, d),
      Vec3(-w, h, -d),
      // Right
      Vec3(w, -h, d),
      Vec3(w, -h, -d),
      Vec3(w, h, -d),
      Vec3(w, h, d),
      // Top
      Vec3(-w, h, d),
      Vec3(w, h, d),
      Vec3(w, h, -d),
      Vec3(-w, h, -d),
      // Bottom
      Vec3(-w, -h, -d),
      Vec3(w, -h, -d),
      Vec3(w, -h, d),
      Vec3(-w, -h, d),
  };

  mesh->normals = {
      Vec3(0, 0, 1),  Vec3(0, 0, 1),  Vec3(0, 0, 1),  Vec3(0, 0, 1),  // Front
      Vec3(0, 0, -1), Vec3(0, 0, -1), Vec3(0, 0, -1), Vec3(0, 0, -1), // Back
      Vec3(-1, 0, 0), Vec3(-1, 0, 0), Vec3(-1, 0, 0), Vec3(-1, 0, 0), // Left
      Vec3(1, 0, 0),  Vec3(1, 0, 0),  Vec3(1, 0, 0),  Vec3(1, 0, 0),  // Right
      Vec3(0, 1, 0),  Vec3(0, 1, 0),  Vec3(0, 1, 0),  Vec3(0, 1, 0),  // Top
      Vec3(0, -1, 0), Vec3(0, -1, 0), Vec3(0, -1, 0), Vec3(0, -1, 0)  // Bottom
  };

  mesh->textureMap = std::vector<Vec3>(24, Vec3(0));
  for (int i = 0; i < 6; ++i) {
    mesh->textureMap[i * 4 + 0] = Vec3(0, 0, 0);
    mesh->textureMap[i * 4 + 1] = Vec3(1, 0, 0);
    mesh->textureMap[i * 4 + 2] = Vec3(1, 1, 0);
    mesh->textureMap[i * 4 + 3] = Vec3(0, 1, 0);
  }

  auto addTriangle = [&](int i0, int i1, int i2) {
    mesh->triangles.push_back({i0, i1, i2, i0, i1, i2, i0, i1, i2});
  };

  for (int i = 0; i < 6; ++i) {
    int base = i * 4;
    addTriangle(base + 0, base + 1, base + 2);
    addTriangle(base + 0, base + 2, base + 3);
  }

  return mesh;
}
std::shared_ptr<Mesh> Mesh::loadFromObj(const std::string &filename) {
  auto parseIndexGroup = [](const std::string &group, int &v, int &vt,
                            int &vn) {
    size_t firstSlash = group.find('/');
    size_t secondSlash = group.find('/', firstSlash + 1);

    if (firstSlash == std::string::npos) {
      // vertex index
      v = std::stoi(group) - 1;
      vt = vn = -1;
    } else if (secondSlash == std::string::npos) {
      // Vertex/texture
      v = std::stoi(group.substr(0, firstSlash)) - 1;
      vt = std::stoi(group.substr(firstSlash + 1)) - 1;
      vn = -1;
    } else if (secondSlash == firstSlash + 1) {
      // Vertex//normal
      v = std::stoi(group.substr(0, firstSlash)) - 1;
      vt = -1;
      vn = std::stoi(group.substr(secondSlash + 1)) - 1;
    } else {
      // Vertex/texture/normal
      v = std::stoi(group.substr(0, firstSlash)) - 1;
      vt = std::stoi(
               group.substr(firstSlash + 1, secondSlash - firstSlash - 1)) -
           1;
      vn = std::stoi(group.substr(secondSlash + 1)) - 1;
    }
  };

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

      int v0, v1, v2;
      int vt0, vt1, vt2;
      int vn0, vn1, vn2;

      parseIndexGroup(a, v0, vt0, vn0);
      parseIndexGroup(b, v1, vt1, vn1);
      parseIndexGroup(c, v2, vt2, vn2);

      mesh->triangles.push_back({v0, v1, v2, vt0, vt1, vt2, vn0, vn1, vn2});
    }
  }

  return mesh;
}
} // namespace farixEngine
