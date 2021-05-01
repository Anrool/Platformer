#define LOG_TAG "Player"

#include "player.h"
#include "animationManager.h"
#include "animationType.h"
#include "core.h"
#include "inputManager.h"
#include "objectType.h"
#include "physicalBody.h"
#include "soundPlayer.h"
#include "utils.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <Box2D/Box2D.h>

const int Player::mPlayerHitpoint = 4;
const OBJECT_TYPE Player::mPlayerType = OBJECT_TYPE::PLAYER;
const OBJECT_TYPE Player::mSensorType = OBJECT_TYPE::PLAYER_SENSOR;
const ANIMATION_TYPE Player::mInitAnimationType = ANIMATION_TYPE::FALL;
const sf::Vector2f Player::mPlayerVelInMeters =
    toB2Coords(sf::Vector2f{250.f, -400.f});
const sf::Vector2f Player::mClimbVelocityInMeters =
    toB2Coords(sf::Vector2f{40.f, -2.f});
const float Player::mPushVelocityInMeters = toMeters(3000.f);

const sf::Time Player::mJumpDelay = sf::seconds(0.3f);
const sf::Time Player::mHurtDelay = sf::seconds(0.3f);
const sf::Time Player::mShootingDelay = sf::seconds(0.5f);

const OBJECT_TYPE Player::mBullerType = OBJECT_TYPE::ALLIED_BULLET;

const sf::Vector2f Player::mBulletRightOffset = {15.f, 6.f};
const sf::Vector2f Player::mBulletLeftOffset = {-6.f, 6.f};

const Player::STATE Player::mInitState = Player::STATE::FALL;

Player::Player(std::unique_ptr<PhysicalBody> body)
    : Shooter{mPlayerHitpoint, mPlayerType, mInitAnimationType, HEADING::RIGHT},
      mState(mInitState), mPhysBody(), mOnGround(false),
      mSensorUserData(mSensorType), mParentBody(nullptr),
      mJumpCountdown(sf::Time::Zero), mCollisionSide(COLLISION_SIDE::NONE),
      mHurtedCountdown(sf::Time::Zero), mInitHalfHeight(0.f), mShooting(false),
      mShootingCountdown(sf::Time::Zero) {
  NOT_NULL(body);
  CHECK(body->getType() == mPlayerType);

  mPhysBody = std::move(body);
  mPhysBody->createFootSensor();

  auto &rawBody = mPhysBody->getBody();
  b2Fixture *const sensorFixture = rawBody.GetFixtureList();

  NOT_NULL(sensorFixture);

  b2Fixture *const playerFixture = sensorFixture->GetNext();

  NOT_NULL(playerFixture);
  IS_NULL(playerFixture->GetNext());

  sensorFixture->SetUserData(&mSensorUserData);
  playerFixture->SetUserData(getUserData());
  setEntityPosition(rawBody);
  rawBody.SetBullet(true);

  mInitHalfHeight = mPhysBody->getBounds().height / 2.f;
}

Player::~Player() {}

