#include "formats/brsfile.h"
#include <fstream>

namespace formats {

brsFile::brsFile(const std::string& fileName, uint32_t myOffset, uint32_t mySize)
    : fn(fileName), brsOffset(myOffset), brsSize(mySize), st(true), number(0)
{
    try {
        std::ifstream fp(fn, std::ios::binary);
        if (!fp) {
            st = false;
            return;
        }

        if (brsSize == 0) {
            fp.seekg(0, std::ios::end);
            brsSize = static_cast<uint32_t>(fp.tellg());
            fp.seekg(0, std::ios::beg);
        }

        uint8_t buf[16];
        uint64_t i = 0;
        while (i < brsSize) {
            number++;
            fp.seekg(brsOffset + i, std::ios::beg);
            fp.read(reinterpret_cast<char*>(buf), 16);
            uint32_t w = *reinterpret_cast<uint32_t*>(buf);
            uint32_t h = *reinterpret_cast<uint32_t*>(buf + 4);
            i += w * h + 16;
        }

        i = 0;
        offset.resize(number + 1);
        uint64_t newnum = 0;
        while (i < brsSize) {
            offset[newnum] = static_cast<uint32_t>(i);
            newnum++;
            fp.seekg(brsOffset + i, std::ios::beg);
            fp.read(reinterpret_cast<char*>(buf), 16);
            uint32_t w = *reinterpret_cast<uint32_t*>(buf);
            uint32_t h = *reinterpret_cast<uint32_t*>(buf + 4);
            i += w * h + 16;
        }
        offset[number] = static_cast<uint32_t>(brsSize);
    } catch (const std::ios_base::failure&) {
        st = false;
    }
}

bool brsFile::getStatus() const {
    return st;
}

int brsFile::getNum() const {
    return static_cast<int>(number);
}

std::vector<uint8_t> brsFile::getPicture(int num, int& width, int& height) {
    std::vector<uint8_t> pic;
    width = 0;
    height = 0;

    try {
        std::ifstream fp(fn, std::ios::binary);
        if (!fp) {
            st = false;
            return pic;
        }

        uint8_t buf[256]; // Buffer size larger than required to handle all data

        int sz = static_cast<int>(offset[num + 1] - offset[num]);
        fp.seekg(brsOffset + offset[num], std::ios::beg);
        fp.read(reinterpret_cast<char*>(buf), sz);

        brsData d;
        d.width = *reinterpret_cast<uint32_t*>(buf);
        d.height = *reinterpret_cast<uint32_t*>(buf + 4);
        d.posX = *reinterpret_cast<uint32_t*>(buf + 8);
        d.posY = *reinterpret_cast<uint32_t*>(buf + 12);

        width = static_cast<int>(d.width);
        height = static_cast<int>(d.height);

        pic.resize(d.width * d.height * 2, 0);

        int k = 16;
        for (int i = 0; i < d.height; ++i) {
            for (int j = 0; j < d.width; ++j) {
                pic[(i * d.width + j) * 2] = buf[k];
                pic[(i * d.width + j) * 2 + 1] = (buf[k] == 0) ? 0 : 255;
                ++k;
            }
        }
    } catch (const std::ios_base::failure&) {
        st = false;
    }

    return pic;
}

} // ite
