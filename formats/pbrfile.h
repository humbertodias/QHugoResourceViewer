#ifndef PBRFILE_H
#define PBRFILE_H


#include <string>
#include <vector>
#include <cstdint>

namespace formats {

    class pbrFile {
    private:
        class pbrHead {
        public:
            uint32_t offsetOfOffsets;
            uint8_t unk[12];

            pbrHead() {
                std::fill(std::begin(unk), std::end(unk), 0);
            }
        };

        struct pbrData {
            uint16_t posX;
            uint16_t posY;
            uint16_t width;
            uint16_t height;
        };

        bool st;
        std::string fn;
        uint64_t number;
        pbrHead head;
        std::vector<uint32_t> offset;
        uint32_t pbrOffset;
        uint32_t pbrSize;

    public:
        pbrFile(const std::string& fileName, uint32_t myOffset, uint32_t mySize);
        bool getStatus() const;
        int getNum() const;
        std::vector<uint8_t> getPicture(int num, int& width, int& height);

    private:
        uint8_t mysar4(uint8_t b);
    };



} // ite

#endif //PBRFILE_H
