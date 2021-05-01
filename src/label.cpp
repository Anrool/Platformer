#define LOG_TAG "Label"

#include "label.h"
#include "componentDefinition.h"
#include "core.h"
#include "resourceManager.h"
#include "utils.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

Label::Label(const ComponentDefinition &definition)
    : mText(makeUnique<sf::Text>()) {
  setString(definition.mTextString, definition.mFontName);
  setFillColor(definition.mColor);
  setCharacteSize(definition.mCharacterSize);
  setPosition(definition.mPosition);
}

Label::~Label() {}

void Label::setString(const std::string &textString,
                      const std::string &fontName) {
  CHECK(!fontName.empty());

  mText->setFont(ResourceManager::getFont(fontName));
  setString(textString);
}

void Label::setString(const std::string &textString) {
  CHECK(!textString.empty());

  mText->setString(textString);
  centerOrigin(*mText);
}

void Label::setFillColor(const sf::Color &color) {
  CHECK(color != sf::Color::Transparent);

  mText->setFillColor(color);
}

void Label::setCharacteSize(unsigned int size) {
  CHECK(size > 0);

  mText->setCharacterSize(size);
  centerOrigin(*mText);
}

void Label::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(*mText, states);
}
