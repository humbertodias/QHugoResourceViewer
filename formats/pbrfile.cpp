#include "pbrfile.h"
#include <fstream>
#include <iostream>

namespace formats {

    pbrFile::pbrFile(const std::string& fileName, uint32_t myOffset, uint32_t mySize)
    : fn(fileName), pbrOffset(myOffset), pbrSize(mySize), st(true), number(0)
{
    try {
        std::ifstream file(fn, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << fn << " is not opened." << std::endl;
            st = false;
            return;
        }

        if (pbrSize == 0) {
            file.seekg(0, std::ios::end);
            pbrSize = static_cast<uint32_t>(file.tellg());
            file.seekg(pbrOffset, std::ios::beg);
        }

        file.read(reinterpret_cast<char*>(&head.offsetOfOffsets), 4);
        file.read(reinterpret_cast<char*>(&head.unk), 12);

        number = (pbrSize - head.offsetOfOffsets) / 4;
        std::vector<uint8_t> buf(number * 4);
        file.seekg(pbrOffset + head.offsetOfOffsets, std::ios::beg);
        file.read(reinterpret_cast<char*>(buf.data()), buf.size());

        offset.resize(number + 1);
        for (size_t i = 0; i < number; ++i) {
            offset[i] = *reinterpret_cast<uint32_t*>(&buf[i * 4]);
        }
        offset[number] = head.offsetOfOffsets;

        file.close();
    }
    catch (const std::exception&) {
        std::cerr << fn << " is not opened." << std::endl;
        st = false;
    }
}

bool pbrFile::getStatus() const {
    return st;
}

int pbrFile::getNum() const {
    return static_cast<int>(number);
}

std::vector<uint8_t> pbrFile::getPicture(int num, int& width, int& height) {
    std::vector<uint8_t> pic;
    width = 0;
    height = 0;

    try {
        std::ifstream file(fn, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << fn << " is not opened." << std::endl;
            st = false;
            return pic;
        }

        pbrData d;
        int sz = static_cast<int>(offset[num + 1] - offset[num]);
        std::vector<uint8_t> buf(sz);
        file.seekg(pbrOffset + offset[num], std::ios::beg);
        file.read(reinterpret_cast<char*>(buf.data()), sz);

        d.posX = *reinterpret_cast<uint16_t*>(&buf[0]);
        d.posY = *reinterpret_cast<uint16_t*>(&buf[2]);
        d.width = *reinterpret_cast<uint16_t*>(&buf[4]);
        d.height = *reinterpret_cast<uint16_t*>(&buf[6]);

        width = static_cast<int>(d.width);
        height = static_cast<int>(d.height);
        pic.resize(d.width * d.height * 2, 0);

        int k = 8;
        for (int i = 0; i < d.height; ++i) {
            uint16_t j = *reinterpret_cast<uint16_t*>(&buf[k]);
            uint16_t n = *reinterpret_cast<uint16_t*>(&buf[k + 2]);
            k += 4;

            while (n > 0) {
                uint8_t t = buf[k];
                ++k;
                --n;
                if (t >= 0x80) {
                    t = static_cast<uint8_t>(t ^ 0xFF);
                    t = static_cast<uint8_t>(t + 1);
                    if (t < 0x40) {
                        while (t > 0) {
                            pic[(i * d.width + j) * 2] = buf[k];
                            pic[(i * d.width + j) * 2 + 1] = (buf[k] == 0) ? 0 : 255;
                            ++j;
                            ++k;
                            --n;
                            --t;
                        }
                    }
                    else {
                        t = static_cast<uint8_t>(t - 0x40);
                        pic[(i * d.width + j) * 2] = buf[k];
                        pic[(i * d.width + j) * 2 + 1] = (buf[k] == 0) ? 0 : 255;
                        ++j;
                        ++k;
                        --n;
                        while (t > 0) {
                            uint8_t t2 = buf[k];
                            ++k;
                            --n;
                            uint8_t t3 = mysar4(t2);
                            pic[(i * d.width + j) * 2] = pic[(i * d.width + j - 1) * 2] + t3;
                            pic[(i * d.width + j) * 2 + 1] = (pic[(i * d.width + j) * 2] == 0) ? 0 : 255;
                            ++j;
                            t2 = mysar4(static_cast<uint8_t>(t2 * 16));
                            pic[(i * d.width + j) * 2] = pic[(i * d.width + j - 1) * 2] + t2;
                            pic[(i * d.width + j) * 2 + 1] = (pic[(i * d.width + j) * 2] == 0) ? 0 : 255;
                            ++j;
                            --t;
                        }
                    }
                }
                else {
                    while (t > 0) {
                        pic[(i * d.width + j) * 2] = buf[k];
                        pic[(i * d.width + j) * 2 + 1] = (buf[k] == 0) ? 0 : 255;
                        ++j;
                        --t;
                    }
                    ++k;
                    --n;
                }
            }
        }
    }
    catch (const std::exception&) {
        std::cerr << fn << " is not opened." << std::endl;
        st = false;
    }

    return pic;
}

uint8_t pbrFile::mysar4(uint8_t b) {
    uint8_t tmp = b;
    for (int i = 0; i < 4; ++i) {
        tmp = static_cast<uint8_t>((tmp / 2) + (tmp & 0x80));
    }
    return tmp;
}

} // ite