void Player::handleRealtimeInput() {
  auto &rawBody = mPhysBody->getBody();
  const auto &curVelocity = rawBody.GetLinearVelocity();
  b2Vec2 newVelocity{0.f, 0.f};

  if (mState == STATE::DEAD) {
    centerOrigin();
  } else if (mState == STATE::HURT) {
    if (mHurtedCountdown <= sf::Time::Zero) {
      mHurtedCountdown = mHurtDelay;

      if (mCollisionSide != COLLISION_SIDE::NONE) {
        newVelocity.x += mCollisionSide == COLLISION_SIDE::RIGHT
                             ? -mPushVelocityInMeters
                             : +mPushVelocityInMeters;

        mCollisionSide = COLLISION_SIDE::NONE;
      }
    }
  } else if (mState == STATE::CLIMB) {
    newVelocity.y -= curVelocity.y;

    if (InputManager::isKeyPressed(KEY_TYPE::LEFT)) {
      newVelocity.x = -mClimbVelocityInMeters.x;
      setHeading(HEADING::LEFT);
    }

    if (InputManager::isKeyPressed(KEY_TYPE::RIGHT)) {
      newVelocity.x = +mClimbVelocityInMeters.x;
      setHeading(HEADING::RIGHT);
    }

    if (InputManager::isKeyPressed(KEY_TYPE::DOWN)) {
      newVelocity.y = -mClimbVelocityInMeters.y;
    }

    if (InputManager::isKeyPressed(KEY_TYPE::UP)) {
      newVelocity.y = +mClimbVelocityInMeters.y;
    }
  } else if (mState != STATE::FALL) {
    const auto heading = getHeading();

    if (InputManager::isKeyPressed(KEY_TYPE::LEFT)) {
      if (heading == HEADING::LEFT || mOnGround) {
        newVelocity.x = -mPlayerVelInMeters.x;
        setHeading(HEADING::LEFT);

        if (mState == STATE::STAND) {
          mState = STATE::WALK;
        }
      }
    }

    if (InputManager::isKeyPressed(KEY_TYPE::RIGHT)) {
      if (heading == HEADING::RIGHT || mOnGround) {
        newVelocity.x = +mPlayerVelInMeters.x;
        setHeading(HEADING::RIGHT);

        if (mState == STATE::STAND) {
          mState = STATE::WALK;
        }
      }
    }

    if (InputManager::isKeyPressed(KEY_TYPE::UP) && mOnGround &&
        mJumpCountdown <= sf::Time::Zero) {
      newVelocity.y = +mPlayerVelInMeters.y;
      mJumpCountdown = mJumpDelay;
    }

    mShooting = InputManager::isKeyPressed(KEY_TYPE::SHOOT);
  }

  // move along with the parent
  if (mParentBody != nullptr) {
    const auto &parentVel = mParentBody->GetLinearVelocity();
    newVelocity.x += parentVel.x;

    if (parentVel.y < 0.f && mJumpCountdown <= sf::Time::Zero) {
      newVelocity.y = parentVel.y;
    }
  }

  const auto mass = rawBody.GetMass();
  const b2Vec2 impulse = {mass * (newVelocity.x - curVelocity.x),
                          mass * newVelocity.y};

  rawBody.ApplyLinearImpulse(impulse, rawBody.GetWorldCenter(),
                             true /* wake */);
}

void Player::update(sf::Time dt) {
  if (mJumpCountdown > sf::Time::Zero) {
    mJumpCountdown -= dt;
  }

  if (mShootingCountdown > sf::Time::Zero) {
    mShootingCountdown -= dt;
  }

  if (mHurtedCountdown > sf::Time::Zero) {
    mHurtedCountdown -= dt;

    if (mHurtedCountdown <= sf::Time::Zero) {
      mState = STATE::FALL;
    }
  }

  if (!mOnGround && (mState == STATE::STAND || mState == STATE::WALK)) {
    if (mJumpCountdown > sf::Time::Zero) {
      mState = STATE::JUMP;
    } else {
      mState = STATE::FALL;
    }
  }

  chooseAnimation();

  if (mOnGround) {
    if (isKilled() && mHurtedCountdown <= sf::Time::Zero) {
      mState = STATE::DEAD;
    } else if (mState == STATE::WALK || mState == STATE::JUMP ||
               mState == STATE::FALL) {
      mState = STATE::STAND;
    }
  }

  getAnimationManager().update(dt);
  setEntityPosition(mPhysBody->getBody());
  spawnBullet();
}

void Player::damage(int hp) {
  if (isHurted()) {
    return;
  }

  Entity::damage(hp);

  mShooting = false;

  if (mOnGround && isKilled()) {
    mState = STATE::DEAD;
  } else {
    if (mState == STATE::CLIMB) {
      onLadder(false);
    }

    mState = STATE::HURT;
  }

  SoundPlayer::play("Hurt.wav", getPosition());
}

