#ifndef INTROSTATE_H
#define INTROSTATE_H

#include "stateBase.h"

#include <SFML/System/Time.hpp>

#include <memory>

class ComponentDefinition;
class Label;
class AnimatedEntity;

namespace sf {
class Sprite;
}

class IntroState : public StateBase {
public:
  explicit IntroState(StateManager &stateManager);
  ~IntroState() final;

  void handleEvent(const sf::Event &event) final;
  void update(sf::Time dt) final;

private:
  static const ComponentDefinition mHeaderDefinition;

  static const ComponentDefinition mInfoDefinition;
  static const sf::Time mInfoDelay;

  static const sf::Vector2f mEntityInitPosition;
  static const sf::Vector2f mEntityScale;
  static const std::string mRollingTextureName;
  static const std::string mRavenSound;

  std::unique_ptr<Label> mHeaderLabel;

  std::unique_ptr<Label> mInfoLabel;
  sf::Time mInfoCountdown;
  bool mShowInfo;

  std::unique_ptr<sf::Sprite> mSprite;

  std::unique_ptr<AnimatedEntity> mAnimatedEntity;
  std::unique_ptr<sf::Sprite> mRollingSprite;
  bool mAnimationInProgress;

  void onFinishHandler();

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // INTROSTATE_H
