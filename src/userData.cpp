#define LOG_TAG "UserData"

#include "userData.h"
#include "core.h"
#include "entity.h"
#include "objectType.h"

UserData::UserData(OBJECT_TYPE type) : mExtended(false) {
  CHECK(type != OBJECT_TYPE::NONE);

  mType = type;
}

UserData::UserData(Entity *const entity) : mExtended(true) {
  NOT_NULL(entity);

  mEntity = entity;
}

bool UserData::isExtended() const { return mExtended; }

OBJECT_TYPE UserData::getType() const {
  return isExtended() ? mEntity->getType() : mType;
}

Entity *UserData::getEntity() const {
  CHECK(isExtended());

  return mEntity;
}
