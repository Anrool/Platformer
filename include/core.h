#ifndef CORE_H
#define CORE_H

#include <string>

#ifndef LOG_TAG
#define LOG_TAG __FILE__
#endif

// logging:
// additional redirection to provide at least 1 arg to va args
#define LOG(...) C_LOG(__VA_ARGS__, "")
#define C_LOG(fmt, ...)                                                        \
  printf("%s | %s(%u): " fmt "%s\n", LOG_TAG, __FUNCTION__, __LINE__,          \
         ##__VA_ARGS__)

// assertions
#define CHECK(cond)                                                            \
  if (!(cond)) {                                                               \
    LOG("CHECK(%s) FAILED", (#cond));                                          \
    exit(1);                                                                   \
  }

#define NOT_NULL(ptr) CHECK((ptr) != nullptr)
#define IS_NULL(ptr) CHECK((ptr) == nullptr)

const std::string APPLICATION_NAME = "Platformer";

// directories
const std::string MEDIA_DIR = "Media/";
const std::string ENTITIES_DIR = MEDIA_DIR + "Entities/";
const std::string TEXTURES_DIR = MEDIA_DIR + "Textures/";
const std::string FONTS_DIR = MEDIA_DIR + "Fonts/";
const std::string LEVELS_DIR = MEDIA_DIR + "Levels/";
const std::string INPUT_LAYOUT_DIR = MEDIA_DIR + "InputLayout/";
const std::string MUSIC_DIR = MEDIA_DIR + "Music/";
const std::string SOUNDS_DIR = MEDIA_DIR + "Sounds/";

const unsigned int WINDOW_WIDTH = 640u;
const unsigned int WINDOW_HEIGHT = 480u;

const unsigned int FRAMERATE = 60u;

const float GRAVITY = -10.f;

const float DEFAULT_SOUND_VOLUME = 20.f;

#endif // CORE_H
