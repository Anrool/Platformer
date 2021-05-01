#ifndef APPLICATION_H
#define APPLICATION_H

#include <SFML/System/Clock.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <memory>

namespace sf {
class RenderWindow;
}

class StateManager;

class Application : private sf::NonCopyable {
public:
  Application();

  void run();

private:
  static const sf::Time mFrameDuration;

  sf::RenderWindow *mWindow;
  std::unique_ptr<StateManager> mStateManager;

  sf::Clock mClock;
  sf::Time mTimeSincePrevFrame;

private:
  void handleEvents();
  void update(sf::Time dt);
  void render() const;
};

#endif // APPLICATION_H
