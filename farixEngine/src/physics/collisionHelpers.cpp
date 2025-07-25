
#include "farixEngine/physics/collisionHelpers.hpp"
#include <cmath>

namespace farixEngine::collision {

bool AABBvsAABB(const Vec3 &posA, const Vec3 &sizeA, const Vec3 &posB,
                const Vec3 &sizeB) {

  return (std::abs(posA.x - posB.x) * 2 < (sizeA.x + sizeB.x)) &&
         (std::abs(posA.y - posB.y) * 2 < (sizeA.y + sizeB.y)) &&
         (std::abs(posA.z - posB.z) * 2 < (sizeA.z + sizeB.z));
}

bool SpherevsSphere(const Vec3 &posA, float radiusA, const Vec3 &posB,
                    float radiusB) {
  float distSq = (posA - posB).length();
  float radiusSum = radiusA + radiusB;
  return distSq <= (radiusSum * radiusSum);
}

} // namespace farixEngine::collision
