#define LOG_TAG "Entity"

#include "entity.h"
#include "animationManager.h"
#include "animationParser.h"
#include "animationType.h"
#include "core.h"
#include "objectType.h"
#include "utils.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/b2Body.h>

Entity::Entity(int maxHitpoints, OBJECT_TYPE objectType,
               ANIMATION_TYPE animationType, HEADING heading)
    : mMaxHitpoints(0), mHitpoints(0), mUserData(this), mAnimationManager() {
  CHECK(maxHitpoints > 0);

  mHitpoints = mMaxHitpoints = maxHitpoints;

  initAnimationManager(objectType, animationType, heading);
}

Entity::~Entity() {}

void Entity::handleRealtimeInput() {}

void Entity::damage(int hp) {
  CHECK(!isKilled());
  CHECK(hp > 0);

  mHitpoints = mHitpoints > hp ? mHitpoints - hp : 0;
}

void Entity::repair(int hp) {
  CHECK(!isKilled());
  CHECK(hp > 0);
  CHECK(mHitpoints + hp <= mMaxHitpoints);

  mHitpoints += hp;
}

int Entity::getHitpoints() const { return mHitpoints; }

void Entity::kill() {
  CHECK(!isKilled());

  damage(mHitpoints);
}

bool Entity::isKilled() const { return getHitpoints() <= 0; }

bool Entity::isDestroyed() const { return isKilled(); }

void Entity::setVelocity(const sf::Vector2f &velocity) { mVelocity = velocity; }

void Entity::setVelocityX(float velocityX) {
  setVelocity({velocityX, mVelocity.y});
}

void Entity::setVelocityY(float velocityY) {
  setVelocity({mVelocity.x, velocityY});
}

const sf::Vector2f &Entity::getVelocity() const { return mVelocity; }

float Entity::getVelocityX() const { return getVelocity().x; }

float Entity::getVelocityY() const { return getVelocity().y; }

sf::FloatRect Entity::getBoundingRect() const {
  const auto &frame = getAnimationManager().getCurrentFrame();
  const sf::Vector2f pos = {0.f, 0.f};
  const sf::Vector2f size = {static_cast<float>(frame.width),
                             static_cast<float>(frame.height)};

  return {pos, size};
}

UserData *Entity::getUserData() { return &mUserData; }

void Entity::setHeading(HEADING heading) {
  CHECK(heading != HEADING::NONE);

  getAnimationManager().setHeading(heading);
}

void Entity::changeHeading() { getAnimationManager().changeHeading(); }

HEADING Entity::getHeading() const {
  return getAnimationManager().getHeading();
}

void Entity::initAnimationManager(OBJECT_TYPE objectType,
                                  ANIMATION_TYPE animationType,
                                  HEADING heading) {
  CHECK(objectType != OBJECT_TYPE::NONE);
  CHECK(animationType != ANIMATION_TYPE::NONE);
  CHECK(heading != HEADING::NONE);

  mAnimationManager = AnimationParser::createManagerFor(objectType);
  mAnimationManager->chooseAnimation(animationType, heading);
  mAnimationManager->start();

  centerOrigin();
}

AnimationManager &Entity::getAnimationManager() const {
  NOT_NULL(mAnimationManager);

  return *mAnimationManager;
}

void Entity::centerOrigin() {
  const auto bounds = getBoundingRect();
  setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
}

void Entity::setEntityPosition(const b2Body &body) {
  const auto posInPixels = toSFMLCoords(body.GetPosition());
  setPosition({posInPixels.x, posInPixels.y});
}

void Entity::onDraw(sf::RenderStates & /*states*/) const {}

void Entity::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  states.transform *= getTransform();

  onDraw(states);
  target.draw(getAnimationManager(), states);

  // drawBoundingRect(target, states);
}

void Entity::drawBoundingRect(sf::RenderTarget &target,
                              sf::RenderStates states) const {
  const auto bounds = getBoundingRect();

  sf::RectangleShape rect;
  rect.setSize({bounds.width, bounds.height});
  rect.setFillColor(sf::Color::Transparent);
  rect.setOutlineThickness(1.f);
  rect.setOutlineColor(sf::Color::Red);

  target.draw(rect, states);
}
