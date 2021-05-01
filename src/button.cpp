#define LOG_TAG "Button"

#include "button.h"
#include "componentDefinition.h"
#include "core.h"
#include "label.h"
#include "resourceManager.h"
#include "soundPlayer.h"
#include "utils.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

const std::map<Button::BUTTON_TYPE, sf::IntRect> Button::mBoundsMap = {
    {BUTTON_TYPE::REGULAR, {10, 70, 150, 46}},
    {BUTTON_TYPE::SELECTED, {10, 130, 150, 46}},
    {BUTTON_TYPE::ACTIVATED, {10, 250, 150, 46}},
};
const std::string Button::mTextureName = "Button.png";
const std::string Button::mActivationSound = "ButtonPush.wav";

Button::Button(const ComponentDefinition &buttonDefinition)
    : mType(BUTTON_TYPE::NONE), mLabel(),
      mSprite(
          makeUnique<sf::Sprite>(ResourceManager::getTexture(mTextureName))),
      mOnClick() {
  changeButtonType(BUTTON_TYPE::REGULAR);
  centerButtonOrigin();
  setPosition(buttonDefinition.mPosition);

  auto labelDefinition = buttonDefinition;
  labelDefinition.mPosition = getOrigin();

  mLabel = makeUnique<Label>(labelDefinition);
}
Button::Button(const ComponentDefinition &buttonDefinition,
               const OnClick &onClick)
    : Button{buttonDefinition} {
  NOT_NULL(onClick);

  mOnClick = onClick;
}

Button::~Button() {}

void Button::setString(const std::string &textString,
                       const std::string &fontName) {
  mLabel->setString(textString, fontName);
}

void Button::setString(const std::string &textString) {
  mLabel->setString(textString);
}

void Button::setFillColor(const sf::Color &color) {
  mLabel->setFillColor(color);
}

void Button::setCharacteSize(unsigned int size) {
  mLabel->setCharacteSize(size);
}

void Button::select() {
  CHECK(!isSelected());

  changeButtonType(BUTTON_TYPE::SELECTED);
}

void Button::deselect() {
  CHECK(isSelected() || isActivated());

  changeButtonType(BUTTON_TYPE::REGULAR);
}

bool Button::isSelected() const { return mType == BUTTON_TYPE::SELECTED; }

void Button::activate() {
  CHECK(isSelected());

  changeButtonType(BUTTON_TYPE::ACTIVATED);

  if (mOnClick != nullptr) {
    mOnClick();
  }

  SoundPlayer::play(mActivationSound);
}

void Button::deactivate() {
  CHECK(isActivated());

  changeButtonType(BUTTON_TYPE::SELECTED);
}

bool Button::isActivated() const { return mType == BUTTON_TYPE::ACTIVATED; }

sf::FloatRect Button::getBoundingRect() const {
  return getTransform().transformRect(mSprite->getLocalBounds());
}

void Button::centerButtonOrigin() {
  const auto bounds = getBoundingRect();
  const sf::Vector2f newOrigin = {bounds.width / 2.f, bounds.height / 2.f};

  setOrigin(newOrigin);
}

void Button::changeButtonType(BUTTON_TYPE type) {
  CHECK(type != BUTTON_TYPE::NONE);

  const auto it = mBoundsMap.find(type);

  CHECK(it != mBoundsMap.cend());

  mSprite->setTextureRect(it->second);
  mType = type;
}

void Button::drawBoundingRect(sf::RenderTarget &target,
                              sf::RenderStates states) const {
  const auto bounds = getBoundingRect();

  sf::RectangleShape rect;
  rect.setSize({bounds.width, bounds.height});
  rect.setFillColor(sf::Color::Transparent);
  rect.setOutlineThickness(1.f);
  rect.setOutlineColor(sf::Color::Red);

  target.draw(rect, states);
}

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  states.transform *= getTransform();

  target.draw(*mSprite, states);
  target.draw(*mLabel, states);

  // drawBoundingRect(target, states);
}
