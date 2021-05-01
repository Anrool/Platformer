#ifndef PLATFORM_H
#define PLATFORM_H

#include "entity.h"

#include <SFML/System/Time.hpp>

class PhysicalBody;
struct b2Vec2;

class Platform : public Entity {
public:
  explicit Platform(std::unique_ptr<PhysicalBody> body);

  void update(sf::Time dt) final;

  OBJECT_TYPE getType() const final;

private:
  static const int mPlatformHitpoints;
  static const ANIMATION_TYPE mPlatformAnimationType;
  static const b2Vec2 mHorizontalPlatformVel;
  static const b2Vec2 mVerticalPlatformVel;

  static const sf::Time mTurnDelay;

  std::unique_ptr<PhysicalBody> mPhysBody;
  sf::Time mTimeSinceLastTurn;
};

#endif // PLATFORM_H
