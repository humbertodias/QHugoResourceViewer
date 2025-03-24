#include "hugo/hugo_archive_processor.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

namespace hugo {
    HugoArchiveProcessor::HugoArchiveProcessor() : countPal(0), isArchiveOpen(false) {
    }

    HugoArchiveProcessor::~HugoArchiveProcessor() {
    }

    uint32_t HugoArchiveProcessor::get_offsetsInFile(uint32_t n) {
        return offsetsInFile[n];
    };

    bool HugoArchiveProcessor::OpenArchive(const std::string &filePath) {
        this->filePath = filePath;
        isArchiveOpen = false;

        // Clear previous data
        offsetsInFile.clear();
        sizesInFile.clear();
        numPal.clear();
        fileEntries.clear();
        paletteEntries.clear();

        try {
            // std::ifstream file(filePath, std::ios::binary);
            file = std::ifstream(filePath, std::ios::binary);
            if (!file.is_open()) {
                TriggerArchiveError(filePath + " could not be opened");
                return false;
            }

            // Read the file header to determine the type
            std::vector<uint8_t> header(7);
            file.read(reinterpret_cast<char *>(header.data()), 7);

            // Check for ITERES format
            std::string headerString(header.begin(), header.begin() + 6);
            if (headerString == "ITERES") {
                ProcessITERESArchive(file);
            }
            // Check for BIGFILE format
            else {
                headerString = std::string(header.begin(), header.begin() + 7);
                if (headerString == "BIGFILE") {
                    ProcessBIGFILEArchive(file);
                } else {
                    ProcessGenericArchive(file);
                }
            }

            file.close();
            isArchiveOpen = true;
            TriggerArchiveLoaded();
            return true;
        } catch (const std::exception &e) {
            TriggerArchiveError(std::string("Error opening archive: ") + e.what());
            return false;
        }
    }

    void HugoArchiveProcessor::ProcessITERESArchive(std::ifstream &file) {
        // Seek to position 6 to read header data
        file.seekg(6, std::ios::beg);

        uint32_t p1 = ReadUInt32(file);
        uint32_t p2 = ReadUInt32(file);
        uint32_t num = ReadUInt32(file);

        // Initialize arrays
        offsetsInFile.resize(num);
        sizesInFile.resize(num);
        numPal.resize(num);
        countPal = 0;

        // Read the buffer containing file information
        std::vector<uint8_t> buffer(p1 * p2);
        file.seekg(0x12, std::ios::beg);
        file.read(reinterpret_cast<char *>(buffer.data()), p1 * p2);

        size_t pos = 0;

        for (uint32_t k = 0; k < num; ++k) {
            // Read file size and offset
            sizesInFile[k] = *reinterpret_cast<uint32_t *>(&buffer[pos]);
            pos += 4;

            offsetsInFile[k] = *reinterpret_cast<uint32_t *>(&buffer[pos]) + p1 * p2;
            pos += 4;

            // Skip 4 bytes (unknown purpose in original code)
            pos += 4;

            // Read filename
            std::string fileName;
            while (buffer[pos] != 0 && pos < buffer.size()) {
                fileName += static_cast<char>(buffer[pos]);
                ++pos;
            }
            ++pos; // Skip null terminator

            fileEntries.push_back(fileName);

            // Check if this is a palette file
            if (TestIsPalette(fileName, offsetsInFile[k])) {
                numPal[countPal] = k;
                countPal++;
                paletteEntries.push_back(fileName);
            }
        }
    }

    void HugoArchiveProcessor::ProcessBIGFILEArchive(std::ifstream &file) {
        // Read the file offset after "BIGFILE" header
        file.seekg(7, std::ios::beg);
        uint32_t fOffset = ReadUInt32(file);

        // Get file size to calculate number of entries
        file.seekg(0, std::ios::end);
        uint32_t fileSize = static_cast<uint32_t>(file.tellg());
        uint32_t num = (fileSize - fOffset) / 0x10C;

        // Initialize arrays
        offsetsInFile.resize(num);
        sizesInFile.resize(num);
        numPal.resize(num);
        countPal = 0;

        // Read the buffer containing file information
        file.seekg(fOffset, std::ios::beg);
        std::vector<uint8_t> buffer(num * 0x10C);
        file.read(reinterpret_cast<char *>(buffer.data()), num * 0x10C);

        for (uint32_t k = 0; k < num; ++k) {
            // Read file size and offset
            sizesInFile[k] = *reinterpret_cast<uint32_t *>(&buffer[k * 0x10C + 0x108]);
            offsetsInFile[k] = *reinterpret_cast<uint32_t *>(&buffer[k * 0x10C + 0x104]);

            // Read filename
            size_t i = k * 0x10C;
            std::string fileName;
            while (buffer[i] != 0 && i < k * 0x10C + 0x104) {
                fileName += static_cast<char>(buffer[i]);
                ++i;
            }

            fileEntries.push_back(fileName);

            // Check if this is a palette file
            if (TestIsPalette(fileName, offsetsInFile[k])) {
                numPal[countPal] = k;
                countPal++;
                paletteEntries.push_back(fileName);
            }
        }
    }

