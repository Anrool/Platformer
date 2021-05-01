#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <functional>
#include <memory>

namespace sf {
class Event;
class Time;
} // namespace sf

enum class STATE_TYPE;
class StateBase;

class StateManager : public sf::Drawable, private sf::NonCopyable {
public:
  StateManager();
  ~StateManager() final;

  void handleEvent(const sf::Event &event);
  void update(sf::Time dt);

  STATE_TYPE getCurrentStateType() const;

  void requestStateTranstion(STATE_TYPE type);

  size_t getCurrentLevel() const;

  // used only by GameState
  void increaseLevel();

private:
  static const STATE_TYPE mInitialStateType;

  std::unique_ptr<StateBase> mState;
  std::unique_ptr<StateBase> mCachedState;

  STATE_TYPE mCurrentStateType;
  STATE_TYPE mDestinationStateType;

  size_t mCurrentLevel;

  bool hasState() const;

  bool hasStateTransition() const;
  void handleStateTransition();
  bool permitedTransition(STATE_TYPE destinationType) const;

  void resetLevel();

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // STATEMANAGER_H
