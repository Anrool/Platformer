#define LOG_TAG "GameOverState"

#include "gameOverState.h"
#include "animatedEntity.h"
#include "button.h"
#include "buttonView.h"
#include "componentDefinition.h"
#include "core.h"
#include "label.h"
#include "resourceManager.h"
#include "stateManager.h"
#include "stateType.h"
#include "utils.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

const std::string GameOverState::mSuccessTextureName = "Night.png";
const sf::Vector2f GameOverState::mEntityInitPositionSuccess = {
    0.05f * WINDOW_WIDTH, 0.05f * WINDOW_HEIGHT};

const std::string GameOverState::mFailureTextureName = "Ending.png";
const sf::Vector2f GameOverState::mEntityInitPositionFailure = {
    0.5f * WINDOW_WIDTH, 0.825f * WINDOW_HEIGHT};

const ComponentDefinition GameOverState::mHeaderLabelSuccessDefinition{
    "Congratulations!",
    "Capture_it.ttf",
    {210, 105, 30},
    STATE_CHAR_SIZE,
    {0.5f * WINDOW_WIDTH, 0.5f * WINDOW_HEIGHT}};
const ComponentDefinition GameOverState::mHeaderLabelFailureDefinition{
    "Failure!",
    "Capture_it.ttf",
    {128, 0, 0},
    STATE_CHAR_SIZE,
    {0.5f * WINDOW_WIDTH, 0.5f * WINDOW_HEIGHT}};

const ComponentDefinition GameOverState::mTryAgainButtonDefinition{
    "Try again", {0.2f * WINDOW_WIDTH, 0.9f * WINDOW_HEIGHT}};
const ComponentDefinition GameOverState::mGoToMenuButtonDefinition{
    "Go to menu", {0.8f * WINDOW_WIDTH, 0.9f * WINDOW_HEIGHT}};

GameOverState::GameOverState(StateManager &stateManager)
    : StateBase{stateManager}, mHeaderLabel(), mButtonView(),
      mSprite(makeUnique<sf::Sprite>()), mAnimatedEntity(),
      mAnimationInProgress(true) {
  const AnimatedEntity::OnFinishCallback entityCallback = [this] {
    onFinishHandler();
  };

  ButtonView::ButtonList buttons;

  if (stateManager.getCurrentLevel() < ResourceManager::getLevelCount()) {
    mHeaderLabel = makeUnique<Label>(mHeaderLabelFailureDefinition);
    mSprite->setTexture(ResourceManager::getTexture(mFailureTextureName));
    mAnimatedEntity =
        makeUnique<AnimatedEntity>(AnimatedEntity::FAILURE, entityCallback);
    mAnimatedEntity->setPosition(mEntityInitPositionFailure);

    buttons.emplace_back(mTryAgainButtonDefinition, [this] {
      getStateManager().requestStateTranstion(STATE_TYPE::GAME);
    });
  } else {
    mHeaderLabel = makeUnique<Label>(mHeaderLabelSuccessDefinition);
    mSprite->setTexture(ResourceManager::getTexture(mSuccessTextureName));
    mAnimatedEntity =
        makeUnique<AnimatedEntity>(AnimatedEntity::SUCCESS, entityCallback);
    mAnimatedEntity->setPosition(mEntityInitPositionSuccess);
  }

  buttons.emplace_back(mGoToMenuButtonDefinition, [this] {
    getStateManager().requestStateTranstion(STATE_TYPE::MENU);
  });

  mButtonView = makeUnique<ButtonView>(std::move(buttons));
}

GameOverState::~GameOverState() {}

void GameOverState::handleEvent(const sf::Event &event) {
  mButtonView->handleEvent(event);
}

void GameOverState::update(sf::Time dt) { mAnimatedEntity->update(dt); }

void GameOverState::onFinishHandler() { mAnimationInProgress = false; }

void GameOverState::draw(sf::RenderTarget &target,
                         sf::RenderStates states) const {
  target.draw(*mSprite, states);

  if (!mAnimationInProgress) {
    target.draw(*mHeaderLabel, states);
    target.draw(*mButtonView, states);
  }

  target.draw(*mAnimatedEntity, states);
}
