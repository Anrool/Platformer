#ifndef OBJECTTYPE_H
#define OBJECTTYPE_H

// entities: [ENTITY_TYPES_START, ENTITY_TYPES_END)
// nonEntities: [NON_ENTITY_TYPES_START, NON_ENTITY_TYPES_START)
// others: [OTHERS, COUNT)

enum class OBJECT_TYPE {
  ENTITY_TYPES_START,

  PLAYER = ENTITY_TYPES_START,

  RUNNER,
  ARCHER,

  VERTICAL_PLATFORM,
  HORIZONTAL_PLATFORM,

  ENTITY_TYPES_END,
  NON_ENTITY_TYPES_START = ENTITY_TYPES_END,

  SOLID = NON_ENTITY_TYPES_START,

  SLOPE_LEFT,
  SLOPE_RIGHT,

  HAZARD,
  FINISH,

  NON_ENTITY_TYPES_END,
  OTHERS = NON_ENTITY_TYPES_END,

  SOLID_ON_LADDER = OTHERS,
  LADDER,

  ALLIED_BULLET,
  ENEMY_BULLET,
  EXPLODED_BULLET,

  PLAYER_SENSOR,

  RAVEN,

  NONE,
  COUNT = NONE
};

#endif // OBJECTTYPE_H