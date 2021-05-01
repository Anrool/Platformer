#define LOG_TAG "SettingsState"

#include "settingsState.h"
#include "button.h"
#include "buttonView.h"
#include "componentDefinition.h"
#include "core.h"
#include "inputManager.h"
#include "label.h"
#include "resourceManager.h"
#include "stateManager.h"
#include "stateType.h"
#include "utils.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

class SettingsState::LabelConfig : public sf::Drawable,
                                   private sf::NonCopyable {
public:
  explicit LabelConfig(sf::Keyboard::Key key, const sf::Vector2f &position)
      : mLabel() {
    ComponentDefinition labelDefinition;
    labelDefinition.mTextString = toString(key);
    labelDefinition.mColor = mConfigLabelColor;
    labelDefinition.mPosition = position;

    mLabel = makeUnique<Label>(labelDefinition);
    mKey = key;
  }

  void setKey(sf::Keyboard::Key key) {
    mKey = key;
    mLabel->setString(toString(key));
  }

  sf::Keyboard::Key getKey() const { return mKey; }

private:
  std::unique_ptr<Label> mLabel;
  sf::Keyboard::Key mKey;

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final {
    target.draw(*mLabel, states);
  }
};

const sf::Vector2f SettingsState::mFirstButtonPosition = {
    0.3f * WINDOW_WIDTH, 0.27f * WINDOW_HEIGHT};
const sf::Vector2f SettingsState::mFirstLabelPosition = {0.7f * WINDOW_WIDTH,
                                                         0.27f * WINDOW_HEIGHT};
const float SettingsState::mConfigVerticalGap = 50.f;
const sf::Color SettingsState::mConfigLabelColor = sf::Color::White;

const ComponentDefinition SettingsState::mHeaderLabelDefinition{
    "Settings",
    ComponentDefinition::mDefaultFontName,
    STATE_HEADER_COLOR,
    STATE_CHAR_SIZE,
    {0.5f * WINDOW_WIDTH, 0.1f * WINDOW_HEIGHT}};
const ComponentDefinition SettingsState::mActionLabelDefinition{
    "Action",
    ComponentDefinition::mDefaultFontName,
    sf::Color::Magenta,
    ComponentDefinition::mDefaultCharacterSize,
    {0.3f * WINDOW_WIDTH, 0.17f * WINDOW_HEIGHT}};
const ComponentDefinition SettingsState::mKeyLabelDefinition{
    "Key",
    ComponentDefinition::mDefaultFontName,
    sf::Color::Magenta,
    ComponentDefinition::mDefaultCharacterSize,
    {0.7f * WINDOW_WIDTH, 0.17f * WINDOW_HEIGHT}};

const ComponentDefinition SettingsState::mGoToMenuButtonDefinition{
    "Go to menu", {0.2f * WINDOW_WIDTH, 0.9f * WINDOW_HEIGHT}};
const ComponentDefinition SettingsState::mSetDefaultsButtonDefinition{
    "Set defaults", {0.5f * WINDOW_WIDTH, 0.9f * WINDOW_HEIGHT}};
const ComponentDefinition SettingsState::mSaveButtonDefinition{
    "Save", {0.8f * WINDOW_WIDTH, 0.9f * WINDOW_HEIGHT}};

SettingsState::SettingsState(StateManager &stateManager)
    : StateBase{stateManager}, mButtonKeyTypeMap(), mKeyTypeLabelMap(),
      mHeaderLabel(makeUnique<Label>(mHeaderLabelDefinition)),
      mActionLabel(makeUnique<Label>(mActionLabelDefinition)),
      mKeyLabel(makeUnique<Label>(mKeyLabelDefinition)),
      // mButtonView(makeUnique<ButtonView>()),
      mSprite(makeUnique<sf::Sprite>(
          ResourceManager::getTexture(STATE_TEXTURE_NAME))) {
  createButtonConfigs();

  /*auto goToMenuButton = makeUnique<Button>(mGoToMenuButtonDefinition);
  goToMenuButton->setOnClick([this] {
      transitToMenuState();
  });

  auto setDefaultsButton = makeUnique<Button>(mSetDefaultsButtonDefinition);
  setDefaultsButton->setOnClick([this] {
      setDefaultLabels();
  });

  auto saveButton = makeUnique<Button>(mSaveButtonDefinition);
  saveButton->setOnClick([this] {
      saveCurrentConfigs();
  });*/

  /*mButtonView->addButton(std::move(goToMenuButton));
  mButtonView->addButton(std::move(setDefaultsButton));
  mButtonView->addButton(std::move(saveButton));*/

  /*ButtonView::ButtonList buttons;
  buttons.emplace_back(std::move(mGoToMenuButtonDefinition), [this] {
      transitToMenuState();
  });
  buttons.emplace_back(std::move(mSetDefaultsButtonDefinition), [this] {
      setDefaultLabels();
  });
  buttons.emplace_back(std::move(mSaveButtonDefinition), [this] {
      saveCurrentConfigs();
  });
  mButtonView = makeUnique<ButtonView>(std::move(buttons));*/
}

