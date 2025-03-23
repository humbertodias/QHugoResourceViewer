#ifndef TILFILE_H
#define TILFILE_H

#include <vector>
#include <string>
#include <cstdint>

namespace formats {


    class tilFile {
    private:
        struct tilHead {
            uint32_t size;
            uint16_t unk1;
            uint16_t numfr;
            uint16_t width;
            uint16_t height;
            uint16_t minfps;
            uint16_t fps;
            uint8_t unk2[4];
            uint16_t wi2;
            uint16_t he2;
            uint8_t unk3[8];
            uint8_t pal[768];
        } head;

        std::string fn;
        bool st;
        uint32_t tilOffset;

    public:
        tilFile(const std::string& fileName, uint32_t myOffset);

        bool getStatus() const;
        std::vector<uint8_t> getPal() const;
        int getNumPal() const;
        int getNum() const;
        std::vector<uint8_t> getPicture(int num, int& width, int& height);
    };

} // ite

#endif //TILFILE_H
