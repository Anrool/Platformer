#ifndef PHYSICALBODY_H
#define PHYSICALBODY_H

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/NonCopyable.hpp>

class PhysicalWorld;
enum class OBJECT_TYPE;
class b2Body;

class PhysicalBody : private sf::NonCopyable {
public:
  explicit PhysicalBody(PhysicalWorld &physicalWorld,
                        const sf::FloatRect &bounds, OBJECT_TYPE type);
  ~PhysicalBody();

  void createFootSensor();

  const sf::FloatRect &getBounds() const;
  OBJECT_TYPE getType() const;
  b2Body &getBody() const;

private:
  PhysicalWorld &mPhysicalWorld;
  sf::FloatRect mBounds;
  OBJECT_TYPE mType;
  b2Body *mBody;
};

#endif // PHYSICALBODY_H
