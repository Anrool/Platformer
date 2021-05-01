#define LOG_TAG "PhysicalBody"

#include "physicalBody.h"
#include "core.h"
#include "objectType.h"
#include "physicalWorld.h"

#include <Box2D/Common/b2Math.h>

PhysicalBody::PhysicalBody(PhysicalWorld &physicalWorld,
                           const sf::FloatRect &bounds, OBJECT_TYPE type)
    : mPhysicalWorld(physicalWorld), mBounds(bounds), mType(OBJECT_TYPE::NONE),
      mBody(nullptr) {
  CHECK(type != OBJECT_TYPE::NONE);

  mType = type;
  mBody = mPhysicalWorld.createBody(bounds, type);

  NOT_NULL(mBody);
}

PhysicalBody::~PhysicalBody() {
  NOT_NULL(mBody);

  mPhysicalWorld.destroyBody(mBody);
}

void PhysicalBody::createFootSensor() {
  CHECK(mType == OBJECT_TYPE::PLAYER);

  mPhysicalWorld.createFootSensorFor(mBody, {mBounds.width, mBounds.height});
}

const sf::FloatRect &PhysicalBody::getBounds() const { return mBounds; }

OBJECT_TYPE PhysicalBody::getType() const { return mType; }

b2Body &PhysicalBody::getBody() const { return *mBody; }
