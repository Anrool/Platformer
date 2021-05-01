#define LOG_TAG "Bullet"

#include "bullet.h"
#include "animationManager.h"
#include "animationType.h"
#include "core.h"
#include "objectType.h"
#include "physicalBody.h"
#include "soundPlayer.h"
#include "utils.h"

#include <Box2D/Dynamics/b2Fixture.h>

const int Bullet::mBulletHitpoints = 1;
const float Bullet::mBulletVelocity = 400.f;

Bullet::Bullet(HEADING heading, std::unique_ptr<PhysicalBody> body)
    : Entity{mBulletHitpoints, body->getType(),
             objToAnimationType(body->getType()), heading},
      mType(OBJECT_TYPE::NONE), mPhysBody(), mExplodeCountdown(sf::Time::Zero) {
  NOT_NULL(body);

  const auto type = body->getType();

  CHECK(type == OBJECT_TYPE::ALLIED_BULLET ||
        type == OBJECT_TYPE::ENEMY_BULLET);

  mType = type;
  mPhysBody = std::move(body);

  auto &rawBody = mPhysBody->getBody();
  b2Fixture *const fixture = rawBody.GetFixtureList();

  NOT_NULL(fixture);
  IS_NULL(fixture->GetNext());

  fixture->SetUserData(getUserData());
  setEntityPosition(rawBody);

  const auto velocityInMeters = toB2Coords(b2Vec2{
      heading == HEADING::RIGHT ? mBulletVelocity : -mBulletVelocity, 0.f});
  rawBody.SetLinearVelocity(velocityInMeters);
  rawBody.SetBullet(true);
  rawBody.SetGravityScale(0.f);
}

void Bullet::update(sf::Time dt) {
  if (isKilled()) {
    if (mExplodeCountdown > sf::Time::Zero) {
      mExplodeCountdown -= dt;
    } else if (getType() != OBJECT_TYPE::EXPLODED_BULLET) {
      mType = OBJECT_TYPE::EXPLODED_BULLET;

      initAnimationManager(mType, objToAnimationType(mType), getHeading());

      auto &newAnimationManager = getAnimationManager();
      newAnimationManager.setLoop(false);

      mExplodeCountdown = newAnimationManager.getDuration();

      auto &rawBody = mPhysBody->getBody();
      rawBody.SetLinearVelocity({0.f, 0.f});
      rawBody.SetActive(false);

      SoundPlayer::play("Explosion.wav", getPosition());
    } else {
      // destroyed bullet can't be updated
      CHECK(false);
    }

    centerOrigin();
  }

  getAnimationManager().update(dt);
  setEntityPosition(mPhysBody->getBody());
}

bool Bullet::isDestroyed() const {
  return mType == OBJECT_TYPE::EXPLODED_BULLET &&
         mExplodeCountdown <= sf::Time::Zero;
}

OBJECT_TYPE Bullet::getType() const { return mType; }

ANIMATION_TYPE Bullet::objToAnimationType(OBJECT_TYPE type) const {
  switch (type) {
  case OBJECT_TYPE::ALLIED_BULLET:
    return ANIMATION_TYPE::ALLIED_BULLET;
  case OBJECT_TYPE::ENEMY_BULLET:
    return ANIMATION_TYPE::ENEMY_BULLET;
  case OBJECT_TYPE::EXPLODED_BULLET:
    return ANIMATION_TYPE::EXPLODED_BULLET;
  default:
    CHECK(false);
  }
}
