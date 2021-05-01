#ifndef USERDATA_H
#define USERDATA_H

#include <SFML/System/NonCopyable.hpp>

enum class OBJECT_TYPE;
class Entity;

class UserData : private sf::NonCopyable {
public:
  explicit UserData(OBJECT_TYPE type);
  explicit UserData(Entity *const entity);

  OBJECT_TYPE getType() const;

  // informs whether getEntity() call is legal
  bool isExtended() const;

  Entity *getEntity() const;

private:
  const bool mExtended;

  union {
    OBJECT_TYPE mType;
    Entity *mEntity;
  };
};

#endif // USERDATA_H
