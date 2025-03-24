#include "hugo/hugoresource.h"

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "hugo/hugoviewer.h"

namespace hugo {
    HugoResource::HugoResource() {
    }

    std::vector<std::string> HugoResource::processITERES(std::ifstream &fp) {
        std::vector<std::string> fileNames;

        if (!fp) {
            return fileNames;
        }

        // Reading the first 7 bytes
        char head[7];
        fp.read(head, 7);

        std::string he(head, 6);
        if (he == "ITERES") {
            head[0] = 0; // Reset head for next usage
            fp.seekg(6, std::ios::beg);
            fp.read(head, 12);

            uint32_t p1 = *reinterpret_cast<uint32_t *>(head);
            uint32_t p2 = *reinterpret_cast<uint32_t *>(head + 4);
            uint32_t num = *reinterpret_cast<uint32_t *>(head + 8);

            std::vector<uint32_t> offsetsInFile(num);
            std::vector<uint32_t> sizesInFile(num);

            std::vector<char> buf(p1 * p2);
            fp.seekg(0x12, std::ios::beg);
            fp.read(buf.data(), p1 * p2);

            int i = 0;
            int countPal = 0;
            std::vector<int> numPal(num);

            for (uint32_t k = 0; k < num; ++k) {
                sizesInFile[k] = *reinterpret_cast<uint32_t *>(&buf[i]);
                offsetsInFile[k] = *reinterpret_cast<uint32_t *>(&buf[i + 4]) + p1 * p2;
                i += 12;

                std::string newname;
                while (buf[i] != 0) {
                    newname += buf[i];
                    ++i;
                }
                ++i;

                // Add the file name to the list
                fileNames.push_back(newname);

                if (HugoViewer::testPal(newname, offsetsInFile[k])) {
                    numPal[countPal] = k;
                    countPal++;
                    // You can add additional logic here, if needed
                }
            }
        }

        return fileNames;
    }

    std::vector<std::string> HugoResource::processBIGFILE(std::ifstream &file) {
        std::vector<std::string> fileNames;

        if (!file) {
            return fileNames;
        }

        char head[7] = {0};
        file.read(head, 7);
        std::string header(head, 7);

        if (header == "BIGFILE") {
            file.seekg(7, std::ios::beg);
            uint32_t fOffset;
            file.read(reinterpret_cast<char *>(&fOffset), sizeof(fOffset));

            file.seekg(0, std::ios::end);
            uint32_t fileSize = file.tellg();
            uint32_t numEntries = (fileSize - fOffset) / 0x10C;

            std::vector<uint32_t> offsetsInFile(numEntries);
            std::vector<uint32_t> sizesInFile(numEntries);

            file.seekg(fOffset, std::ios::beg);
            std::vector<char> buf(numEntries * 0x10C);
            file.read(buf.data(), buf.size());

            for (uint32_t k = 0; k < numEntries; ++k) {
                sizesInFile[k] = *reinterpret_cast<uint32_t *>(&buf[k * 0x10C + 0x108]);
                offsetsInFile[k] = *reinterpret_cast<uint32_t *>(&buf[k * 0x10C + 0x104]);

                std::string newName;
                int i = k * 0x10C;
                while (buf[i] != 0 && i < k * 0x10C + 0x104) {
                    newName += buf[i++];
                }

                fileNames.push_back(newName);
            }
        }

        return fileNames;
    }

}
