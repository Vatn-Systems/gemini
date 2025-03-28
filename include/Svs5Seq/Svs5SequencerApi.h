#pragma once

#include <vector>
#include <string>

#include <functional>
#include <string.h>

#ifdef SEQUENCER_API_STATIC_LIB
  #define AFX_EXT_SVS5_SEQUENCER_INTERFACE
#else
  #if defined WIN32 || defined __CYGWIN__
    #ifdef EXPORT_SEQUENCER_API_INTERFACE
      #ifdef __GNUC__
        #define AFX_EXT_SVS5_SEQUENCER_INTERFACE __attribute__((dllexport))
      #else
        #define AFX_EXT_SVS5_SEQUENCER_INTERFACE __declspec(dllexport)
      #endif
    #else
      #ifdef __GNUC__
        #define AFX_EXT_SVS5_SEQUENCER_INTERFACE __attribute__((dllimport))
      #else
        #define AFX_EXT_SVS5_SEQUENCER_INTERFACE __declspec(dllimport)
      #endif
    #endif
  #else
    #if __GNUC__ >= 4
      #define AFX_EXT_SVS5_SEQUENCER_INTERFACE __attribute__((visibility("default")))
    #else
      #define AFX_EXT_SVS5_SEQUENCER_INTERFACE
    #endif
  #endif
#endif

#ifndef WIN32
#ifndef __int64
typedef long long __int64;
#endif
#endif

/*!
 * \brief Library error code Svs5ErrorCode type
 */
typedef unsigned long  Svs5ErrorCode;


/*!
 * \brief Library error codes
 */
#define SVS5_SEQUENCER_STATUS_OK                0
#define SVS5_SEQUENCER_ALREADY_STARTED          ( SVS5_SEQUENCER_STATUS_OK | 1 )
#define SVS5_SEQUENCER_NOT_RUNNING              ( SVS5_SEQUENCER_STATUS_OK | 2 )
#define SVS5_SEQUENCER_ANOTHER_INSTANCE_RUNNING ( SVS5_SEQUENCER_STATUS_OK | 3 )
#define SVS5_SEQUENCER_INVALID_CONFIG           ( SVS5_SEQUENCER_STATUS_OK | 4 )
#define SVS5_SEQUENCER_INVALID_PARAMETERS_SIZE  ( SVS5_SEQUENCER_STATUS_OK | 5 )
#define SVS5_SEQUENCER_INVALID_PARAMETERS_VALUE ( SVS5_SEQUENCER_STATUS_OK | 6 )
#define SVS5_SEQUENCER_INVALID_PROPERTY_ID      ( SVS5_SEQUENCER_STATUS_OK | 7 )
#define SVS5_SEQUENCER_INVALID_PLAYBACK_FILE    ( SVS5_SEQUENCER_STATUS_OK | 8 )
#define SVS5_SEQUENCER_UPGRADE_IN_PROGRESS      ( SVS5_SEQUENCER_STATUS_OK | 9 )

namespace SequencerApi
{

/*!
 * \struct SequencerPingMode
 * \brief User configuration to select free running mode or ping at a fixed interval
 */
struct SequencerPingMode{
    SequencerPingMode()
    : m_bFreeRun( true )        // default free run ( as fast as possible )
    , m_msInterval( 20 )        // ping every 20ms
    , m_extTTLTrigger(false)    //False by default
    {
    }
    bool            m_bFreeRun;         // true: as fast as possible, false: ping based on interval in ms
    unsigned short  m_msInterval;       // 0 - 999
    bool            m_extTTLTrigger;    //Sets whether the TTL Trigger has been activated
};

/*!
 * \struct SequencerSosConfig
 * \brief User configuration to select speed of sound mode and it's value
 */
struct SequencerSosConfig{
    SequencerSosConfig()
    : m_bUsedUserSos( true )// default: used user configurable SOS
    , m_manualSos( 1500.0 ) // default Speed of Sound
    {
    }
    bool    m_bUsedUserSos; // true: used user SOS, false: used sonar SOS
    float   m_manualSos;    // default: 1500.0
};


// Serial Configuration
struct AuxPortConfig{
    unsigned char  m_portNum;       // Only 720is supports 2 Aux ports,
    unsigned int   m_baudRate;      // 9600 - 115200
    bool           m_rs232;         // Only 720is and 720im can switch between RS232/RS485
                                    // Reset of the platforms only support RS232
    AuxPortConfig()
    : m_portNum( 0 )
    , m_baudRate( 9600 )
    , m_rs232( true )
    {
    }
};


// Performance configuration parameters
// User can configure these seetings based on the CPU usage
enum ESdkPerformanceControl{
    LOW_CPU,            // Low image quality ( 256 beams )
    MEDIUM_CPU,         // Medium image quality ( 256 beams )
    HIGH_CPU,           // High image quality ( 512 beams )
    UL_HIGH_CPU         // Ultra high quality
                        // 512 beams for 720is and 720ik,
                        // 1024 beams for 1200ik in higher resolution
};

struct SonarImageQualityLevel{
    SonarImageQualityLevel()
    : m_performance(UL_HIGH_CPU)
    , m_screenPixels( 2048 )
    {
    }
    ESdkPerformanceControl  m_performance;
    int                     m_screenPixels; // Image quality is now automated based on the screen pixels.
                                            // To use the highest quality user screen pixels to 2048
                                            // 1024, 512, 256 respectively
};

struct ListOfFileNames{
    ListOfFileNames()
    : m_numberOfFiles( 0 )
    , m_fileNames( NULL )
    {
    }

