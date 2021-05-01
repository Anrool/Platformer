#ifndef TEXTURETYPE_H
#define TEXTURETYPE_H

enum class TEXTURE_TYPE {
  TILES,

  // for animations
  STITCH,
  RUNNER,
  ARCHER,
  PLATFORM,

  // for levels
  FIRST_LEVEL,
  SECOND_LEVEL,
  THIRD_LEVEL,

  // for game over state
  SUCCESS_ENDING,
  FAILED_ENDING,

};

#endif // TEXTURETYPE_H
