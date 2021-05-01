#define LOG_TAG "ComponentDefinition"

#include "componentDefinition.h"
#include "core.h"

const std::string ComponentDefinition::mDefaultFontName =
    "KaushanScript-Regular.otf";
const sf::Color ComponentDefinition::mDefaultColor = sf::Color::Black;
const unsigned int ComponentDefinition::mDefaultCharacterSize = 30u;

// default string will cause label contruction failure
const std::string ComponentDefinition::mDefaultString = "";
const sf::Vector2f ComponentDefinition::mDefaultPosition{};
