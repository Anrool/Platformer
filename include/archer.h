#ifndef ARCHER_H
#define ARCHER_H

#include "shooter.h"

#include <SFML/System/Time.hpp>

class PhysicalBody;

class Archer : public Shooter {
public:
  explicit Archer(std::unique_ptr<PhysicalBody> body);

  void update(sf::Time dt) final;

  OBJECT_TYPE getType() const final;

private:
  static const int mArcherHitpoints;
  static const OBJECT_TYPE mArcherType;
  static const ANIMATION_TYPE mArcherAnimationType;

  static const sf::Time mShootingDelay;
  static const OBJECT_TYPE mBullerType;

  static const sf::Vector2f mBulletRightOffset;
  static const sf::Vector2f mBulletLeftOffset;
  static const float mHiddenHeight;

  std::unique_ptr<PhysicalBody> mPhysBody;
  sf::Time mShootingCountdown;

  void spawnBullet();

  void onDraw(sf::RenderStates &states) const final;
};

#endif // ARCHER_H
