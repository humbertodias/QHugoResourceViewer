#ifndef CGF_FILE_H
#define CGF_FILE_H

#include <cstdint>
#include <string>
#include <vector>

class cgfFile {
public:
    struct CgfHead {
        std::string name;
        uint32_t bt;
        uint32_t num;
        uint32_t sizedata;
        uint32_t size;
        uint32_t numpal;
        uint32_t unk;
    };

    struct CgfData {
        uint32_t posx;
        uint32_t posy;
        uint32_t width;
        uint32_t height;
        uint32_t unk;
        uint32_t offset;
    };

private:
    CgfHead head;
    bool st;
    std::string fn;
    uint32_t cgfOffset;

public:
    cgfFile(const std::string& fileName, uint32_t myOffset);
    bool getStatus() const;
    std::vector<uint8_t> getPicture(int num, int& width, int& height);
    std::vector<uint8_t> getPal();
    int getNumPal() const;
    int getNum() const;
};

#endif // CGF_FILE_H