    ListOfFileNames( const std::vector<std::string>& fileList )
    : m_numberOfFiles( 0 )
    , m_fileNames( NULL )
    {
        ConstructFileNamesList( fileList );
    }

    ~ListOfFileNames()
    {
        FreeFileNameList();
    }
    void ConstructFileNamesList( const std::vector<std::string>& fileList )
    {
        FreeFileNameList();
        m_numberOfFiles = fileList.size();
        m_fileNames = new char*[ m_numberOfFiles ];
        // Copy all strings
        for (size_t i = 0; i < m_numberOfFiles; ++i )
        {
            size_t fileNameLength = fileList[ i ].size();
            m_fileNames[ i ] = new char [ fileNameLength + 1 ]; // + 1 for NULL string
            memcpy( &m_fileNames[i][0], fileList[ i ].c_str(), fileNameLength );
            m_fileNames[i][ fileNameLength ] = '\0';
        }
    }
    void FreeFileNameList()
    {
        if( m_fileNames )
        {
            for (size_t i = 0; i < m_numberOfFiles; ++i )
            {
                delete m_fileNames[ i ];
            }
            delete []m_fileNames;
        }
    }
    size_t  m_numberOfFiles;
    char**  m_fileNames;
};

// Callback Data Types defined here:
enum ESvs5MessageType{
    GEMINI_STATUS,
    ECD_LIVE_TARGET_IMAGE,  //Deprecated
    GLF_LIVE_TARGET_IMAGE,
    SENSOR_RECORD,
    LOGGER_REC_UPDATE,      // See structure GLF::SOutputFileInfo
    LOGGER_PLAYBACK_UPDATE, // See structure GLF::SInputFileListInfo
    TGT_IMG_PLAYBACK,
    ECD_IMG_PLAYBACK,       //Deprecated
    LOGGER_FILE_INDEX,
    LOGGER_STATUS_INFO,
    FRAME_RATE,
    GPS_RECORD,
    COMPASS_RECORD,
    AUXPORT1_DATA,
    AUXPORT2_DATA,
    UPGRADE_AVAILABLE,
    FIRMWARE_UPGRADE_INFO,
    COM_PORT_STATUS,
    AHRS_RAW_DATA,
    AHRS_HPR_DATA
};

// defines for sonar orientation
enum ESvs5SonarOrientation{
    SONAR_ORIENTATION_UP,       // correct mounting
    SONAR_ORIENTATION_DOWN      // Inverted mounting
};


//Configuration message passed into SVS5 library
enum ESvs5ConfigType{
    SVS5_CONFIG_ONLINE,             // Enable/Disable streaming ( bool )
    SVS5_CONFIG_RANGE,              // Range ( 1 - 120 ) in meters ( double )
    SVS5_CONFIG_GAIN,               // Gain ( 1 - 100 ) in percentage ( int )
    SVS5_CONFIG_SIMULATE_ADC,       // Enable/Disable simulation ( bool )
    SVS5_CONFIG_PING_MODE,          // Ping configuration parametersin SequencerPingMode struct
    SVS5_CONFIG_SOUND_VELOCITY,     // Configure sound velocity in SequencerSosConfig ( Using sonar SOS or user configured )
    SVS5_CONFIG_SONAR_ORIENTATION,  // Sonar orientaton ( See ESvs5SonarOrientation structures )
    SVS5_CONFIG_RANGE_RESOLUTION,   // Configure sonar range resolution ( Only applies to 1200ik product)
    SVS5_CONFIG_HIGH_RESOLUTION,    // Configure sonar improved range resolution ( Only applies to 1200ik product)
    SVS5_CONFIG_CHIRP_MODE,         // Configure sonar chirp mode ( 0: Disabled, 1: Enabled, 2: Auto )
    SVS5_CONFIG_LOG_RAW_GPS,        // Log RAW GPS data
    SVS5_CONFIG_LOG_RAW_COMPASS,    // Log RAW Compass data
    SVS5_CONFIG_CPU_PERFORMANCE,    // Configure SDK based on the CPU performance (SonarImageQualityLevel struct)
    SVS5_CONFIG_APERTURE,           // Configure sonar Aperture ( Can switch between 120 / 65 degrees )
    SVS5_CONFIG_REBOOT_SONAR,       // Reboot Gemini sonar
    SVS5_CONFIG_LOG_GPS,            // Log formatted GPS data
    SVS5_CONFIG_LOG_COMPASS,        // Log formatted Compass data
    SVS5_CONFIG_OPEN_720IM_COM_PORT,// Open 720im / Micron Gemini on regular com port ( See ComPortConfig struct in  GeminiStructuresPublic.h )
    SVS5_CONFIG_AUX_PORT,           // Configure Aux port ( See AuxPortConfig structure )
                                    // Note: Sonar should be online/pinging when sending the AUX port configuration message
    SVS5_CONFIG_NOISE_REDUCTION,    // Configure Noise Reduction (Only supported for Micron Gemini)

