#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "stateBase.h"

#include <memory>

class World;

class GameState : public StateBase {
public:
  explicit GameState(StateManager &stateManager);
  ~GameState() final;

  void handleEvent(const sf::Event &event) final;
  void update(sf::Time dt) final;

private:
  std::unique_ptr<World> mWorld;

  void setDefaultView() const;

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // GAMESTATE_H
