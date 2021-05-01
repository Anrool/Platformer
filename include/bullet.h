#ifndef BULLET_H
#define BULLET_H

#include "entity.h"

#include <SFML/System/Time.hpp>

enum class HEADING;
class PhysicalBody;

class Bullet : public Entity {
public:
  explicit Bullet(HEADING heading, std::unique_ptr<PhysicalBody> body);

  void update(sf::Time dt) final;

  bool isDestroyed() const final;

  OBJECT_TYPE getType() const final;

private:
  static const int mBulletHitpoints;
  static const float mBulletVelocity;

  OBJECT_TYPE mType;
  std::unique_ptr<PhysicalBody> mPhysBody;

  sf::Time mExplodeCountdown;

  ANIMATION_TYPE objToAnimationType(OBJECT_TYPE type) const;
};

#endif // BULLET_H
