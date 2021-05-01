#ifndef PLAYER_H
#define PLAYER_H

#include "shooter.h"

#include <SFML/System/Time.hpp>

class PhysicalBody;

struct PlayerCallback {
  virtual void onGround(bool onGround) = 0;
  virtual void onLadder(bool onLadder) = 0;

  virtual ~PlayerCallback() = default;
};

class Player : public Shooter, public PlayerCallback {
public:
  enum class COLLISION_SIDE { LEFT, RIGHT, NONE };

  explicit Player(std::unique_ptr<PhysicalBody> body);
  ~Player() final;

  // methods from Entity
  void handleRealtimeInput() final;
  void update(sf::Time dt) final;

  void damage(int hp) final;

  bool isDestroyed() const final;

  OBJECT_TYPE getType() const final;

  // methods from PlayerCallback
  void onGround(bool onGround) override;
  void onLadder(bool onLadder) override;

  void setParentBody(const b2Body *const parent);

  void onCollisionWithEnemy(COLLISION_SIDE side, int hp);

private:
  enum class STATE {
    STAND,
    WALK,
    JUMP,
    FALL,
    CLIMB,
    HURT,
    DEAD,
  };

  static const int mPlayerHitpoint;
  static const OBJECT_TYPE mPlayerType;
  static const OBJECT_TYPE mSensorType;
  static const ANIMATION_TYPE mInitAnimationType;
  static const sf::Vector2f mPlayerVelInMeters;
  static const sf::Vector2f mClimbVelocityInMeters;
  static const float mPushVelocityInMeters;

  static const sf::Time mJumpDelay;
  static const sf::Time mHurtDelay;
  static const sf::Time mShootingDelay;

  static const OBJECT_TYPE mBullerType;

  static const sf::Vector2f mBulletRightOffset;
  static const sf::Vector2f mBulletLeftOffset;

  static const STATE mInitState;

  STATE mState;
  std::unique_ptr<PhysicalBody> mPhysBody;

  bool mOnGround;
  UserData mSensorUserData;

  // a parent body the player is standing on
  const b2Body *mParentBody;

  sf::Time mJumpCountdown;

  COLLISION_SIDE mCollisionSide;
  sf::Time mHurtedCountdown;
  float mInitHalfHeight;

  bool mShooting;
  sf::Time mShootingCountdown;

  bool isHurted() const;

  void chooseAnimation();
  void spawnBullet();

  ANIMATION_TYPE stateToAnimationType() const;
  bool isLoopedAnimation(ANIMATION_TYPE type) const;

  void onDraw(sf::RenderStates &states) const final;
};

#endif // PLAYER_H