    void HugoArchiveProcessor::ProcessGenericArchive(std::ifstream &file) {
        // Seek back to the beginning
        file.seekg(0, std::ios::beg);
        uint32_t fOffset = ReadUInt32(file) + 4;

        // Check for a valid offset
        if (fOffset != 134284558) {
            ProcessStandardArchive(file, fOffset);
        } else {
            ProcessOlderArchiveFormat(file);
        }
    }

    void HugoArchiveProcessor::ProcessStandardArchive(std::ifstream &file, uint32_t fOffset) {
        file.seekg(fOffset, std::ios::beg);
        uint32_t num = ReadUInt32(file);

        // Initialize arrays
        offsetsInFile.resize(num);
        sizesInFile.resize(num);
        numPal.resize(num);
        countPal = 0;

        // Read the buffer containing file information
        std::vector<uint8_t> buffer(num * 0x48);
        file.read(reinterpret_cast<char *>(buffer.data()), num * 0x48);

        for (uint32_t k = 0; k < num; ++k) {
            // Read file size and offset
            sizesInFile[k] = *reinterpret_cast<uint32_t *>(&buffer[k * 0x48 + 0x44]);
            offsetsInFile[k] = *reinterpret_cast<uint32_t *>(&buffer[k * 0x48 + 0x40]);

            // Read filename
            size_t i = k * 0x48;
            std::string fileName;
            while (buffer[i] != 0 && i < k * 0x48 + 0x40) {
                fileName += static_cast<char>(buffer[i]);
                ++i;
            }

            fileEntries.push_back(fileName);

            // Check if this is a palette file
            if (TestIsPalette(fileName, offsetsInFile[k])) {
                numPal[countPal] = k;
                countPal++;
                paletteEntries.push_back(fileName);
            }
        }
    }

    void HugoArchiveProcessor::ProcessOlderArchiveFormat(std::ifstream &file) {
        // Try with 0x8000 offset from end first
        file.seekg(-0x8000, std::ios::end);
        char tmp[12] = {0};
        file.read(tmp, 11);
        std::string tmpstr(tmp);

        if (tmpstr == "charset.pcc") {
            ProcessFooterArchive(file, 0x8000, 40);
        } else {
            // Try with 0x10000 offset from end
            file.seekg(-0x10000, std::ios::end);
            file.read(tmp, 11);
            tmpstr = std::string(tmp);

            if (tmpstr == "charset.pcc") {
                ProcessFooterArchive(file, 0x10000, 0x48);
            } else {
                TriggerArchiveError(filePath + " is not a Hugo archive");
            }
        }
    }

    void HugoArchiveProcessor::ProcessFooterArchive(std::ifstream &file, int footerOffset, int entrySize) {
        // First, count the number of entries
        uint32_t num = 0;
        std::string fname = "started";

        while (!fname.empty()) {
            file.seekg(-footerOffset + num * entrySize, std::ios::end);

            // Read entry to get the filename
            std::vector<uint8_t> entryBuffer(entrySize);
            file.read(reinterpret_cast<char *>(entryBuffer.data()), entrySize);

            // Extract filename
            fname = "";
            for (int i = 0; i < entrySize && entryBuffer[i] != 0; ++i) {
                fname += static_cast<char>(entryBuffer[i]);
            }

            if (!fname.empty()) {
                num++;
            }
        }

        // Initialize arrays
        offsetsInFile.resize(num);
        sizesInFile.resize(num);
        numPal.resize(num);
        countPal = 0;

        // Read all entries
        file.seekg(-footerOffset, std::ios::end);
        std::vector<uint8_t> buffer(entrySize * num);
        file.read(reinterpret_cast<char *>(buffer.data()), entrySize * num);

        for (uint32_t k = 0; k < num; ++k) {
            int offsetPos = entrySize == 0x48 ? k * 0x48 + 0x40 : k * 40 + 32;
            int sizePos = entrySize == 0x48 ? k * 0x48 + 0x44 : k * 40 + 36;
            int maxNamePos = entrySize == 0x48 ? k * 0x48 + 0x44 : k * 40 + 32;

            // Read file size and offset
            sizesInFile[k] = *reinterpret_cast<uint32_t *>(&buffer[sizePos]);
            offsetsInFile[k] = *reinterpret_cast<uint32_t *>(&buffer[offsetPos]);

            // Read filename
            size_t i = k * entrySize;
            std::string fileName;
            while (buffer[i] != 0 && i < maxNamePos) {
                fileName += static_cast<char>(buffer[i]);
                ++i;
            }

            fileEntries.push_back(fileName);

            // Check if this is a palette file
            if (TestIsPalette(fileName, offsetsInFile[k])) {
                numPal[countPal] = k;
                countPal++;
                paletteEntries.push_back(fileName);
            }
        }
    }

