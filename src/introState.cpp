#define LOG_TAG "IntroState"

#include "introState.h"
#include "animatedEntity.h"
#include "componentDefinition.h"
#include "core.h"
#include "label.h"
#include "resourceManager.h"
#include "soundPlayer.h"
#include "stateManager.h"
#include "stateType.h"
#include "utils.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

const ComponentDefinition IntroState::mHeaderDefinition{
    APPLICATION_NAME,
    ComponentDefinition::mDefaultFontName,
    STATE_HEADER_COLOR,
    STATE_CHAR_SIZE,
    {0.5f * WINDOW_WIDTH, 0.5f * WINDOW_HEIGHT}};

const ComponentDefinition IntroState::mInfoDefinition{
    "Press \"Enter\"",
    "Blackjack.otf",
    {0, 128, 128},
    ComponentDefinition::mDefaultCharacterSize,
    {0.5f * WINDOW_WIDTH, 0.7f * WINDOW_HEIGHT}};
const sf::Time IntroState::mInfoDelay = sf::seconds(0.5f);

const sf::Vector2f IntroState::mEntityInitPosition = {17.25f,
                                                      0.5f * WINDOW_HEIGHT};
const sf::Vector2f IntroState::mEntityScale = {0.25, 0.25f};
const std::string IntroState::mRollingTextureName = "Emptiness.png";
const std::string IntroState::mRavenSound = "Crow.wav";

IntroState::IntroState(StateManager &stateManager)
    : StateBase{stateManager},
      mHeaderLabel(makeUnique<Label>(mHeaderDefinition)),
      mInfoLabel(makeUnique<Label>(mInfoDefinition)),
      mInfoCountdown(mInfoDelay), mShowInfo(true),
      mSprite(makeUnique<sf::Sprite>(
          ResourceManager::getTexture(STATE_TEXTURE_NAME))),
      mAnimatedEntity(makeUnique<AnimatedEntity>(
          AnimatedEntity::INTRO, [this] { onFinishHandler(); })),
      mRollingSprite(makeUnique<sf::Sprite>(
          ResourceManager::getTexture(mRollingTextureName))),
      mAnimationInProgress(true) {
  mSprite->setOrigin(WINDOW_WIDTH, 0.f);

  mAnimatedEntity->setPosition(mEntityInitPosition);
  mAnimatedEntity->setScale(mEntityScale);

  SoundPlayer::play(mRavenSound);
}

IntroState::~IntroState() {}

void IntroState::handleEvent(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed &&
      event.key.code == sf::Keyboard::Return) {
    getStateManager().requestStateTranstion(STATE_TYPE::MENU);
  }
}

void IntroState::update(sf::Time dt) {
  mInfoCountdown -= dt;

  if (mInfoCountdown <= sf::Time::Zero) {
    mInfoCountdown = mInfoDelay;
    mShowInfo = !mShowInfo;
  }

  if (mAnimationInProgress) {
    const sf::Vector2f animationPos = {
        mAnimatedEntity->getPosition().x - mEntityInitPosition.x, 0.f};
    mRollingSprite->setPosition(animationPos);
    mSprite->setPosition(animationPos);
    mAnimatedEntity->update(dt);
  }
}

void IntroState::onFinishHandler() {
  mAnimationInProgress = false;
  mSprite->setPosition(WINDOW_WIDTH, 0.f);
}

void IntroState::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  target.draw(*mSprite, states);

  if (mAnimationInProgress) {
    target.draw(*mRollingSprite, states);
    target.draw(*mAnimatedEntity, states);
  } else {
    target.draw(*mHeaderLabel, states);

    if (mShowInfo) {
      target.draw(*mInfoLabel, states);
    }
  }
}
