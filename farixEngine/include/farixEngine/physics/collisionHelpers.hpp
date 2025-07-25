
#pragma once
#include "farixEngine/math/vec3.hpp" 

namespace farixEngine::collision {

bool AABBvsAABB(const Vec3 &posA, const Vec3 &sizeA, const Vec3 &posB, const Vec3 &sizeB);
bool SpherevsSphere(const Vec3 &posA, float radiusA, const Vec3 &posB, float radiusB);

} // namespace farixEngine::collision
