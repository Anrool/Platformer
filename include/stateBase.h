#ifndef STATE_H
#define STATE_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/NonCopyable.hpp>

class StateManager;

namespace sf {
class Event;
class Time;
} // namespace sf

class StateBase : public sf::Drawable, private sf::NonCopyable {
public:
  explicit StateBase(StateManager &stateManager);

  virtual void handleEvent(const sf::Event &event) = 0;
  virtual void update(sf::Time dt);

protected:
  StateManager &getStateManager() const;

private:
  StateManager &mStateManager;
};

#endif // STATE_H