SettingsState::~SettingsState() {}

void SettingsState::handleEvent(const sf::Event &event) {
  auto &selectedButton = mButtonView->getSelectedButton();

  if (event.type == sf::Event::KeyPressed &&
      event.key.code == sf::Keyboard::Escape) {
    if (selectedButton.isActivated()) {
      disableActiveButton();
    } else {
      transitToMenuState();
    }
  } else if (selectedButton.isActivated() &&
             event.type == sf::Event::KeyPressed &&
             event.key.code != sf::Keyboard::Return) {
    disableActiveButton();

    const auto newKeyType = findKeyTypeFor(&selectedButton);
    auto &buttonLabel = findLabelFor(newKeyType);
    const auto key = event.key.code;

    if (buttonLabel.getKey() == key) {
      return;
    }

    const auto it = std::find_if(
        mKeyTypeLabelMap.begin(), mKeyTypeLabelMap.end(),
        [key](const std::map<KEY_TYPE, std::unique_ptr<LabelConfig>>::value_type
                  &p) { return p.second->getKey() == key; });

    if (it != mKeyTypeLabelMap.cend()) {
      it->second->setKey(sf::Keyboard::Unknown);
    }

    buttonLabel.setKey(key);
  } else {
    mButtonView->handleEvent(event);
  }
}

void SettingsState::createButtonConfigs() {
  const auto &currentLayout = InputManager::getLayout(false);
  size_t i = 0;

  ButtonView::ButtonList buttons;

  for (const auto &layoutPair : currentLayout) {
    const auto buttonPos =
        mFirstButtonPosition + sf::Vector2f{0.f, i * mConfigVerticalGap};
    const auto labelPos =
        mFirstLabelPosition + sf::Vector2f{0.f, i * mConfigVerticalGap};

    const auto keyType = layoutPair.first;
    const auto key = layoutPair.second;

    const ComponentDefinition layoutButtonDefinition{toString(keyType),
                                                     buttonPos};

    // auto button = makeUnique<Button>(layoutButtonDefinition);
    buttons.emplace_back(layoutButtonDefinition);
    const Button *const button = &buttons.back();
    auto label = makeUnique<LabelConfig>(key, labelPos);

    CHECK(mButtonKeyTypeMap.emplace(button, keyType).second);
    CHECK(mKeyTypeLabelMap.emplace(keyType, std::move(label)).second);

    // mButtonView->addButton(std::move(button));
    i++;
  }

  buttons.emplace_back(std::move(mGoToMenuButtonDefinition),
                       [this] { transitToMenuState(); });
  buttons.emplace_back(std::move(mSetDefaultsButtonDefinition),
                       [this] { setDefaultLabels(); });
  buttons.emplace_back(std::move(mSaveButtonDefinition),
                       [this] { saveCurrentConfigs(); });
  mButtonView = makeUnique<ButtonView>(std::move(buttons));
}

void SettingsState::transitToMenuState() const {
  getStateManager().requestStateTranstion(STATE_TYPE::MENU);
}

void SettingsState::setDefaultLabels() {
  const auto &defaultLayout = InputManager::getLayout(true);

  for (const auto &layoutPair : defaultLayout) {
    auto &label = findLabelFor(layoutPair.first);
    label.setKey(layoutPair.second);
  }

  disableActiveButton();
}

void SettingsState::saveCurrentConfigs() {
  for (const auto &p : mKeyTypeLabelMap) {
    if (InputManager::getKey(p.first) != p.second->getKey()) {
      InputManager::setKey(p.first, p.second->getKey());
    }
  }

  InputManager::saveLayout();
  disableActiveButton();
}

void SettingsState::disableActiveButton() const {
  auto &selectedButton = mButtonView->getSelectedButton();

  CHECK(selectedButton.isActivated());

  selectedButton.deactivate();
}

SettingsState::LabelConfig &SettingsState::findLabelFor(KEY_TYPE type) const {
  CHECK(type != KEY_TYPE::NONE);

  const auto it = mKeyTypeLabelMap.find(type);

  CHECK(it != mKeyTypeLabelMap.cend());

  return *it->second;
}

KEY_TYPE SettingsState::findKeyTypeFor(const Button *const button) {
  NOT_NULL(button);

  const auto it = mButtonKeyTypeMap.find(button);

  CHECK(it != mButtonKeyTypeMap.cend());

  return it->second;
}

void SettingsState::draw(sf::RenderTarget &target,
                         sf::RenderStates states) const {
  target.draw(*mSprite, states);
  target.draw(*mHeaderLabel, states);
  target.draw(*mActionLabel, states);
  target.draw(*mKeyLabel, states);
  target.draw(*mButtonView, states);

  for (const auto &p : mKeyTypeLabelMap) {
    target.draw(*p.second, states);
  }
}
