#ifndef BRSFILE_H
#define BRSFILE_H

#include <string>
#include <vector>
#include <cstdint>

namespace formats {

    class brsFile {
    private:
        struct brsData {
            uint32_t width;
            uint32_t height;
            uint32_t posX;
            uint32_t posY;
        };

        bool st;
        std::string fn;
        uint64_t number;
        std::vector<uint32_t> offset;
        uint32_t brsOffset;
        uint32_t brsSize;

    public:
        brsFile(const std::string& fileName, uint32_t myOffset, uint32_t mySize);
        bool getStatus() const;
        int getNum() const;
        std::vector<uint8_t> getPicture(int num, int& width, int& height);
    };

} // ite

#endif //BRSFILE_H