    SVS5_CONFIG_FIRMWARE_LOCATION,  // Configure firmware file location
    SVS5_CONFIG_UPGRADE_FIRMWARE,   // Upgrade firmware on the target device
    SVS5_CONFIG_ABORT_UPGRADE,      // Cancel firmware Upgrade on the target device

    // Config logger (ECD/GLF)
    SVS5_CONFIG_LOGGER,             // Configure logger (default : GLF )

    // Record Logger
    SVS5_CONFIG_FILE_LOCATION,      // Configure default location ( const char* )
    SVS5_CONFIG_REC,                // Start/Stop logger ( bool )

    // Playback Logger
    SVS5_CONFIG_PLAY_START,         //List of files (struct ListOfFileNames )
    SVS5_CONFIG_PLAY_FILE_INDEX,    //File Index
    SVS5_CONFIG_PLAY_PAUSE,         //Playback in pause state
    SVS5_CONFIG_PLAY_REPEAT,        //Playback in the loopback state
    SVS5_CONFIG_PLAY_STOP,          //Stop playback
    SVS5_CONFIG_PLAY_SPEED,         //0 for free running, 1: RealTime
    SVS5_CONFIG_PLAY_FRAME,         //Explicitly request frame

    SVS5_CONFIG_RLE_COMPRESSION,    //RLE data compression (level = 0..255(Max), 0 = Compression Off)
    SVS5_CONFIG_H264_COMPRESSION,   //H264 Compression Enabled (true = ON, false = OFF)
};

/*************************************//**
Purpose: User callback function, installed in the library to receive Gemini status messages, Gemini
         Target messages etc.

Parameters:
    msgType             One of the message type defined in ESvs5MessageType
    size                size of value
    void*               data pointer

Returns:                None
****************************************/
typedef std::function<void (
                        unsigned int msgType,
                        unsigned int size,
                        const char* const value
                        ) > Svs5Callback;


/*************************************//**
Purpose: Gets the copy of version string in the data buffer

* e.g. '"v1.0.1\nCopyright (C) 2019 Tritech International Ltd"'

Parameters:
    None

Returns:                char* as mentioned above as an example
****************************************/
AFX_EXT_SVS5_SEQUENCER_INTERFACE const char* GetLibraryVersionInfo( );

/*************************************//**
Purpose: Starts Svs5 library, look for compatible network interface provided and start listening
         from Gemini network interface e.g. broadcast sonar status messages

Parameters:
    fnSvs5              Call back function provided by the application

Returns:                Svs5ErrorCode
****************************************/
AFX_EXT_SVS5_SEQUENCER_INTERFACE Svs5ErrorCode StartSvs5( Svs5Callback fnSvs5 );


/*************************************//**
Purpose: Svs5SetConfiguration : Sets the configuration in the library so it can configure sonar
         accordingly.

Parameters:
    configType          One of the configuration type ESvs5ConfigType
    size                size of value
    void*               data pointer
    deviceID            sonar device ID ( 0: means apply to all, else specific to the device )

    e.g.to configure sonar in online mode
    bool fOnline = true;
    SequencerApi::Svs5SetConfiguration(
                                SequencerApi::SVS5_CONFIG_ONLINE,
                                sizeof(bool),
                                &fOnline
                                );

Returns:                Svs5ErrorCode
****************************************/
AFX_EXT_SVS5_SEQUENCER_INTERFACE Svs5ErrorCode Svs5SetConfiguration(
                                                    ESvs5ConfigType configType,
                                                    size_t          size,
                                                    const void*     configData,
                                                    unsigned int    deviceID = 0
                                                    );

/*************************************//**
Purpose: Svs5SetConfiguration : Gets the last configuration set by the application.

Parameters:
    configType          One of the configuration type ESvs5ConfigType
    size                size of value
    void*               data pointer
    deviceID            sonar device ID ( 0: means apply to all, else specific to the device )

    e.g.to get the last sonar configuration mode (online / offline )
    bool fOnline;
    SequencerApi::Svs5GetConfiguration(
                                SequencerApi::SVS5_CONFIG_ONLINE,
                                sizeof(bool),
                                &fOnline
                                );

Returns:                Svs5ErrorCode
****************************************/
AFX_EXT_SVS5_SEQUENCER_INTERFACE Svs5ErrorCode Svs5GetConfiguration(
                                                        ESvs5ConfigType configType,
                                                        size_t          size,
                                                        void*           configData,
                                                        unsigned int    deviceID = 0
                                                        );

/*************************************//**
Purpose: Stop Svs5 library, Stop listening from the Gemini network interface, release all allocated
         resources and exit

Parameters:
    None

Returns:                Svs5ErrorCode
****************************************/
AFX_EXT_SVS5_SEQUENCER_INTERFACE Svs5ErrorCode StopSvs5( );


/////////////////////////////////////////////////////////////
} // namespace SequencerApi



