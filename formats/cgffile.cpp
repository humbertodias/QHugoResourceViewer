#include "cgffile.h"
#include <fstream>
#include <iostream>

namespace formats {

class cgffile {
private:
    struct cgfHead {
        std::string name;
        uint32_t bt;
        uint32_t num;
        uint32_t sizedata;
        uint32_t size;
        uint32_t numpal;
        uint32_t unk;
    };

    struct cgfData {
        uint32_t posx;
        uint32_t posy;
        uint32_t width;
        uint32_t height;
        uint32_t unk;
        uint32_t offset;
    };

    cgfHead head;
    bool st;
    std::string fn;
    uint32_t cgfOffset;

public:
    cgffile(const std::string &fileName, uint32_t myOffset) : fn(fileName), st(true), cgfOffset(myOffset) {
        try {
            std::ifstream fp(fn, std::ios::binary);
            if (!fp.is_open()) {
                throw std::ios_base::failure("Failed to open file");
            }

            fp.seekg(cgfOffset, std::ios::beg);
            char buf[28];
            fp.read(buf, 28);

            head.name = std::string(buf, 4);
            head.bt = *reinterpret_cast<uint32_t*>(&buf[4]);
            head.num = *reinterpret_cast<uint32_t*>(&buf[8]);
            head.sizedata = *reinterpret_cast<uint32_t*>(&buf[12]);
            head.size = *reinterpret_cast<uint32_t*>(&buf[16]);
            head.numpal = *reinterpret_cast<uint32_t*>(&buf[20]);
            head.unk = *reinterpret_cast<uint32_t*>(&buf[24]);

            fp.close();
        } catch (const std::exception &e) {
            std::cerr << fn << " is not opened: " << e.what() << std::endl;
            st = false;
        }
    }

    bool getStatus() const {
        return st;
    }

    std::vector<uint8_t> getPicture(int num, int &width, int &height) {
        width = 0;
        height = 0;
        cgfData d1;
        cgfData d2;
        std::vector<uint8_t> pic;

        try {
            std::ifstream fp(fn, std::ios::binary);
            if (!fp.is_open()) {
                throw std::ios_base::failure("Failed to open file");
            }

            fp.seekg(cgfOffset + 28 + num * 24, std::ios::beg);
            char databuf[24];
            fp.read(databuf, 24);

            d1.posx = *reinterpret_cast<uint32_t*>(&databuf[0]);
            d1.posy = *reinterpret_cast<uint32_t*>(&databuf[4]);
            d1.width = *reinterpret_cast<uint32_t*>(&databuf[8]);
            d1.height = *reinterpret_cast<uint32_t*>(&databuf[12]);
            d1.unk = *reinterpret_cast<uint32_t*>(&databuf[16]);
            d1.offset = *reinterpret_cast<uint32_t*>(&databuf[20]);

            if (num == head.num - 1) {
                d2 = {0, 0, 0, 0, 0, 28 + head.sizedata + head.size};
            } else {
                fp.seekg(cgfOffset + 28 + (num + 1) * 24, std::ios::beg);
                fp.read(databuf, 24);
                d2.posx = *reinterpret_cast<uint32_t*>(&databuf[0]);
                d2.posy = *reinterpret_cast<uint32_t*>(&databuf[4]);
                d2.width = *reinterpret_cast<uint32_t*>(&databuf[8]);
                d2.height = *reinterpret_cast<uint32_t*>(&databuf[12]);
                d2.unk = *reinterpret_cast<uint32_t*>(&databuf[16]);
                d2.offset = *reinterpret_cast<uint32_t*>(&databuf[20]);
            }

            width = static_cast<int>(d1.width);
            height = static_cast<int>(d1.height);
            pic.resize(width * height * 2);

            int sz = static_cast<int>(d2.offset - d1.offset);
            if (sz == 0) {
                width = 0;
                height = 0;
                fp.close();
                return {};
            }

            std::vector<uint8_t> buf(sz);
            fp.seekg(cgfOffset + 28 + head.sizedata + d1.offset, std::ios::beg);
            fp.read(reinterpret_cast<char*>(buf.data()), sz);

            for (int i = 0, j = 0; i < d1.width * d1.height;) {
                sz = *reinterpret_cast<uint32_t*>(&buf[j]);
                int k = 4;
                while (k < sz) {
                    uint8_t nmb;
                    if (buf[j + k] == 0) {
                        ++k;
                        nmb = buf[j + k++];
                        while (nmb != 0) {
                            pic[i * 2] = 0;
                            pic[i * 2 + 1] = 0;
                            ++i;
                            --nmb;
                        }
                    } else if (buf[j + k] == 1) {
                        ++k;
                        nmb = buf[j + k++];
                        while (nmb != 0) {
                            pic[i * 2] = buf[j + k];
                            pic[i * 2 + 1] = buf[j + k + 1];
                            ++i;
                            k += 2;
                            --nmb;
                        }
                    } else if (buf[j + k] == 2) {
                        ++k;
                        nmb = buf[j + k++];
                        while (nmb != 0) {
                            pic[i * 2] = buf[j + k];
                            pic[i * 2 + 1] = buf[j + k + 1];
                            ++i;
                            --nmb;
                        }
                        k += 2;
                    } else if (buf[j + k] == 3 || buf[j + k] == 4) {
                        ++k;
                        nmb = buf[j + k++];
                        while (nmb != 0) {
                            pic[i * 2] = buf[j + k];
                            pic[i * 2 + 1] = (buf[j + k - 1] == 3) ? 255 : 255;
                            ++i;
                            --nmb;
                            ++k;
                        }
                    }
                }
                j += k;
                while (i % d1.width != 0) {
                    pic[i * 2] = 0;
                    pic[i * 2 + 1] = 0;
                    ++i;
                }
            }

            fp.close();
        } catch (const std::exception &e) {
            std::cerr << fn << " is not opened: " << e.what() << std::endl;
            st = false;
        }

        return pic;
    }

    std::vector<uint8_t> getPal() {
        std::vector<uint8_t> pal;
        if (head.numpal != 0) {
            try {
                std::ifstream fp(fn, std::ios::binary);
                if (!fp.is_open()) {
                    throw std::ios_base::failure("Failed to open file");
                }

                fp.seekg(cgfOffset + 28 + head.size + head.sizedata, std::ios::beg);
                std::vector<char> buf(256 * 4);
                fp.read(buf.data(), head.numpal * 4);

                pal.resize(256 * 3);
                for (int i = 0; i < head.numpal; ++i) {
                    pal[i * 3] = buf[i * 4];
                    pal[i * 3 + 1] = buf[i * 4 + 1];
                    pal[i * 3 + 2] = buf[i * 4 + 2];
                }

                fp.close();
            } catch (const std::exception &e) {
                std::cerr << fn << " is not opened: " << e.what() << std::endl;
                st = false;
            }
        }
        return pal;
    }

    int getNumPal() const {
        return static_cast<int>(head.numpal);
    }

    int getNum() const {
        return static_cast<int>(head.num);
    }
};


} // ite
