#include "formats/lzpfile.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

namespace formats {

lzpFile::lzpHead::lzpHead() {
    std::memset(unk, 0, sizeof(unk));
    std::memset(pal, 0, sizeof(pal));
}

lzpFile::lzpFile(const std::string& fileName, uint32_t myOffset)
    : fn(fileName), lzpOffset(myOffset), st(true) {

    try {
        std::ifstream fp(fn, std::ios::binary);
        if (!fp.is_open()) {
            throw std::ios_base::failure("File not opened");
        }

        fp.seekg(lzpOffset, std::ios::beg);
        fp.read(reinterpret_cast<char*>(&head), sizeof(lzpHead));

        offset.resize(head.num);

        uint32_t currentOffset = 0x320;
        for (uint32_t i = 0; i < head.num; ++i) {
            offset[i] = currentOffset;
            fp.seekg(lzpOffset + currentOffset, std::ios::beg);
            uint32_t sz;
            fp.read(reinterpret_cast<char*>(&sz), sizeof(uint32_t));
            currentOffset += sz + 4;
        }

        fp.close();

        // Setting default width and height if they are 0
        if (head.width == 0) head.width = 320;
        if (head.height == 0) head.height = 240;

    }
    catch (const std::ios_base::failure&) {
        std::cerr << fn << " is not opened." << std::endl;
        st = false;
    }
}

bool lzpFile::getStatus() const {
    return st;
}

std::vector<uint8_t> lzpFile::getPal() const {
    std::vector<uint8_t> pal(768);
    for (int i = 0; i < 256; ++i) {
        pal[i * 3] = head.pal[i * 3 + 2];
        pal[i * 3 + 1] = head.pal[i * 3 + 1];
        pal[i * 3 + 2] = head.pal[i * 3];
    }
    return pal;
}

int lzpFile::getNumPal() const {
    return 256;
}

int lzpFile::getNum() const {
    return head.num;
}

std::vector<uint8_t> lzpFile::getPicture(int num, int& width, int& height) {
    width = head.width;
    height = head.height;
    std::vector<uint8_t> pic(width * height * 2);

    try {
        std::ifstream fp(fn, std::ios::binary);
        if (!fp.is_open()) {
            throw std::ios_base::failure("File not opened");
        }

        fp.seekg(lzpOffset + offset[num], std::ios::beg);

        uint32_t sz;
        fp.read(reinterpret_cast<char*>(&sz), sizeof(uint32_t));
        std::vector<uint8_t> buf(sz * 2);
        fp.read(reinterpret_cast<char*>(buf.data()), sz);

        fp.close();

        std::vector<uint8_t> zbuf(0x1000, 0);
        size_t i = 0, j = 0, k = 0xFEE;

        while (j < width * height) {
            uint32_t bt = static_cast<uint32_t>(buf[i] + 0xFF00);
            ++i;

            while ((bt > 0xFF) && (j < width * height)) {
                if ((bt & 1) != 0) {
                    bt /= 2;
                    pic[j] = buf[i];
                    zbuf[k] = buf[i];
                    ++i;
                    ++j;
                    ++k;
                    k &= 0xFFF;
                }
                else {
                    bt /= 2;
                    uint32_t b1 = buf[i];
                    uint32_t b2 = buf[i + 1];
                    i += 2;
                    uint32_t rz = b2 / 16;
                    b1 += 0x100 * rz;
                    b2 %= 16;
                    b2 += 3;

                    while (b2 != 0) {
                        pic[j] = zbuf[b1];
                        zbuf[k] = zbuf[b1];
                        ++j;
                        ++k;
                        --b2;
                        ++b1;
                        k &= 0xFFF;
                        b1 &= 0xFFF;
                    }
                }
            }
        }

    }
    catch (const std::ios_base::failure&) {
        std::cerr << fn << " is not opened." << std::endl;
        st = false;
    }

    return pic;
}


} // ite
