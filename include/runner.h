#ifndef RUNNER_H
#define RUNNER_H

#include "entity.h"

#include <SFML/System/Time.hpp>

class PhysicalBody;
struct b2Vec2;

class Runner : public Entity {
public:
  explicit Runner(std::unique_ptr<PhysicalBody> body);

  void update(sf::Time dt) final;

  OBJECT_TYPE getType() const final;

private:
  static const int mRunnerHitpoints;
  static const OBJECT_TYPE mRunnerType;
  static const ANIMATION_TYPE mRunnerAnimationType;
  static const b2Vec2 mRunnerVelocity;

  static const sf::Time mTurnDelay;

  std::unique_ptr<PhysicalBody> mPhysBody;
  sf::Time mTimeSinceLastTurn;
};

#endif // RUNNER_H
