#pragma once

#define DEFAULT_DATA_EXTENSION      ".dat"
#define DEFAULT_CONFIG_EXTENSION    ".cfg"
#define DEFAULT_LOGGER_EXTENSION    ".glf"

namespace GLF
{

typedef enum{
    GENESIS_LOGGER_MSG_FILE_LOCATION,
    GENESIS_LOGGER_MSG_REC_TGT_IMG,
    GENESIS_LOGGER_MSG_REC_START,
    GENESIS_LOGGER_MSG_REC_STOP,
    GENESIS_LOGGER_MSG_PLAY_START, //std::vector<std::string>&
    GENESIS_LOGGER_MSG_PLAY_FILE_INDEX, //File Index
    GENESIS_LOGGER_MSG_PLAY_FRAME,
    GENESIS_LOGGER_MSG_PLAY_PAUSE,
    GENESIS_LOGGER_MSG_PLAY_SPEED,
    GENESIS_LOGGER_MSG_PLAY_REPEAT,
    GENESIS_LOGGER_MSG_PLAY_STOP,
    GENESIS_LOGGER_MSG_CURRENT_IMAGE,
    GENESIS_LOGGER_APP_VERSION,
}EGLoggerProperties;


} // namespace GLF

