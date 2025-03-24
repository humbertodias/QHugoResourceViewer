#ifndef HUGORESOURCE_H
#define HUGORESOURCE_H
#include <string>
#include <vector>

namespace hugo {
    class HugoResource {
    public:
        HugoResource();

        std::vector<std::string> processITERES(std::ifstream &fp);

        std::vector<std::string> processBIGFILE(std::ifstream &fp);
    };
}
#endif // HUGORESOURCE_H
