#ifndef PALFILE_H
#define PALFILE_H

#include <cstdint>
#include <string>
#include <vector>

namespace formats {

    class palFile {
    private:
        std::vector<unsigned char> myPal;
        bool status;
        uint32_t palOffset;

    public:
        palFile(const std::string& fileName, uint32_t myOffset);
        std::vector<unsigned char> getPal() const;
        bool getStatus() const;
    };

} // ite

#endif //PALFILE_H
