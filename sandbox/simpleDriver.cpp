#include <iostream>
#include <iomanip>
#include <csignal>
#include <fstream>

#include "../include/Svs5Seq/Svs5SequencerApi.h"
#include "../include/GenesisSerializer/GeminiStatusRecord.h"
#include "../include/GenesisSerializer/GlfLoggerGeminiStructure.h"

static unsigned short sg_sonarID = 0;
static bool writeToFile = true;
static std::string outputBasePathStr = "./sonarData/"; 
int targetImageCounter = 0;
bool statusMessageRecieved = false;
bool readyForSettings = false;
bool firstTime = true;


static void callbackFun(
    unsigned int msgType,
    size_t size,
    const char *const value)
    {
        const GLF::GeminiSonarStatusMessage *const statusMsg = (const GLF::GeminiSonarStatusMessage *const)value;
    const GLF::GeminiStatusRecord *const pStatus = &statusMsg->m_geminiSonarStatus;

    switch (msgType) // Add different cases here corresponding to the different messages that can be recieved by the Gemini
    {
        case SequencerApi::GEMINI_STATUS:
    {
        const GLF::GeminiSonarStatusMessage *const statusMsg = (const GLF::GeminiSonarStatusMessage *const)value;
        const GLF::GeminiStatusRecord *const pStatus = &statusMsg->m_geminiSonarStatus;
        // If status message is coming with 0 Alternate IP adrress then don't print anything
        if (!pStatus->m_sonarAltIp)
        {
            return;
        }
        sg_sonarID = pStatus->m_deviceID;
        std::cout << "Status  message received from : ";
        unsigned int from = pStatus->m_sonarAltIp;
        // TODO: IP address writes out backwards. NBD but should fix. 
        std::cout << std::dec << std::setfill('0') << std::setw(2)
        << (int)((from >> 24) & 0xFF) << "." << (int)((from >> 16) & 0xFF) << "."
        << (int)((from >> 8) & 0xFF) << "." << (int)((from >> 0) & 0xFF) << "\n";
        
        std::string s;
        if (!statusMessageRecieved){
            statusMessageRecieved = true;
        }
        break;
    }
    case SequencerApi::GLF_LIVE_TARGET_IMAGE:
    {
        GLF::GLogTargetImage *logTgtImage = (GLF::GLogTargetImage *)value;
        int len = logTgtImage->m_mainImage.m_vecData->size();
        
        std::cout << "data len = " << len
        << ", Width " << logTgtImage->m_mainImage.m_uiEndBearing
        << ", Height " << logTgtImage->m_mainImage.m_uiEndRange
        << ( ( logTgtImage->m_mainImage.m_usPingFlags & 0x8000 ) ? " User Selected SOS " : "Sonar Speed of Sound " )
        << logTgtImage->m_mainImage.m_fSosAtXd
        << std::endl;
        
        // write to file
        if (writeToFile)    
        {
            std::string fileName = "sonar_image_" + std::to_string(targetImageCounter) + ".csv"; // TODO: Better filename? Perhaps include timestamp.
            std::ofstream outFile(outputBasePathStr + fileName);
            if (outFile.is_open())
            {
                int finalCount = 0;
                for (int count = 0; count < len; count++)
                {
                    std::uint8_t val = logTgtImage->m_mainImage.m_vecData->at(count);
                    outFile << std::to_string(val) << ",\n";
                    //std::cout << "count = " << count << "value = " << std::to_string(logTgtImage->m_mainImage.m_vecData->at(count)) << std::endl;
                }
                outFile.close();
                targetImageCounter = targetImageCounter + 1;
            }
            //writeToFile = false; // For Debug if you only want to write one. 
        }
    }
    default:
    {
        std::cout << "other msg type: " << msgType << std::endl;
        break;
    }
}
}

static void ConfigOnline( bool fOnline )
{
    // configure sonar to online / offline
    SequencerApi::Svs5SetConfiguration(
                            SequencerApi::SVS5_CONFIG_ONLINE,
                            sizeof(bool),
                            &fOnline
                            );
}

// For clean exit
void signalHandler(int signum)
{
    printf("Received SIG_TERM...stopping Svs5\n");
    SequencerApi::StopSvs5();
    exit(signum);
}

void ConfigGain(int gain)
{
    SequencerApi::Svs5SetConfiguration(
        SequencerApi::SVS5_CONFIG_GAIN,
        sizeof(int),
        &gain);
        std::cout << "Set:: Gain " << std::dec << gain << std::endl;
    }
    
    void ConfigRange(double rangeLinesMeters)
    {
        SequencerApi::Svs5SetConfiguration(
            SequencerApi::SVS5_CONFIG_RANGE,
        sizeof(double),
        &rangeLinesMeters);

        std::cout << "Set:: Range " << rangeLinesMeters << std::endl;
}

void ConfigNoiseReductionFilter(bool enable)
{
    SequencerApi::Svs5SetConfiguration(
        SequencerApi::SVS5_CONFIG_NOISE_REDUCTION,
        sizeof(bool), (const char* const)&enable);

        std::cout << "Set:: NoiseReductionFilter " << enable << std::endl;

}

int main()
{
    signal(SIGINT, signalHandler);

    std::cout << "SVS5 sequencer library version :  " << SequencerApi::GetLibraryVersionInfo() << std::endl;

    SequencerApi::StartSvs5(std::bind(
        &callbackFun,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3));

    ConfigOnline(true);    

    // Required to keep alive
    while (true)
    {
        if (statusMessageRecieved && !readyForSettings) // We got the first message and can now push settings to the sonar...
        { // Do settings
            // ConfigOnline(true);
            readyForSettings = true;
            std::cout << "Gemini is ready for settings" << std::endl;
        }
        
        if (readyForSettings && firstTime) // We've recieved the first message but have not passed settings to the sonar...
        {
            ConfigRange(50.0);

            ConfigGain(50);

            firstTime = false;
        }
    }

    return 0;
}
