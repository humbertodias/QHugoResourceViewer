#include "palfile.h"
#include <fstream>
#include <iostream>

namespace formats {

    palFile::palFile(const std::string& fileName, uint32_t myOffset)
        : palOffset(myOffset), status(true), myPal(0x300) {
        try {
            std::ifstream fp(fileName, std::ios::binary);
            if (!fp) {
                throw std::ios_base::failure("File not opened");
            }

            fp.seekg(palOffset, std::ios::beg);
            unsigned char ch[16];
            fp.read(reinterpret_cast<char*>(ch), 16);

            std::string check(reinterpret_cast<char*>(ch), 10);
            if (check == "CPAL768STD") {
                fp.seekg(palOffset + 10, std::ios::beg);
                fp.read(reinterpret_cast<char*>(myPal.data()), 0x300);
            } else {
                check = std::string(reinterpret_cast<char*>(ch), 12);
                if (check == "CPAL256X3STD") {
                    fp.seekg(palOffset + 12, std::ios::beg);
                    fp.read(reinterpret_cast<char*>(myPal.data()), 0x300);
                } else if (check == "CPAL038X3STD") {
                    fp.seekg(palOffset + 12, std::ios::beg);
                    fp.read(reinterpret_cast<char*>(myPal.data()), 38 * 3);
                } else if (check == "CPAL109X3STD") {
                    fp.seekg(palOffset + 12, std::ios::beg);
                    fp.read(reinterpret_cast<char*>(myPal.data()), 109 * 3);
                }
            }
            fp.close();
        } catch (const std::exception& e) {
            status = false;
            std::cerr << e.what() << std::endl;
        }
    }

    std::vector<unsigned char> palFile::getPal() const {
        std::vector<unsigned char> pal(768);
        for (int i = 0; i < 256; ++i) {
            pal[i * 3] = myPal[i * 3];
            pal[i * 3 + 1] = myPal[i * 3 + 1];
            pal[i * 3 + 2] = myPal[i * 3 + 2];
        }
        return pal;
    }

    bool palFile::getStatus() const {
        return status;
    }


} // ite
