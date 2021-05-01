#define LOG_TAG "AnimationManager"

#include "animationManager.h"
#include "animation.h"
#include "animationType.h"
#include "core.h"
#include "resourceManager.h"
#include "utils.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

AnimationManager::AnimationManager(const std::string &textureName)
    : mAnimations(), mCurrentAnimation(mAnimations.cend()),
      mHeading(HEADING::NONE), mSprite() {
  CHECK(!textureName.empty());

  mSprite = makeUnique<sf::Sprite>(ResourceManager::getTexture(textureName));
}

AnimationManager::~AnimationManager() {}

void AnimationManager::addAnimation(ANIMATION_TYPE type,
                                    std::unique_ptr<Animation> animation) {
  CHECK(type != ANIMATION_TYPE::NONE);
  NOT_NULL(animation);
  CHECK(mAnimations.emplace(type, std::move(animation)).second);
}

void AnimationManager::removeAnimation(ANIMATION_TYPE type) {
  CHECK(type != ANIMATION_TYPE::NONE);

  const auto it = mAnimations.find(type);

  CHECK(it != mAnimations.cend());

  if (it == mCurrentAnimation) {
    mCurrentAnimation->second->stop();
    mCurrentAnimation = mAnimations.cend();
  }

  mAnimations.erase(type);
}

size_t AnimationManager::getAnimationCount() const {
  return mAnimations.size();
}

void AnimationManager::chooseAnimation(ANIMATION_TYPE type, HEADING heading) {
  CHECK(type != ANIMATION_TYPE::NONE);

  const auto it = mAnimations.find(type);

  CHECK(it != mAnimations.end());

  // don't stop already chosen animation
  if (hasChosenAnimation() && it != mCurrentAnimation) {
    mCurrentAnimation->second->stop();
  }

  // heading should be passed here or be set before
  if (heading == HEADING::NONE) {
    CHECK(mHeading != HEADING::NONE);
  } else {
    mHeading = heading;
  }

  mCurrentAnimation = it;
}

bool AnimationManager::hasAnimationFor(ANIMATION_TYPE type) const {
  return mAnimations.find(type) != mAnimations.cend();
}

ANIMATION_TYPE AnimationManager::getCurrentAnimationType() const {
  CHECK(hasChosenAnimation());

  return mCurrentAnimation->first;
}

void AnimationManager::setHeading(HEADING heading) {
  CHECK(heading != HEADING::NONE);

  mHeading = heading;
}

HEADING AnimationManager::getHeading() const { return mHeading; }

void AnimationManager::changeHeading() {
  CHECK(mHeading != HEADING::NONE);

  const auto newHeading =
      getHeading() == HEADING::RIGHT ? HEADING::LEFT : HEADING::RIGHT;

  mHeading = newHeading;
}

const sf::IntRect &AnimationManager::getCurrentFrame() const {
  CHECK(hasChosenAnimation());

  return mCurrentAnimation->second->getCurrentFrame();
}

sf::Time AnimationManager::getDuration() const {
  CHECK(hasChosenAnimation());

  return mCurrentAnimation->second->getDuration();
}

void AnimationManager::setLoop(bool loop) const {
  CHECK(hasChosenAnimation());

  mCurrentAnimation->second->setLoop(loop);
}

bool AnimationManager::isLooped() const {
  CHECK(hasChosenAnimation());

  return mCurrentAnimation->second->isLooped();
}

void AnimationManager::start() const {
  CHECK(hasChosenAnimation());

  mCurrentAnimation->second->start();
}

void AnimationManager::pause() const {
  CHECK(hasChosenAnimation());

  mCurrentAnimation->second->pause();
}

void AnimationManager::stop() const {
  CHECK(hasChosenAnimation());

  mCurrentAnimation->second->stop();
}

bool AnimationManager::isPlaying() const {
  CHECK(hasChosenAnimation());

  return mCurrentAnimation->second->isPlaying();
}

void AnimationManager::update(sf::Time dt) const {
  CHECK(hasChosenAnimation());

  mCurrentAnimation->second->update(dt);
}

void AnimationManager::draw(sf::RenderTarget &target,
                            sf::RenderStates states) const {
  CHECK(hasChosenAnimation());

  auto frame = getCurrentFrame();

  if (getHeading() == HEADING::LEFT) {
    frame.left += frame.width;
    frame.width *= -1;
  }

  mSprite->setTextureRect(frame);
  target.draw(*mSprite, states);
}

bool AnimationManager::hasChosenAnimation() const {
  return mCurrentAnimation != mAnimations.cend() && mHeading != HEADING::NONE;
}
