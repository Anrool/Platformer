#ifndef COMPONENTDEFINITION_H
#define COMPONENTDEFINITION_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include <string>

struct ComponentDefinition {
  std::string mTextString;
  std::string mFontName;
  sf::Color mColor;
  unsigned int mCharacterSize;
  sf::Vector2f mPosition;

  ComponentDefinition()
      : ComponentDefinition{mDefaultString, mDefaultFontName, mDefaultColor,
                            mDefaultCharacterSize, mDefaultPosition} {}

  explicit ComponentDefinition(const std::string &textString,
                               const sf::Vector2f &position)
      : ComponentDefinition{textString, mDefaultFontName, mDefaultColor,
                            mDefaultCharacterSize, position} {}

  explicit ComponentDefinition(const std::string &textString,
                               const std::string &fontName,
                               const sf::Color &color,
                               unsigned int characterSize,
                               const sf::Vector2f &position)
      : mTextString(textString), mFontName(fontName), mColor(color),
        mCharacterSize(characterSize), mPosition(position) {}

  static const std::string mDefaultFontName;
  static const sf::Color mDefaultColor;
  static const unsigned int mDefaultCharacterSize;

private:
  static const std::string mDefaultString;
  static const sf::Vector2f mDefaultPosition;
};

#endif // COMPONENTDEFINITION_H
