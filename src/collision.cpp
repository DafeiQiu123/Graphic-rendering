#include <realtime.h>

    bool Realtime::checkHorizontalCollision(const glm::vec3 &position, float radius) {
    // Compute the character's bounding box with adjusted vertical size
    glm::vec3 charMin = position - glm::vec3(radius, radius * 0.8f, radius);
    glm::vec3 charMax = position + glm::vec3(radius, radius * 0.8f, radius);

    // Calculate grid positions for spatial partitioning
    int startX = std::max(0, std::min(4, static_cast<int>(floor(position.x + 0.5f))));
    int startY = std::max(0, std::min(8, static_cast<int>(floor(position.y + 0.5f))));
    int startZ = std::max(0, std::min(4, static_cast<int>(floor(position.z + 0.5f))));

    // Check surrounding blocks with expanded vertical range during jump
    for (int x = std::max(0, startX - 1); x <= std::min(4, startX + 1); x++) {
        for (int y = std::max(0, startY - 1); y <= std::min(8, startY + 1); y++) {
            for (int z = std::max(0, startZ - 1); z <= std::min(4, startZ + 1); z++) {
                const auto &blockHitbox = m_mapHitbox[x][y][z];

                // Verify block exists at these coordinates
                if ((int)blockHitbox.cood.x == x &&
                    (int)blockHitbox.cood.y == y &&
                    (int)blockHitbox.cood.z == z) {

                    // Add small tolerance to prevent sticking
                    glm::vec3 adjustedBlockMin = blockHitbox.hitBoxA + glm::vec3(0.01f);
                    glm::vec3 adjustedBlockMax = blockHitbox.hitBoxB - glm::vec3(0.01f);

                    if (checkBoxIntersection(charMin, charMax, adjustedBlockMin, adjustedBlockMax)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Realtime::isOnGround(const glm::vec3& position, float radius) {
    // Calculate character's bounding box with slightly reduced horizontal size
    float horizontalRadius = radius * 0.9f;
    glm::vec3 characterMin = position - glm::vec3(horizontalRadius, radius, horizontalRadius);
    glm::vec3 characterMax = position + glm::vec3(horizontalRadius, radius, horizontalRadius);

    // Ground plane check
    if (characterMin.y <= 0.0f) {
        return true;
    }

    // Get grid coordinates
    int gridX = static_cast<int>(floor(position.x + 0.5f));
    int gridY = static_cast<int>(floor(position.y - radius));  // Check below current position
    int gridZ = static_cast<int>(floor(position.z + 0.5f));

    // Check surrounding blocks for better ground detection
    for (int x = std::max(0, gridX - 1); x <= std::min(4, gridX + 1); x++) {
        for (int z = std::max(0, gridZ - 1); z <= std::min(4, gridZ + 1); z++) {
            if (gridY >= 0 && gridY < 9) {
                const auto& blockHitbox = m_mapHitbox[x][gridY][z];

                if (static_cast<int>(blockHitbox.cood.x) == x &&
                    static_cast<int>(blockHitbox.cood.y) == gridY &&
                    static_cast<int>(blockHitbox.cood.z) == z) {

                    // Add small threshold for smoother landing
                    bool xOverlap = characterMax.x >= blockHitbox.hitBoxA.x &&
                                    characterMin.x <= blockHitbox.hitBoxB.x;
                    bool zOverlap = characterMax.z >= blockHitbox.hitBoxA.z &&
                                    characterMin.z <= blockHitbox.hitBoxB.z;
                    bool yCheck = characterMin.y <= blockHitbox.hitBoxB.y + 0.05f;

                    if (xOverlap && zOverlap && yCheck) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Realtime::checkBoxIntersection(const glm::vec3& box1Min, const glm::vec3& box1Max,
                                    const glm::vec3& box2Min, const glm::vec3& box2Max) {
    return (box1Max.x > box2Min.x && box1Min.x < box2Max.x) &&
           (box1Max.y > box2Min.y && box1Min.y < box2Max.y) &&
           (box1Max.z > box2Min.z && box1Min.z < box2Max.z);
}
