#define LOG_TAG "Archer"

#include "archer.h"
#include "animationManager.h"
#include "animationType.h"
#include "core.h"
#include "objectType.h"
#include "physicalBody.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <Box2D/Dynamics/b2Fixture.h>

const int Archer::mArcherHitpoints = 1;
const OBJECT_TYPE Archer::mArcherType = OBJECT_TYPE::ARCHER;
const ANIMATION_TYPE Archer::mArcherAnimationType = ANIMATION_TYPE::ARCHER;

const sf::Time Archer::mShootingDelay = sf::seconds(1.f);
const OBJECT_TYPE Archer::mBullerType = OBJECT_TYPE::ENEMY_BULLET;

const sf::Vector2f Archer::mBulletRightOffset = {38.f, 5.f};
const sf::Vector2f Archer::mBulletLeftOffset = {-38.f, 5.f};
const float Archer::mHiddenHeight = 9.f;

Archer::Archer(std::unique_ptr<PhysicalBody> body)
    : Shooter{mArcherHitpoints, mArcherType, mArcherAnimationType,
              HEADING::RIGHT},
      mPhysBody(), mShootingCountdown(sf::Time::Zero) {
  NOT_NULL(body);
  CHECK(body->getType() == mArcherType);

  mPhysBody = std::move(body);

  auto &rawBody = mPhysBody->getBody();
  b2Fixture *const fixture = rawBody.GetFixtureList();

  NOT_NULL(fixture);
  IS_NULL(fixture->GetNext());

  fixture->SetUserData(getUserData());
  setEntityPosition(rawBody);
}

void Archer::update(sf::Time dt) {
  if (mShootingCountdown > sf::Time::Zero) {
    mShootingCountdown -= dt;
  }

  getAnimationManager().update(dt);
  setEntityPosition(mPhysBody->getBody());
  spawnBullet();
}

OBJECT_TYPE Archer::getType() const { return mArcherType; }

void Archer::spawnBullet() {
  if (mShootingCountdown > sf::Time::Zero) {
    return;
  }

  mShootingCountdown = mShootingDelay;
  const auto heading = getHeading();
  const auto position = heading == HEADING::RIGHT
                            ? getPosition() + mBulletRightOffset
                            : getPosition() + mBulletLeftOffset;

  mBulletSpawnCallback(heading, mBullerType, position);
}

void Archer::onDraw(sf::RenderStates &states) const {
  states.transform.translate(0.f, mHiddenHeight);
}
