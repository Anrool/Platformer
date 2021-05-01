#define LOG_TAG "Application"

#include "application.h"
#include "core.h"
#include "inputManager.h"
#include "resourceManager.h"
#include "stateManager.h"
#include "utils.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

const sf::Time Application::mFrameDuration = sf::seconds(1.f / FRAMERATE);

Application::Application()
    : mWindow(nullptr), mStateManager(makeUnique<StateManager>()), mClock(),
      mTimeSincePrevFrame() {
  ResourceManager::createInstance();
  InputManager::createInstance();

  mWindow = &ResourceManager::getWindow();

  mWindow->setFramerateLimit(FRAMERATE);
  mWindow->setVerticalSyncEnabled(true);
  mWindow->setKeyRepeatEnabled(false);
}

void Application::run() {
  while (mWindow->isOpen()) {
    mTimeSincePrevFrame += mClock.restart();

    while (mTimeSincePrevFrame > mFrameDuration) {
      mTimeSincePrevFrame -= mFrameDuration;

      handleEvents();

      // uniform update
      update(mFrameDuration);
      render();
    }
  }
}

void Application::handleEvents() {
  sf::Event event;

  while (mWindow->pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      mWindow->close();
      break;
    }

    mStateManager->handleEvent(event);
  }
}

void Application::update(sf::Time dt) { mStateManager->update(dt); }

void Application::render() const {
  mWindow->clear();
  mWindow->draw(*mStateManager);
  mWindow->display();
}

int main() {
  makeUnique<Application>()->run();

  return 0;
}
