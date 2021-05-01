#define LOG_TAG "Shooter"

#include "shooter.h"
#include "core.h"

Shooter::Shooter(int maxHitpoints, OBJECT_TYPE objectType,
                 ANIMATION_TYPE animationType, HEADING heading)
    : Entity{maxHitpoints, objectType, animationType, heading},
      mBulletSpawnCallback([](HEADING /*heading*/, OBJECT_TYPE /*type*/,
                              const sf::Vector2f & /*pos*/) { CHECK(false); }) {
}

void Shooter::setBulletSpawnCallback(const BulletSpawnCallback &cb) {
  NOT_NULL(cb);

  mBulletSpawnCallback = cb;
}
