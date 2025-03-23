#ifndef RAWFILE_H
#define RAWFILE_H

#include <string>
#include <vector>
#include <cstdint>

namespace formats {

    class rawFile {
    private:
        struct rawHead {
            char name[6];
            uint16_t version;
            uint16_t width;
            uint16_t height;
            uint16_t numpal;
            uint8_t unk[18];
            uint8_t pal[768];
        } head;

        std::string fn;
        bool st;
        uint32_t rawOffset;

    public:
        rawFile(const std::string& fileName, uint32_t myOffset);

        bool getStatus() const;
        std::vector<uint8_t> getPal() const;
        int getNumPal() const;
        int getNum() const;
        std::vector<uint8_t> getPicture(int num, int& width, int& height);
    };

} // ite

#endif //RAWFILE_H
