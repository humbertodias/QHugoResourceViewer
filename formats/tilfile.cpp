#include "tilfile.h"
#include <iostream>
#include <fstream>

namespace formats {

tilFile::tilFile(const std::string& fileName, uint32_t myOffset)
    : fn(fileName), tilOffset(myOffset), st(true) {

    head.unk2[0] = head.unk2[1] = head.unk2[2] = head.unk2[3] = 0;
    head.unk3[0] = head.unk3[1] = head.unk3[2] = head.unk3[3] = 0;
    head.unk3[4] = head.unk3[5] = head.unk3[6] = head.unk3[7] = 0;

    try {
        std::ifstream fp(fn, std::ios::binary);
        if (!fp.is_open()) {
            throw std::ios_base::failure("File not opened");
        }

        fp.seekg(tilOffset, std::ios::beg);
        uint8_t buf[32];
        fp.read(reinterpret_cast<char*>(buf), 32);

        head.size = *reinterpret_cast<uint32_t*>(buf);
        head.unk1 = *reinterpret_cast<uint16_t*>(buf + 4);
        head.numfr = *reinterpret_cast<uint16_t*>(buf + 6) + 2;
        head.width = *reinterpret_cast<uint16_t*>(buf + 8);
        head.height = *reinterpret_cast<uint16_t*>(buf + 10);
        head.minfps = *reinterpret_cast<uint16_t*>(buf + 12);
        head.fps = *reinterpret_cast<uint16_t*>(buf + 14);
        std::copy(buf + 16, buf + 20, head.unk2);
        head.wi2 = *reinterpret_cast<uint16_t*>(buf + 21);
        head.he2 = *reinterpret_cast<uint16_t*>(buf + 23);
        std::copy(buf + 24, buf + 32, head.unk3);

        fp.read(reinterpret_cast<char*>(head.pal), 768);

        fp.close();
    }
    catch (const std::ios_base::failure&) {
        std::cerr << fn << " is not opened." << std::endl;
        st = false;
    }
}

bool tilFile::getStatus() const {
    return st;
}

std::vector<uint8_t> tilFile::getPal() const {
    std::vector<uint8_t> pal(768);
    for (int i = 0; i < 256; ++i) {
        pal[i * 3] = head.pal[i * 3 + 2];
        pal[i * 3 + 1] = head.pal[i * 3 + 1];
        pal[i * 3 + 2] = head.pal[i * 3];
    }
    return pal;
}

int tilFile::getNumPal() const {
    return 256;
}

int tilFile::getNum() const {
    return static_cast<int>(head.numfr);
}

std::vector<uint8_t> tilFile::getPicture(int num, int& width, int& height) {
    width = head.width;
    height = head.height;
    std::vector<uint8_t> pic(width * height);
    std::vector<uint8_t> buf(head.wi2 * head.he2 * 2);

    try {
        std::ifstream fp(fn, std::ios::binary);
        if (!fp.is_open()) {
            throw std::ios_base::failure("File not opened");
        }

        fp.seekg(tilOffset + 0x320 + num * head.wi2 * head.he2 * 2, std::ios::beg);
        fp.read(reinterpret_cast<char*>(buf.data()), head.wi2 * head.he2 * 2);

        for (int i = 0; i < head.he2; ++i) {
            for (int j = 0; j < head.wi2; ++j) {
                uint16_t z = *reinterpret_cast<uint16_t*>(buf.data() + i * head.wi2 * 2 + j * 2);
                for (int k = 0; k < 0x10; ++k) {
                    fp.seekg(tilOffset + 0x320 + head.numfr * head.wi2 * head.he2 * 2 + k * 16 + z * 256, std::ios::beg);
                    fp.read(reinterpret_cast<char*>(pic.data()) + head.width * i * 16 + j * 16 + k * head.width, 0x10);
                }
            }
        }

        fp.close();
    }
    catch (const std::ios_base::failure&) {
        std::cerr << fn << " is not opened." << std::endl;
        st = false;
    }

    return pic;
}

} // ite
