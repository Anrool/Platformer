#define LOG_TAG "IntermediateState"

#include "intermediateState.h"
#include "componentDefinition.h"
#include "core.h"
#include "label.h"
#include "resourceManager.h"
#include "stateManager.h"
#include "stateType.h"
#include "utils.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>

const sf::Color IntermediateState::mBackgroundColor = {0, 0, 0, 150};

const ComponentDefinition IntermediateState::mHeaderLabelDefinition{
    "",
    ComponentDefinition::mDefaultFontName,
    STATE_HEADER_COLOR,
    STATE_CHAR_SIZE,
    {0.5f * WINDOW_WIDTH, 0.5f * WINDOW_HEIGHT}};

const ComponentDefinition IntermediateState::mInfoLabelDefinition{
    "Press \"Enter\" to start a next level",
    "Blackjack.otf",
    {0, 128, 128},
    ComponentDefinition::mDefaultCharacterSize,
    {0.5f * WINDOW_WIDTH, 0.68f * WINDOW_HEIGHT}};
const sf::Time IntermediateState::mInfoDelay = sf::seconds(0.5f);

IntermediateState::IntermediateState(StateManager &stateManager)
    : StateBase{stateManager}, mHeaderLabel(),
      mInfoLabel(makeUnique<Label>(mInfoLabelDefinition)),
      mInfoCountdown(mInfoDelay), mShowInfo(true),
      mBackgroundTexture(makeUnique<sf::Texture>()),
      mBackgroundSprite(makeUnique<sf::Sprite>()),
      mBackgroundRectangle(makeUnique<sf::RectangleShape>(
          sf::Vector2f{static_cast<float>(WINDOW_WIDTH),
                       static_cast<float>(WINDOW_HEIGHT)})) {
  ComponentDefinition headerDefinition = mHeaderLabelDefinition;
  headerDefinition.mTextString =
      "Level " + std::to_string(stateManager.getCurrentLevel()) + " completed!";
  mHeaderLabel = makeUnique<Label>(headerDefinition);

  CHECK(mBackgroundTexture->create(WINDOW_WIDTH, WINDOW_HEIGHT));

  mBackgroundTexture->update(ResourceManager::getWindow());
  mBackgroundSprite->setTexture(*mBackgroundTexture);
  mBackgroundRectangle->setFillColor(mBackgroundColor);
}

IntermediateState::~IntermediateState() {}

void IntermediateState::handleEvent(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed &&
      event.key.code == sf::Keyboard::Return) {
    getStateManager().requestStateTranstion(STATE_TYPE::GAME);
  }
}

void IntermediateState::update(sf::Time dt) {
  mInfoCountdown -= dt;

  if (mInfoCountdown <= sf::Time::Zero) {
    mInfoCountdown = mInfoDelay;
    mShowInfo = !mShowInfo;
  }
}

void IntermediateState::draw(sf::RenderTarget &target,
                             sf::RenderStates states) const {
  target.draw(*mBackgroundSprite, states);
  target.draw(*mBackgroundRectangle, states);
  target.draw(*mHeaderLabel, states);

  if (mShowInfo) {
    target.draw(*mInfoLabel, states);
  }
}
