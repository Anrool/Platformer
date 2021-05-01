#ifndef SETTINGSSTATE_H
#define SETTINGSSTATE_H

#include "stateBase.h"

#include <map>
#include <memory>

namespace sf {
class Color;
class Sprite;
} // namespace sf

class ComponentDefinition;
enum class KEY_TYPE;
class Label;
class Button;
class ButtonView;

class SettingsState : public StateBase {
public:
  explicit SettingsState(StateManager &stateManager);
  ~SettingsState() final;

  void handleEvent(const sf::Event &event) final;

private:
  class LabelConfig;

  static const sf::Vector2f mFirstButtonPosition;
  static const sf::Vector2f mFirstLabelPosition;
  static const float mConfigVerticalGap;
  static const sf::Color mConfigLabelColor;

  static const ComponentDefinition mHeaderLabelDefinition;
  static const ComponentDefinition mActionLabelDefinition;
  static const ComponentDefinition mKeyLabelDefinition;

  static const ComponentDefinition mGoToMenuButtonDefinition;
  static const ComponentDefinition mSetDefaultsButtonDefinition;
  static const ComponentDefinition mSaveButtonDefinition;

  std::map<const Button *, const KEY_TYPE> mButtonKeyTypeMap;
  std::map<KEY_TYPE, std::unique_ptr<LabelConfig>> mKeyTypeLabelMap;

  std::unique_ptr<const Label> mHeaderLabel;
  std::unique_ptr<const Label> mActionLabel;
  std::unique_ptr<const Label> mKeyLabel;

  std::unique_ptr<ButtonView> mButtonView;
  std::unique_ptr<const sf::Sprite> mSprite;

  void createButtonConfigs();

  void transitToMenuState() const;
  void setDefaultLabels();
  void saveCurrentConfigs();

  void disableActiveButton() const;

  LabelConfig &findLabelFor(KEY_TYPE type) const;
  KEY_TYPE findKeyTypeFor(const Button *const button);

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // SETTINGSSTATE_H
