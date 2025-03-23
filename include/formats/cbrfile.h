#ifndef CBRFILE_H
#define CBRFILE_H

#include <string>
#include <vector>
#include <cstdint>

namespace formats {

    class cbrFile {
    private:
        struct CbrHead {
            uint32_t offsetOfOffsets;
            unsigned char unk[8];
            CbrHead();
        };

        struct CbrData {
            uint32_t width;
            uint32_t height;
            uint32_t posX;
            uint32_t posY;
        };

        bool status;
        std::string filename;
        long number;
        CbrHead head;
        std::vector<uint32_t> offset;
        uint32_t cbrOffset;
        uint32_t cbrSize;

    public:
        cbrFile(const std::string& fileName, uint32_t myOffset, uint32_t mySize);
        bool getStatus() const;
        int getNum() const;
        std::vector<unsigned char> getPicture(int num, int& width, int& height);
    };

} // ite

#endif //CBRFILE_H
