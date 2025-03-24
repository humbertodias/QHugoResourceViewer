#ifndef HUGO_ARCHIVE_PROCESSOR_H
#define HUGO_ARCHIVE_PROCESSOR_H

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace hugo {
    class HugoArchiveProcessor {
    public:
        HugoArchiveProcessor();
        ~HugoArchiveProcessor();

        uint32_t get_offsetsInFile(uint32_t n);

        std::ifstream get_archive_file();

        /**
         * @brief Error event information
         */
        struct ArchiveErrorEventArgs {
            std::string errorMessage;

            explicit ArchiveErrorEventArgs(const std::string &message) : errorMessage(message) {
            }
        };

        /**
         * @brief Callback type for archive loading events
         */
        using ArchiveLoadedCallback = std::function<void()>;

        /**
         * @brief Callback type for archive error events
         */
        using ArchiveErrorCallback = std::function<void(const ArchiveErrorEventArgs &)>;

        /**
         * @brief Opens and processes a Hugo archive file
         * @param filePath Path to the archive file
         * @return True if the archive was opened successfully
         */
        bool OpenArchive(const std::string &filePath);

        /**
         * @brief Extracts a file from the archive
         * @param index Index of the file to extract
         * @return Vector containing the file data, empty vector if failed
         */
        std::vector<uint8_t> ExtractFile(int index);

        /**
         * @brief Gets file data by name
         * @param fileName Name of the file to extract
         * @return Vector containing the file data, empty vector if failed
         */
        std::vector<uint8_t> GetFileByName(const std::string &fileName);

        /**
         * @brief Set callback for archive loaded event
         * @param callback Function to call when archive is loaded
         */
        void SetArchiveLoadedCallback(ArchiveLoadedCallback callback);

        /**
         * @brief Set callback for archive error event
         * @param callback Function to call when an error occurs
         */
        void SetArchiveErrorCallback(ArchiveErrorCallback callback);

        /**
         * @brief Get the list of all file entries in the archive
         * @return Vector of file names
         */
        const std::vector<std::string> &GetFileEntries() const;

        /**
         * @brief Get the list of palette entries in the archive
         * @return Vector of palette file names
         */
        const std::vector<std::string> &GetPaletteEntries() const;

        /**
         * @brief Check if an archive is currently open
         * @return True if an archive is open
         */
        bool IsArchiveOpen() const;

        /**
         * @brief Get the path of the currently open archive
         * @return Path to the current archive, empty if none
         */
        const std::string &GetFilePath() const;
        std::ifstream file;
        std::string filePath;

    private:
        // File properties
        std::vector<uint32_t> offsetsInFile;
        std::vector<uint32_t> sizesInFile;
        int countPal;
        std::vector<int> numPal;
        bool isArchiveOpen;

        // Extracted data
        std::vector<std::string> fileEntries;
        std::vector<std::string> paletteEntries;

        // Event callbacks
        ArchiveLoadedCallback archiveLoadedCallback;
        ArchiveErrorCallback archiveErrorCallback;

        // Private processing methods
        void ProcessITERESArchive(std::ifstream &file);

        void ProcessBIGFILEArchive(std::ifstream &file);

        void ProcessGenericArchive(std::ifstream &file);

        void ProcessStandardArchive(std::ifstream &file, uint32_t offset);

        void ProcessOlderArchiveFormat(std::ifstream &file);

        void ProcessFooterArchive(std::ifstream &file, int footerOffset, int entrySize);

        bool TestIsPalette(const std::string &fileName, uint32_t offset);

        // Helper methods
        void TriggerArchiveLoaded();

        void TriggerArchiveError(const std::string &errorMessage);

        uint32_t ReadUInt32(std::ifstream &file);

        std::string ReadNullTerminatedString(std::ifstream &file);

        std::string ReadNullTerminatedString(const std::vector<uint8_t> &buffer, size_t &position, size_t maxLength);
    };
} // hugo

#endif //HUGO_ARCHIVE_PROCESSOR_H
