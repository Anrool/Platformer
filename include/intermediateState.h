#ifndef INTERMEDIATESTATE_H
#define INTERMEDIATESTATE_H

#include "stateBase.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Time.hpp>

#include <memory>

class ComponentDefinition;
class Label;

namespace sf {
class Sprite;
}

class IntermediateState : public StateBase {
public:
  explicit IntermediateState(StateManager &stateManager);
  ~IntermediateState() final;

  void handleEvent(const sf::Event &event) final;
  void update(sf::Time dt) final;

private:
  static const sf::Color mBackgroundColor;

  static const ComponentDefinition mHeaderLabelDefinition;

  static const ComponentDefinition mInfoLabelDefinition;
  static const sf::Time mInfoDelay;

  std::unique_ptr<Label> mHeaderLabel;

  std::unique_ptr<Label> mInfoLabel;
  sf::Time mInfoCountdown;
  bool mShowInfo;

  std::unique_ptr<sf::Texture> mBackgroundTexture;
  std::unique_ptr<sf::Sprite> mBackgroundSprite;
  std::unique_ptr<sf::RectangleShape> mBackgroundRectangle;

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // INTERMEDIATESTATE_H