bool Player::isDestroyed() const {
  const auto &animationManager = getAnimationManager();
  return (mState == STATE::DEAD &&
          animationManager.getCurrentAnimationType() == ANIMATION_TYPE::DEAD &&
          !animationManager.isPlaying());
}

OBJECT_TYPE Player::getType() const { return mPlayerType; }

void Player::onGround(bool onGround) { mOnGround = onGround; }

void Player::onLadder(bool onLadder) {
  if (onLadder && !isHurted()) {
    auto &rawBody = mPhysBody->getBody();
    rawBody.SetLinearVelocity({0.f, 0.f});
    rawBody.SetGravityScale(0.f);
    mShooting = false;
    mState = STATE::CLIMB;
  } else if (mState == STATE::CLIMB) {
    auto &rawBody = mPhysBody->getBody();
    rawBody.SetGravityScale(1.f);
    mState = STATE::FALL;
  }
}

void Player::setParentBody(const b2Body *const parent) { mParentBody = parent; }

void Player::onCollisionWithEnemy(COLLISION_SIDE side, int hp) {
  CHECK(side != COLLISION_SIDE::NONE);
  CHECK(hp > 0);

  if (isHurted()) {
    return;
  }

  mCollisionSide = side;
  damage(hp);
}

bool Player::isHurted() const {
  return mState == STATE::HURT || mState == STATE::DEAD;
}

void Player::chooseAnimation() {
  auto &animationManager = getAnimationManager();
  auto newAnimation = animationManager.getCurrentAnimationType();

  if (mShooting) {
    newAnimation = ANIMATION_TYPE::SHOOT;
  } else {
    newAnimation = stateToAnimationType();
  }

  animationManager.chooseAnimation(newAnimation);
  animationManager.setLoop(isLoopedAnimation(newAnimation));
  animationManager.start();

  const auto &rawBody = mPhysBody->getBody();
  const auto isStanding = rawBody.GetLinearVelocity() == b2Vec2{0.f, 0.f};

  if (newAnimation == ANIMATION_TYPE::CLIMB && isStanding) {
    animationManager.pause();
  }
}

void Player::spawnBullet() {
  if (!mShooting || mShootingCountdown > sf::Time::Zero) {
    return;
  }

  mShootingCountdown = mShootingDelay;
  const auto heading = getHeading();
  const auto position = heading == HEADING::RIGHT
                            ? getPosition() + mBulletRightOffset
                            : getPosition() + mBulletLeftOffset;

  mBulletSpawnCallback(heading, mBullerType, position);
}

ANIMATION_TYPE Player::stateToAnimationType() const {
  switch (mState) {
  case STATE::STAND:
    return ANIMATION_TYPE::STAND;
  case STATE::WALK:
    return ANIMATION_TYPE::WALK;
  case STATE::JUMP:
    return ANIMATION_TYPE::JUMP;
  case STATE::HURT:
    return ANIMATION_TYPE::HURT;
  case STATE::FALL:
    return ANIMATION_TYPE::FALL;
  case STATE::CLIMB:
    return ANIMATION_TYPE::CLIMB;
  case STATE::DEAD:
    return ANIMATION_TYPE::DEAD;
  default:
    CHECK(false);
  }
}

bool Player::isLoopedAnimation(ANIMATION_TYPE type) const {
  return type != ANIMATION_TYPE::JUMP && type != ANIMATION_TYPE::HURT &&
         type != ANIMATION_TYPE::DEAD;
}

void Player::onDraw(sf::RenderStates &states) const {
  if (mState == STATE::DEAD) {
    const auto &frame = getAnimationManager().getCurrentFrame();
    const auto centerOffsetY =
        mInitHalfHeight - static_cast<float>(frame.height) / 2.f;
    states.transform.translate(0.f, centerOffsetY);
  }
}
