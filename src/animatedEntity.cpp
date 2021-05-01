#define LOG_TAG "AnimatedEntity"

#include "animatedEntity.h"
#include "animationManager.h"
#include "animationType.h"
#include "core.h"
#include "objectType.h"
#include "utils.h"

#include <cmath>

struct AnimatedEntity::Movement {
  float mAngle;
  float mLength;
  sf::Time mDelay;
  float mVelocity;
  ANIMATION_TYPE mAnimationType;
  bool mLoop;

  Movement(float angle, float length, sf::Time delay, float velocity,
           ANIMATION_TYPE animationType, bool loop = true)
      : mAngle(angle), mLength(length), mDelay(delay), mVelocity(velocity),
        mAnimationType(animationType), mLoop(loop) {}
};

const std::map<AnimatedEntity::TYPE, AnimatedEntity::MovementVector>
    AnimatedEntity::mMovementMap = {
        {FAILURE,
         {
             {180.f, 25.f, sf::Time::Zero, 84.f, ANIMATION_TYPE::HURT},
             {90.f, 12.f, sf::Time::Zero, 15.f, ANIMATION_TYPE::DEAD, false},
         }},
        {SUCCESS,
         {
             {90.f, 400.f, sf::Time::Zero, 320.f, ANIMATION_TYPE::FALL},
             {0.f, 200.f, sf::Time::Zero, 150.f, ANIMATION_TYPE::WALK},
             {-45.f, 100.f, sf::Time::Zero, 250.f, ANIMATION_TYPE::JUMP},
             {+60.f, 58.f, sf::Time::Zero, 170.f, ANIMATION_TYPE::JUMP},
             {-90.f, 220.f, sf::Time::Zero, 120.f, ANIMATION_TYPE::SPIN},
         }},
        {
            INTRO,
            {
                {0.f, 0.f, sf::seconds(0.75f), 0.f,
                 ANIMATION_TYPE::RAVEN_SPEAK},
                {0.f, 0.f, sf::seconds(1.08f), 0.f, ANIMATION_TYPE::RAVEN_LOOK},
                {0.f, 10.f, sf::Time::Zero, 180.f, ANIMATION_TYPE::RAVEN_TURN},
                {0.f, 630.f, sf::Time::Zero, 350.f, ANIMATION_TYPE::RAVEN_FLY},
            },
        },
};

AnimatedEntity::AnimatedEntity(TYPE type, OnFinishCallback onFinishCallback)
    : Entity{1, toObjectType(type), findMovements(type).front().mAnimationType,
             HEADING::RIGHT},
      mObjectType(OBJECT_TYPE::NONE), mType(TYPE::NONE), mOnFinishCallback(),
      mMovements(findMovements(type)), mCurrentMovement(mMovements.cbegin()),
      mDistancePassed(0.f), mMovementCountdown(mCurrentMovement->mDelay) {
  CHECK(type != NONE);
  NOT_NULL(onFinishCallback);

  mObjectType = toObjectType(type);
  mType = type;
  mOnFinishCallback = onFinishCallback;

  setVelocityFromMovement();
  getAnimationManager().setLoop(mCurrentMovement->mLoop);
}

void AnimatedEntity::update(sf::Time dt) {
  auto &animatinManager = getAnimationManager();

  // check if current movement completed
  if (mDistancePassed >= mCurrentMovement->mLength) {
    const auto next = mCurrentMovement + 1;

    // delay after movement
    if (mMovementCountdown > sf::Time::Zero) {
      mMovementCountdown -= dt;
      setVelocity({0.f, 0.f});
    } else if (next < mMovements.cend()) {
      mCurrentMovement = next;
      mDistancePassed = 0.f;
      mMovementCountdown = mCurrentMovement->mDelay;

      setVelocityFromMovement();

      animatinManager.chooseAnimation(mCurrentMovement->mAnimationType);
      animatinManager.setLoop(mCurrentMovement->mLoop);
      animatinManager.start();
    } else {
      setVelocity({0.f, 0.f});
      mOnFinishCallback();
    }
  }

  mDistancePassed += mCurrentMovement->mVelocity * dt.asSeconds();
  move(getVelocity() * dt.asSeconds());
  animatinManager.update(dt);
}

OBJECT_TYPE AnimatedEntity::getType() const { return mObjectType; }

const AnimatedEntity::MovementVector &AnimatedEntity::findMovements(TYPE type) {
  CHECK(type != NONE);

  const auto it = mMovementMap.find(type);

  CHECK(it != mMovementMap.cend());

  return it->second;
}

OBJECT_TYPE AnimatedEntity::toObjectType(AnimatedEntity::TYPE type) {
  CHECK(type != AnimatedEntity::NONE);

  if (type == AnimatedEntity::INTRO) {
    return OBJECT_TYPE::RAVEN;
  }

  return OBJECT_TYPE::PLAYER;
}

void AnimatedEntity::setVelocityFromMovement() {
  CHECK(mCurrentMovement != mMovements.cend());

  const float velocity = mCurrentMovement->mVelocity;
  const float rads = toRadian(mCurrentMovement->mAngle);
  const float vx = velocity * std::cos(rads);
  const float vy = velocity * std::sin(rads);

  setVelocity({vx, vy});
}