    bool HugoArchiveProcessor::TestIsPalette(const std::string &fileName, uint32_t offset) {
        // For demonstration purposes, let's assume files ending with '.pal' are palettes
        size_t dotPos = fileName.find_last_of('.');
        if (dotPos != std::string::npos) {
            std::string ext = fileName.substr(dotPos);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            return ext == ".pal";
        }
        return false;
    }

    std::vector<uint8_t> HugoArchiveProcessor::ExtractFile(int index) {
        if (!isArchiveOpen || index < 0 || index >= static_cast<int>(sizesInFile.size())) {
            return std::vector<uint8_t>();
        }

        try {
            std::ifstream file(filePath, std::ios::binary);
            if (!file.is_open()) {
                TriggerArchiveError("Failed to open archive for extraction");
                return std::vector<uint8_t>();
            }

            file.seekg(offsetsInFile[index], std::ios::beg);
            std::vector<uint8_t> data(sizesInFile[index]);
            file.read(reinterpret_cast<char *>(data.data()), sizesInFile[index]);

            return data;
        } catch (const std::exception &e) {
            TriggerArchiveError(std::string("Failed to extract file: ") + e.what());
            return std::vector<uint8_t>();
        }
    }

    std::vector<uint8_t> HugoArchiveProcessor::GetFileByName(const std::string &fileName) {
        if (!isArchiveOpen) {
            return std::vector<uint8_t>();
        }

        for (size_t i = 0; i < fileEntries.size(); i++) {
            if (fileEntries[i] == fileName) {
                return ExtractFile(static_cast<int>(i));
            }
        }

        return std::vector<uint8_t>();
    }

    void HugoArchiveProcessor::SetArchiveLoadedCallback(ArchiveLoadedCallback callback) {
        archiveLoadedCallback = callback;
    }

    void HugoArchiveProcessor::SetArchiveErrorCallback(ArchiveErrorCallback callback) {
        archiveErrorCallback = callback;
    }

    const std::vector<std::string> &HugoArchiveProcessor::GetFileEntries() const {
        return fileEntries;
    }

    const std::vector<std::string> &HugoArchiveProcessor::GetPaletteEntries() const {
        return paletteEntries;
    }

    bool HugoArchiveProcessor::IsArchiveOpen() const {
        return isArchiveOpen;
    }

    const std::string &HugoArchiveProcessor::GetFilePath() const {
        return filePath;
    }

    void HugoArchiveProcessor::TriggerArchiveLoaded() {
        if (archiveLoadedCallback) {
            archiveLoadedCallback();
        }
    }

    void HugoArchiveProcessor::TriggerArchiveError(const std::string &errorMessage) {
        if (archiveErrorCallback) {
            archiveErrorCallback(ArchiveErrorEventArgs(errorMessage));
        }
    }

    uint32_t HugoArchiveProcessor::ReadUInt32(std::ifstream &file) {
        uint32_t value;
        file.read(reinterpret_cast<char *>(&value), sizeof(value));
        return value;
    }

    std::string HugoArchiveProcessor::ReadNullTerminatedString(std::ifstream &file) {
        std::string result;
        char c;
        while (file.get(c) && c != '\0') {
            result += c;
        }
        return result;
    }

    std::string HugoArchiveProcessor::ReadNullTerminatedString(const std::vector<uint8_t> &buffer, size_t &position,
                                                               size_t maxLength) {
        std::string result;
        while (position < buffer.size() && position < maxLength && buffer[position] != 0) {
            result += static_cast<char>(buffer[position]);
            ++position;
        }
        ++position; // Skip null terminator
        return result;
    }
} // hugo
