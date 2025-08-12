#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/math/vec3.hpp"
#include "farixEngine/utils/uuid.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

namespace farixEngine {

std::shared_ptr<Mesh> Mesh::createQuad(Vec3 size, std::string eid) {
  auto mesh = std::make_shared<Mesh>();
  mesh->type = "Sprite";
  mesh->size = {size[0], size[1], 0};
mesh->id = eid.empty() ? utils::generateUUID() : eid;

  float w = size[0] / 2.0f; 
  float h = size[1] / 2.0f;

  mesh->vertices = {
      {Vec3(-w, -h, 0), Vec3(0, 0, 1), Vec2(0, 0)},
      {Vec3(w, -h, 0), Vec3(0, 0, 1), Vec2(1, 0)},
      {Vec3(w, h, 0), Vec3(0, 0, 1), Vec2(1, 1)},
      {Vec3(-w, h, 0), Vec3(0, 0, 1), Vec2(0, 1)},
  };

  mesh->indices = {0, 1, 2, 0, 2, 3};

  return mesh;
}

std::shared_ptr<Mesh> Mesh::createSphere(float radius, int latSegments,
                                         int lonSegments, std::string eid) {
  auto mesh = std::make_shared<Mesh>();
  mesh->type = "Sphere";
  mesh->sphereData = {radius, (float)latSegments, (float)lonSegments};
mesh->id = eid.empty() ? utils::generateUUID() : eid;
  for (int lat = 0; lat <= latSegments; ++lat) {
    float theta = lat * M_PI / latSegments;
    float sinTheta = sin(theta);
    float cosTheta = cos(theta);

    for (int lon = 0; lon <= lonSegments; ++lon) {
      float phi = lon * 2.0f * M_PI / lonSegments;
      float sinPhi = sin(phi);
      float cosPhi = cos(phi);

      Vec3 normal(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
      Vec3 position = normal * radius;
      Vec2 uv((float)lon / lonSegments, 1.0f - (float)lat / latSegments);

      mesh->vertices.push_back({position, normal, uv});
    }
  }

  for (int lat = 0; lat < latSegments; ++lat) {
    for (int lon = 0; lon < lonSegments; ++lon) {
      int current = lat * (lonSegments + 1) + lon;
      int next = current + lonSegments + 1;

      mesh->indices.push_back(current);
      mesh->indices.push_back(current + 1);
      mesh->indices.push_back(next);

      mesh->indices.push_back(next);
      mesh->indices.push_back(current + 1);
      mesh->indices.push_back(next + 1);
    }
  }

  return mesh;
}

std::shared_ptr<Mesh> Mesh::createBox(float width, float height, float depth, std::string eid) {
  auto mesh = std::make_shared<Mesh>();
  mesh->type = "Box";
  mesh->size = {width, height, depth};
  mesh->id = eid.empty() ? utils::generateUUID() : eid;
  float w = width / 2.0f;
  float h = height / 2.0f;
  float d = depth / 2.0f;

  mesh->vertices = {
      // Front face
      {{-w, -h, d}, {0, 0, 1}, {0, 0}},
      {{w, -h, d}, {0, 0, 1}, {1, 0}},
      {{w, h, d}, {0, 0, 1}, {1, 1}},
      {{-w, h, d}, {0, 0, 1}, {0, 1}},

      // Back face
      {{w, -h, -d}, {0, 0, -1}, {0, 0}},
      {{-w, -h, -d}, {0, 0, -1}, {1, 0}},
      {{-w, h, -d}, {0, 0, -1}, {1, 1}},
      {{w, h, -d}, {0, 0, -1}, {0, 1}},

      // Left face
      {{-w, -h, -d}, {-1, 0, 0}, {0, 0}},
      {{-w, -h, d}, {-1, 0, 0}, {1, 0}},
      {{-w, h, d}, {-1, 0, 0}, {1, 1}},
      {{-w, h, -d}, {-1, 0, 0}, {0, 1}},

      // Right face
      {{w, -h, d}, {1, 0, 0}, {0, 0}},
      {{w, -h, -d}, {1, 0, 0}, {1, 0}},
      {{w, h, -d}, {1, 0, 0}, {1, 1}},
      {{w, h, d}, {1, 0, 0}, {0, 1}},

      // Top face
      {{-w, h, d}, {0, 1, 0}, {0, 0}},
      {{w, h, d}, {0, 1, 0}, {1, 0}},
      {{w, h, -d}, {0, 1, 0}, {1, 1}},
      {{-w, h, -d}, {0, 1, 0}, {0, 1}},

      // Bottom face
      {{-w, -h, -d}, {0, -1, 0}, {0, 0}},
      {{w, -h, -d}, {0, -1, 0}, {1, 0}},
      {{w, -h, d}, {0, -1, 0}, {1, 1}},
      {{-w, -h, d}, {0, -1, 0}, {0, 1}},
  };

  // Indices for each face (two triangles per face)
  for (int face = 0; face < 6; ++face) {
    int base = face * 4;
    mesh->indices.push_back(base + 0);
    mesh->indices.push_back(base + 1);
    mesh->indices.push_back(base + 2);

    mesh->indices.push_back(base + 0);
    mesh->indices.push_back(base + 2);
    mesh->indices.push_back(base + 3);
  }

  return mesh;
}

std::shared_ptr<Mesh> Mesh::load(const std::string&filename){
  return loadFromObj(filename);
}

std::shared_ptr<Mesh> Mesh::loadFromObj(const std::string &filename, std::string eid) {
  auto parseIndexGroup = [](const std::string &group, uint32_t &v, uint32_t &vt,
                            uint32_t &vn) {
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
  mesh->type = "Obj";
  mesh->id = eid.empty() ? utils::generateUUID() : eid;

  std::ifstream file(filename);
  if (!file.is_open()) {
    return mesh;
  }

  std::string line;
  double scale = 1;

  std::vector<Vec3> tempPositions;
  std::vector<Vec3> tempNormals;
  std::vector<Vec2> tempUvs;
  std::vector<Triangle> tempTriangles;

  while (std::getline(file, line)) {
    std::istringstream iss(line);

    std::string type, a, b, c;
    iss >> type >> a >> b >> c;

    if (type == "v") {
      tempPositions.push_back(Vec3(std::stof(a) * scale, std::stof(b) * scale,
                                   std::stof(c) * scale));
    } else if (type == "vt") {
      tempUvs.push_back(Vec2(std::stof(a), std::stof(b)));
    } else if (type == "vn") {
      tempNormals.push_back(Vec3(std::stof(a), std::stof(b), std::stof(c)));
    } else if (type == "f") {
      uint32_t v0, v1, v2;
      uint32_t vt0, vt1, vt2;
      uint32_t vn0, vn1, vn2;

      parseIndexGroup(a, v0, vt0, vn0);
      parseIndexGroup(b, v1, vt1, vn1);
      parseIndexGroup(c, v2, vt2, vn2);

      tempTriangles.push_back({v0, v1, v2, vn0, vn1, vn2, vt0, vt1, vt2});
    }
  }

  if (tempNormals.empty()) {
    tempNormals.resize(tempPositions.size(), Vec3(0));

    for (const auto &tri : tempTriangles) {
      int i0 = tri.i0, i1 = tri.i1, i2 = tri.i2;
      Vec3 p0 = tempPositions[i0];
      Vec3 p1 = tempPositions[i1];
      Vec3 p2 = tempPositions[i2];

      Vec3 normal = (p1 - p0).cross(p2 - p0).normalized();

      tempNormals[i0] = tempNormals[i0] + normal;
      tempNormals[i1] = tempNormals[i0] + normal;
      tempNormals[i2] = tempNormals[i0] + normal;
    }

    for (auto &n : tempNormals) {
      n = n.normalized();
    }

    for (auto &tri : tempTriangles) {
      tri.n0 = tri.i0;
      tri.n1 = tri.i1;
      tri.n2 = tri.i2;
    }
  }

  if (tempUvs.empty()) {
    tempUvs.resize(tempPositions.size(), Vec2(0, 0));
    for (auto &tri : tempTriangles) {
      tri.uv0 = tri.i0;
      tri.uv1 = tri.i1;
      tri.uv2 = tri.i2;
    }
  }

  std::unordered_map<Vertex, uint32_t> vertexToIndex;

  auto checkVertex = [&](const Vertex &v) -> uint32_t {
    auto it = vertexToIndex.find(v);
    if (it != vertexToIndex.end()) {
      return it->second;
    }

    uint32_t newIndex = static_cast<uint32_t>(mesh->vertices.size());
    mesh->vertices.push_back(v);
    vertexToIndex[v] = newIndex;
    return newIndex;
  };

  for (auto &tri : tempTriangles) {
    Vertex v1{tempPositions[tri.i0], tempNormals[tri.n0], tempUvs[tri.uv0]};
    Vertex v2{tempPositions[tri.i1], tempNormals[tri.n1], tempUvs[tri.uv1]};
    Vertex v3{tempPositions[tri.i2], tempNormals[tri.n2], tempUvs[tri.uv2]};

    

    uint32_t i1 = checkVertex(v1);
    mesh->indices.push_back(i1);
    uint32_t i2 = checkVertex(v2);
    mesh->indices.push_back(i2);
    uint32_t i3 = checkVertex(v3);
    mesh->indices.push_back(i3);
  }

  return mesh;
}
} // namespace farixEngine
