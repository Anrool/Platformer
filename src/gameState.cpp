#define LOG_TAG "GameState"

#include "gameState.h"
#include "core.h"
#include "resourceManager.h"
#include "stateManager.h"
#include "stateType.h"
#include "utils.h"
#include "world.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

GameState::GameState(StateManager &stateManager)
    : StateBase(stateManager),
      mWorld(makeUnique<World>(stateManager.getCurrentLevel())) {}

GameState::~GameState() { setDefaultView(); }

void GameState::handleEvent(const sf::Event &event) {
  if (event.type == sf::Event::LostFocus ||
      (event.type == sf::Event::KeyPressed &&
       event.key.code == sf::Keyboard::Escape)) {
    setDefaultView();
    getStateManager().requestStateTranstion(STATE_TYPE::PAUSE);
  }
}

void GameState::update(sf::Time dt) {
  auto &manager = getStateManager();

  mWorld->update(dt);

  if (mWorld->failed()) {
    manager.requestStateTranstion(STATE_TYPE::GAME_OVER);
  } else if (mWorld->success()) {
    manager.increaseLevel();

    if (manager.getCurrentLevel() < ResourceManager::getLevelCount()) {
      manager.requestStateTranstion(STATE_TYPE::INTERMEDIATE);
    } else {
      manager.requestStateTranstion(STATE_TYPE::GAME_OVER);
    }
  }
}

void GameState::setDefaultView() const {
  auto &window = ResourceManager::getWindow();
  window.setView(window.getDefaultView());
}

void GameState::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  target.draw(*mWorld, states);
}
