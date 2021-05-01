#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "stateBase.h"

#include <memory>

class ComponentDefinition;
class Label;
class ButtonView;

namespace sf {
class Sprite;
}

class MenuState : public StateBase {
public:
  explicit MenuState(StateManager &stateManager);
  ~MenuState() final;

  void handleEvent(const sf::Event &event) final;

private:
  static const ComponentDefinition mHeaderLabelDefinition;

  static const ComponentDefinition mPlayButtonDefinition;
  static const ComponentDefinition mSettingsButtonDefinition;
  static const ComponentDefinition mExitButtonDefinition;

  std::unique_ptr<Label> mHeaderLabel;
  std::unique_ptr<ButtonView> mButtonView;
  std::unique_ptr<sf::Sprite> mSprite;

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // MENUSTATE_H
