#ifndef ENTITY_H
#define ENTITY_H

#include "userData.h"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <memory>

enum class ANIMATION_TYPE;
enum class HEADING;
class b2Body;
class AnimationManager;

namespace sf {
class Time;
}

class Entity : public sf::Drawable,
               public sf::Transformable,
               private sf::NonCopyable {
public:
  explicit Entity(int maxHitpoints, OBJECT_TYPE objectType,
                  ANIMATION_TYPE animationType, HEADING heading);
  ~Entity() override;

  virtual void handleRealtimeInput();
  virtual void update(sf::Time dt) = 0;

  virtual void damage(int hp);
  virtual void repair(int hp);
  int getHitpoints() const;

  void kill();
  bool isKilled() const;

  virtual bool isDestroyed() const;

  void setVelocity(const sf::Vector2f &velocity);
  void setVelocityX(float velocityX);
  void setVelocityY(float velocityY);

  const sf::Vector2f &getVelocity() const;
  float getVelocityX() const;
  float getVelocityY() const;

  sf::FloatRect getBoundingRect() const;

  virtual OBJECT_TYPE getType() const = 0;

  UserData *getUserData();

  void setHeading(HEADING heading);
  HEADING getHeading() const;
  void changeHeading();

protected:
  void initAnimationManager(OBJECT_TYPE objectType,
                            ANIMATION_TYPE animationType, HEADING heading);

  AnimationManager &getAnimationManager() const;

  void centerOrigin();
  void setEntityPosition(const b2Body &body);

  virtual void onDraw(sf::RenderStates &states) const;

private:
  int mMaxHitpoints;
  int mHitpoints;
  sf::Vector2f mVelocity;
  UserData mUserData;

  std::unique_ptr<AnimationManager> mAnimationManager;

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

  void drawBoundingRect(sf::RenderTarget &target,
                        sf::RenderStates states) const;
};

#endif // ENTITY_H
