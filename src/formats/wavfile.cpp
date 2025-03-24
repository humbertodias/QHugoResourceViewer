#include "formats/wavfile.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace formats {

    wavFile::wavFile(const std::string& fileName, uint32_t myOffset)
        : fn(fileName), rawOffset(myOffset), st(true) {

        try {
            fp = std::ifstream (fn, std::ios::binary);
            if (!fp.is_open()) {
                throw std::ios_base::failure("File not opened");
            }

            fp.seekg(rawOffset, std::ios::beg);
            fp.read(reinterpret_cast<char*>(&head), sizeof(wavHead));

            if (std::string(head.riff, 4) != "RIFF" || std::string(head.wave, 4) != "WAVE") {
                std::cerr << "Isn't a valid WAV file." << std::endl;
            }

            st = true;

            // fp.close();
        }
        catch (const std::ios_base::failure&) {
            std::cerr << fn << " is not opened." << std::endl;
            st = false;
        }
    }

    bool wavFile::getStatus() const {
        return st;
    }

    std::vector<uint8_t> wavFile::getFileContent() {
        fp.seekg(rawOffset, std::ios::beg);
        uint32_t fileEndOffset = 8 + head.fileSize;
        std::vector<char> buffer(fileEndOffset);
        fp.read(buffer.data(), fileEndOffset);
        return std::vector<uint8_t>(buffer.data(), buffer.data() + fileEndOffset);
    }

} // ite
