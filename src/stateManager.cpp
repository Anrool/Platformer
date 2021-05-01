#define LOG_TAG "StateManager"

#include "stateManager.h"
#include "core.h"
#include "gameOverState.h"
#include "gameState.h"
#include "intermediateState.h"
#include "introState.h"
#include "menuState.h"
#include "musicPlayer.h"
#include "pauseState.h"
#include "resourceManager.h"
#include "settingsState.h"
#include "soundPlayer.h"
#include "stateType.h"
#include "utils.h"

#include <SFML/Window/Event.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

const STATE_TYPE StateManager::mInitialStateType = STATE_TYPE::INTRO;

StateManager::StateManager()
    : mState(), mCachedState(), mCurrentStateType(STATE_TYPE::NONE),
      mDestinationStateType(STATE_TYPE::NONE), mCurrentLevel(0) {
  requestStateTranstion(mInitialStateType);
  handleStateTransition();
}

StateManager::~StateManager() {}

void StateManager::handleEvent(const sf::Event &event) {
  CHECK(hasState());

  mState->handleEvent(event);
}

void StateManager::update(sf::Time dt) {
  CHECK(hasState());

  mState->update(dt);

  SoundPlayer::removeStopped();

  if (hasStateTransition()) {
    handleStateTransition();
  }
}

STATE_TYPE StateManager::getCurrentStateType() const {
  CHECK(!hasStateTransition());

  return mCurrentStateType;
}

void StateManager::requestStateTranstion(STATE_TYPE type) {
  CHECK(type != STATE_TYPE::NONE);
  CHECK(type != mCurrentStateType);
  CHECK(!hasStateTransition());
  CHECK(permitedTransition(type));

  mDestinationStateType = type;
}

size_t StateManager::getCurrentLevel() const { return mCurrentLevel; }

void StateManager::increaseLevel() {
  CHECK(mCurrentStateType == STATE_TYPE::GAME);
  CHECK(mCurrentLevel < ResourceManager::getLevelCount());

  mCurrentLevel++;
}

bool StateManager::hasState() const {
  return mState != nullptr && mCurrentStateType != STATE_TYPE::NONE;
}

bool StateManager::hasStateTransition() const {
  return mDestinationStateType != STATE_TYPE::NONE;
}

void StateManager::handleStateTransition() {
  CHECK(hasStateTransition());

  if (mDestinationStateType == STATE_TYPE::INTRO) {
    IS_NULL(mState);

    mState = makeUnique<IntroState>(*this);
  } else if (mDestinationStateType == STATE_TYPE::MENU) {
    resetLevel();

    // remove GameState while returning from PauseState
    if (mCurrentStateType == STATE_TYPE::PAUSE) {
      NOT_NULL(mCachedState);

      mCachedState.reset();
    }

    mState = makeUnique<MenuState>(*this);

    if (mCurrentStateType != STATE_TYPE::SETTINGS) {
      MusicPlayer::play(MusicPlayer::MUSIC_TYPE::MENU);
    }
  } else if (mDestinationStateType == STATE_TYPE::SETTINGS) {
    mState = makeUnique<SettingsState>(*this);
  } else if (mDestinationStateType == STATE_TYPE::GAME) {
    // resume game, move back to saved GameState
    if (mCurrentStateType == STATE_TYPE::PAUSE) {
      NOT_NULL(mCachedState);

      mState = std::move(mCachedState);
      MusicPlayer::resume();
    } else {
      mState = makeUnique<GameState>(*this);
      MusicPlayer::play(MusicPlayer::MUSIC_TYPE::GAME);
    }
  } else if (mDestinationStateType == STATE_TYPE::PAUSE) {
    // save GameState so that it can be resumed lately
    mCachedState = std::move(mState);
    mState = makeUnique<PauseState>(*this);
    MusicPlayer::pause();
  } else if (mDestinationStateType == STATE_TYPE::INTERMEDIATE) {
    mState = makeUnique<IntermediateState>(*this);
    MusicPlayer::play(MusicPlayer::MUSIC_TYPE::INTERMEDIATE);
  } else if (mDestinationStateType == STATE_TYPE::GAME_OVER) {
    mState = makeUnique<GameOverState>(*this);

    if (getCurrentLevel() == ResourceManager::getLevelCount()) {
      MusicPlayer::play(MusicPlayer::MUSIC_TYPE::SUCCESS);
    } else {
      MusicPlayer::play(MusicPlayer::MUSIC_TYPE::FAILURE);
    }
  } else {
    CHECK(mDestinationStateType == STATE_TYPE::EXIT);

    ResourceManager::getWindow().close();
  }

  mCurrentStateType = mDestinationStateType;
  mDestinationStateType = STATE_TYPE::NONE;
}

bool StateManager::permitedTransition(STATE_TYPE destinationType) const {
  CHECK(!hasStateTransition());

  switch (destinationType) {
  case STATE_TYPE::INTRO:
    return mCurrentStateType == STATE_TYPE::NONE;
  case STATE_TYPE::MENU:
    return mCurrentStateType == STATE_TYPE::INTRO ||
           mCurrentStateType == STATE_TYPE::SETTINGS ||
           mCurrentStateType == STATE_TYPE::PAUSE ||
           mCurrentStateType == STATE_TYPE::GAME_OVER;
  case STATE_TYPE::SETTINGS:
    return mCurrentStateType == STATE_TYPE::MENU;
  case STATE_TYPE::GAME:
    return mCurrentStateType == STATE_TYPE::MENU ||
           mCurrentStateType == STATE_TYPE::PAUSE ||
           mCurrentStateType == STATE_TYPE::INTERMEDIATE ||
           mCurrentStateType == STATE_TYPE::GAME_OVER;
  case STATE_TYPE::PAUSE:
  case STATE_TYPE::INTERMEDIATE:
  case STATE_TYPE::GAME_OVER:
    return mCurrentStateType == STATE_TYPE::GAME;
  case STATE_TYPE::EXIT:
    return mCurrentStateType == STATE_TYPE::MENU;
  default:
    CHECK(false);
  }
}

void StateManager::resetLevel() { mCurrentLevel = 0; }

void StateManager::draw(sf::RenderTarget &target,
                        sf::RenderStates states) const {
  CHECK(hasState());

  target.draw(*mState, states);
}
