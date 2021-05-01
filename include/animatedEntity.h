#ifndef ANIMATEDENTITY_H
#define ANIMATEDENTITY_H

#include "entity.h"

#include <functional>
#include <map>
#include <vector>

#include <SFML/System/Time.hpp>

class AnimatedEntity : public Entity {
public:
  enum TYPE {
    INTRO,
    SUCCESS,
    FAILURE,
    NONE,
  };

  using OnFinishCallback = std::function<void(void)>;

  explicit AnimatedEntity(TYPE type, OnFinishCallback onFinishCallback);

  void update(sf::Time dt) final;

  OBJECT_TYPE getType() const final;

private:
  struct Movement;

  using MovementVector = std::vector<Movement>;
  using MovementMap = std::map<TYPE, MovementVector>;

  static const MovementMap mMovementMap;

  OBJECT_TYPE mObjectType;
  TYPE mType;
  OnFinishCallback mOnFinishCallback;

  const MovementVector &mMovements;
  MovementVector::const_iterator mCurrentMovement;
  float mDistancePassed;
  sf::Time mMovementCountdown;

  static const MovementVector &findMovements(TYPE type);

  OBJECT_TYPE toObjectType(TYPE type);

  void setVelocityFromMovement();
};

#endif // ANIMATEDENTITY_H
