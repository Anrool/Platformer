#define LOG_TAG "StateBase"

#include "stateBase.h"
#include "core.h"

#include <SFML/System/Time.hpp>

StateBase::StateBase(StateManager &stateManager)
    : mStateManager{stateManager} {}

StateManager &StateBase::getStateManager() const { return mStateManager; }

void StateBase::update(sf::Time /*dt*/) {}
