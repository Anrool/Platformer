#define LOG_TAG "Platform"

#include "platform.h"
#include "animationManager.h"
#include "animationType.h"
#include "core.h"
#include "objectType.h"
#include "physicalBody.h"
#include "utils.h"

#include <Box2D/Dynamics/b2Fixture.h>

const int Platform::mPlatformHitpoints = 1;
const ANIMATION_TYPE Platform::mPlatformAnimationType =
    ANIMATION_TYPE::PLATFORM;
const b2Vec2 Platform::mHorizontalPlatformVel = {125.f, 0.f};
const b2Vec2 Platform::mVerticalPlatformVel = {0.f, 125.f};

const sf::Time Platform::mTurnDelay = sf::seconds(1.f);

Platform::Platform(std::unique_ptr<PhysicalBody> body)
    : Entity{mPlatformHitpoints, body->getType(), mPlatformAnimationType,
             HEADING::RIGHT},
      mPhysBody(), mTimeSinceLastTurn(sf::Time::Zero) {
  NOT_NULL(body);

  const auto type = body->getType();

  CHECK(type == OBJECT_TYPE::HORIZONTAL_PLATFORM ||
        type == OBJECT_TYPE::VERTICAL_PLATFORM);

  mPhysBody = std::move(body);

  auto &rawBody = mPhysBody->getBody();
  b2Fixture *const fixture = rawBody.GetFixtureList();

  NOT_NULL(fixture);
  IS_NULL(fixture->GetNext());

  fixture->SetUserData(getUserData());
  setEntityPosition(rawBody);

  const auto velocity = type == OBJECT_TYPE::HORIZONTAL_PLATFORM
                            ? mHorizontalPlatformVel
                            : mVerticalPlatformVel;
  rawBody.SetLinearVelocity(toB2Coords(velocity));
}

void Platform::update(sf::Time dt) {
  mTimeSinceLastTurn += dt;

  while (mTimeSinceLastTurn > mTurnDelay) {
    mTimeSinceLastTurn -= mTurnDelay;

    auto &rawBody = mPhysBody->getBody();
    auto velocity = rawBody.GetLinearVelocity();
    rawBody.SetLinearVelocity(-velocity);
  }

  getAnimationManager().update(dt);
  setEntityPosition(mPhysBody->getBody());
}

OBJECT_TYPE Platform::getType() const { return mPhysBody->getType(); }
