#ifndef SHOOTER_H
#define SHOOTER_H

#include "entity.h"

#include <functional>

class Shooter : public Entity {
public:
  using BulletSpawnCallback = std::function<void(
      HEADING heading, OBJECT_TYPE type, const sf::Vector2f &position)>;

  explicit Shooter(int maxHitpoints, OBJECT_TYPE objectType,
                   ANIMATION_TYPE animationType, HEADING heading);

  void setBulletSpawnCallback(const BulletSpawnCallback &cb);

protected:
  BulletSpawnCallback mBulletSpawnCallback;
};

#endif // SHOOTER_H
