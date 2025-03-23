#include "formats/cbrfile.h"
#include <fstream>
#include <iostream>

namespace formats {


cbrFile::CbrHead::CbrHead() {
    std::fill(std::begin(unk), std::end(unk), 0);
}

cbrFile::cbrFile(const std::string& fileName, uint32_t myOffset, uint32_t mySize)
    : filename(fileName), cbrOffset(myOffset), cbrSize(mySize), status(true) {
    try {
        std::ifstream fp(filename, std::ios::binary);
        if (!fp) {
            throw std::ios_base::failure("File not opened");
        }

        if (cbrSize == 0) {
            fp.seekg(0, std::ios::end);
            cbrSize = static_cast<uint32_t>(fp.tellg());
            fp.seekg(0, std::ios::beg);
        }

        fp.seekg(cbrOffset, std::ios::beg);
        fp.read(reinterpret_cast<char*>(&head.offsetOfOffsets), sizeof(head.offsetOfOffsets));
        fp.read(reinterpret_cast<char*>(head.unk), sizeof(head.unk));

        uint32_t tmp = *reinterpret_cast<uint32_t*>(head.unk);
        if (tmp == 0) {
            status = false;
        } else {
            number = (cbrSize - head.offsetOfOffsets) / 4;
            std::vector<unsigned char> buf(number * 4);
            fp.seekg(cbrOffset + head.offsetOfOffsets, std::ios::beg);
            fp.read(reinterpret_cast<char*>(buf.data()), buf.size());

            offset.resize(number + 1);
            for (size_t i = 0; i < number; ++i) {
                offset[i] = *reinterpret_cast<uint32_t*>(&buf[i * 4]);
            }
            offset[number] = head.offsetOfOffsets;
        }
        fp.close();
    } catch (const std::exception& e) {
        status = false;
        std::cerr << e.what() << std::endl;
    }
}

bool cbrFile::getStatus() const {
    return status;
}

int cbrFile::getNum() const {
    return static_cast<int>(number);
}

std::vector<unsigned char> cbrFile::getPicture(int num, int& width, int& height) {
    std::vector<unsigned char> pic;
    width = height = 0;

    try {
        std::ifstream fp(filename, std::ios::binary);
        if (!fp) {
            throw std::ios_base::failure("File not opened");
        }

        uint32_t sz = offset[num + 1] - offset[num];
        std::vector<unsigned char> buf(sz);
        fp.seekg(cbrOffset + offset[num], std::ios::beg);
        fp.read(reinterpret_cast<char*>(buf.data()), sz);

        CbrData data;
        data.width = *reinterpret_cast<uint32_t*>(buf.data());
        data.height = *reinterpret_cast<uint32_t*>(&buf[4]);
        data.posX = *reinterpret_cast<uint32_t*>(&buf[8]);
        data.posY = *reinterpret_cast<uint32_t*>(&buf[12]);

        width = static_cast<int>(data.width);
        height = static_cast<int>(data.height);

        pic.resize(data.width * data.height * 2, 0);
        int k = 16;
        for (int i = 0; i < data.height; ++i) {
            uint16_t j = *reinterpret_cast<uint16_t*>(&buf[k]);
            uint16_t n = *reinterpret_cast<uint16_t*>(&buf[k + 2]);
            k += 4;
            while (n > 0) {
                pic[(i * data.width + j) * 2] = buf[k];
                pic[(i * data.width + j) * 2 + 1] = (buf[k] != 0) ? 255 : 0;
                ++k;
                ++j;
                --n;
            }
        }

        fp.close();
    } catch (const std::exception& e) {
        status = false;
        std::cerr << e.what() << std::endl;
    }

    return pic;
}

} // ite
