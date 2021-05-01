#define LOG_TAG "Runner"

#include "runner.h"
#include "animationManager.h"
#include "animationType.h"
#include "core.h"
#include "objectType.h"
#include "physicalBody.h"
#include "utils.h"

#include <Box2D/Dynamics/b2Fixture.h>

const int Runner::mRunnerHitpoints = 1;
const OBJECT_TYPE Runner::mRunnerType = OBJECT_TYPE::RUNNER;
const ANIMATION_TYPE Runner::mRunnerAnimationType = ANIMATION_TYPE::RUNNER;
const b2Vec2 Runner::mRunnerVelocity = {20.f, 0.f};

const sf::Time Runner::mTurnDelay = sf::seconds(1.5f);

Runner::Runner(std::unique_ptr<PhysicalBody> body)
    : Entity{mRunnerHitpoints, mRunnerType, mRunnerAnimationType,
             HEADING::RIGHT},
      mPhysBody(), mTimeSinceLastTurn(sf::Time::Zero) {
  NOT_NULL(body);
  CHECK(body->getType() == mRunnerType);

  mPhysBody = std::move(body);

  auto &rawBody = mPhysBody->getBody();
  b2Fixture *const fixture = rawBody.GetFixtureList();

  NOT_NULL(fixture);
  IS_NULL(fixture->GetNext());

  fixture->SetUserData(getUserData());
  setEntityPosition(rawBody);

  rawBody.SetLinearVelocity(toB2Coords(mRunnerVelocity));
}

void Runner::update(sf::Time dt) {
  mTimeSinceLastTurn += dt;

  while (mTimeSinceLastTurn > mTurnDelay) {
    mTimeSinceLastTurn -= mTurnDelay;

    auto &rawBody = mPhysBody->getBody();
    auto velocity = rawBody.GetLinearVelocity();
    velocity.x *= -1.f;
    rawBody.SetLinearVelocity(velocity);

    changeHeading();
  }

  getAnimationManager().update(dt);
  setEntityPosition(mPhysBody->getBody());
}

OBJECT_TYPE Runner::getType() const { return mRunnerType; }
