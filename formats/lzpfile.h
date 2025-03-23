#ifndef LZPFILE_H
#define LZPFILE_H

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>


namespace formats {

    class lzpFile {
    private:
        struct lzpHead {
            uint32_t num;
            uint32_t width;
            uint32_t height;
            uint32_t fps;
            uint8_t unk[16];
            uint8_t pal[0x300];

            lzpHead();
        };

        bool st;
        std::string fn;
        std::vector<uint32_t> offset;
        lzpHead head;
        uint32_t lzpOffset;

    public:
        lzpFile(const std::string& fileName, uint32_t myOffset);

        bool getStatus() const;
        std::vector<uint8_t> getPal() const;
        int getNumPal() const;
        int getNum() const;
        std::vector<uint8_t> getPicture(int num, int& width, int& height);
    };

} // ite

#endif //LZPFILE_H
