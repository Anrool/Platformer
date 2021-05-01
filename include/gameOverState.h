#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H

#include "stateBase.h"

#include <SFML/Graphics/Color.hpp>

#include <memory>

class ComponentDefinition;
class Label;
class ButtonView;
class AnimatedEntity;

namespace sf {
class Sprite;
}

class GameOverState : public StateBase {
public:
  explicit GameOverState(StateManager &stateManager);
  ~GameOverState() final;

  void handleEvent(const sf::Event &event) final;
  void update(sf::Time dt) final;

private:
  static const std::string mSuccessTextureName;
  static const sf::Vector2f mEntityInitPositionSuccess;

  static const std::string mFailureTextureName;
  static const sf::Vector2f mEntityInitPositionFailure;

  static const ComponentDefinition mHeaderLabelSuccessDefinition;
  static const ComponentDefinition mHeaderLabelFailureDefinition;

  static const ComponentDefinition mTryAgainButtonDefinition;
  static const ComponentDefinition mGoToMenuButtonDefinition;

  std::unique_ptr<Label> mHeaderLabel;
  std::unique_ptr<ButtonView> mButtonView;
  std::unique_ptr<sf::Sprite> mSprite;

  std::unique_ptr<AnimatedEntity> mAnimatedEntity;
  bool mAnimationInProgress;

  void onFinishHandler();

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // GAMEOVERSTATE_H
