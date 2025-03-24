#ifndef WAVFILE_H
#define WAVFILE_H

#include <string>
#include <vector>
#include <cstdint>
#include <fstream>

namespace formats {

    class wavFile {
      public:
        struct wavHead {
            char riff[4];               // "RIFF"
            uint32_t fileSize;          // size
            char wave[4];               // "WAVE"
            char fmt[4];                // "fmt "
            uint32_t fmtSize;           // size of chunk fmt
            uint16_t audioFormat;       // format type (1 = PCM)
            uint16_t numChannels;       // number of channels
            uint32_t sampleRate;        // sample rate
            uint32_t byteRate;          // bytes per second
            uint16_t blockAlign;        // block size
            uint16_t bitsPerSample;     // bites per sample
        } head;

        std::string fn;
        std::ifstream fp;
        bool st;
        uint32_t rawOffset;

    // public:
        wavFile(const std::string& fileName, uint32_t myOffset);

        bool getStatus() const;
        std::vector<uint8_t> getFileContent();
    };

} // ite

#endif //WAVFILE_H
