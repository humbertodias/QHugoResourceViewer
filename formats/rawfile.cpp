#include "rawfile.h"
#include <iostream>
#include <fstream>

namespace formats {

    rawFile::rawFile(const std::string& fileName, uint32_t myOffset)
        : fn(fileName), rawOffset(myOffset), st(true) {

        try {
            std::ifstream fp(fn, std::ios::binary);
            if (!fp.is_open()) {
                throw std::ios_base::failure("File not opened");
            }

            fp.seekg(rawOffset, std::ios::beg);
            fp.read(reinterpret_cast<char*>(&head), sizeof(rawHead));

            fp.close();
        }
        catch (const std::ios_base::failure&) {
            std::cerr << fn << " is not opened." << std::endl;
            st = false;
        }
    }

    bool rawFile::getStatus() const {
        return st;
    }

    std::vector<uint8_t> rawFile::getPal() const {
        std::vector<uint8_t> pal(768);
        for (int i = 0; i < 256; ++i) {
            pal[i * 3] = head.pal[i * 3 + 2];
            pal[i * 3 + 1] = head.pal[i * 3 + 1];
            pal[i * 3 + 2] = head.pal[i * 3];
        }
        return pal;
    }

    int rawFile::getNumPal() const {
        return head.numpal;
    }

    int rawFile::getNum() const {
        return 1;
    }

    std::vector<uint8_t> rawFile::getPicture(int num, int& width, int& height) {
        width = head.width;
        height = head.height;
        std::vector<uint8_t> pic(width * height);

        try {
            std::ifstream fp(fn, std::ios::binary);
            if (!fp.is_open()) {
                throw std::ios_base::failure("File not opened");
            }

            fp.seekg(rawOffset + 0x20 + head.numpal * 3, std::ios::beg);
            fp.read(reinterpret_cast<char*>(pic.data()), width * height);

            fp.close();
        }
        catch (const std::ios_base::failure&) {
            std::cerr << fn << " is not opened." << std::endl;
            st = false;
        }

        return pic;
    }

} // ite
