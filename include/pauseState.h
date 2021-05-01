#ifndef PAUSESTATE_H
#define PAUSESTATE_H

#include "stateBase.h"

#include <memory>

class ComponentDefinition;
class Label;
class ButtonView;

namespace sf {
class Color;
class RectangleShape;
class Sprite;
} // namespace sf

class PauseState : public StateBase {
public:
  explicit PauseState(StateManager &stateManager);
  ~PauseState() final;

  void handleEvent(const sf::Event &event) final;

private:
  static const sf::Color mBackgroundColor;

  static const ComponentDefinition mHeaderLabelDefinition;

  static const ComponentDefinition mContinueButtonDefinition;
  static const ComponentDefinition mGoToMenuButtonDefinition;

  std::unique_ptr<Label> mHeaderLabel;
  std::unique_ptr<ButtonView> mButtonView;

  std::unique_ptr<sf::Texture> mBackgroundTexture;
  std::unique_ptr<sf::Sprite> mBackgroundSprite;
  std::unique_ptr<sf::RectangleShape> mBackgroundRectangle;

  void resumeGame();

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // PAUSESTATE_H
