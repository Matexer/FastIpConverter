#ifndef FASTIPCONVERTER_H
#define FASTIPCONVERTER_H

#include <cstdint>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <type_traits>
#include <vector>
#include <memory>
#include <arpa/inet.h>
#include <chrono>
#include <ctime>


class FastIpConverter
{
    struct IpRange {
      struct in_addr start;
      struct in_addr end;

      u_int32_t getDistance() const;
    };

    typedef std::shared_ptr<std::vector<std::string>> SinglesListP;
    typedef std::shared_ptr<std::vector<IpRange>> RangesListP;
    typedef SinglesListP LinesListP;

    const std::string &inputFilePath;
    const std::string &outputFilePath;

    struct sockaddr_in tmp_sockaddr;

    SinglesListP single;
    RangesListP range;

public:

    FastIpConverter(const std::string &inputFilePath, const std::string &outputFilePath);

    static bool fileExists(const std::string &path);

    void convert();

private:

    void saveToFile();

    void rangesToSingle();

    void loadRangesfromFile(LinesListP rawLines);

    bool isLineValid(const std::string &line);

    bool isIpValid(const std::string &ip);

    static LinesListP loadLines(const std::string inputFilePath);

    static SinglesListP getIps(const IpRange &ipRange);

    static IpRange getIpRange(const std::string &row);

    static bool isSingleHost(const std::string &row);

    static uint8_t getDashPosition(const std::string &row);

    static std::string parseSingleIp(const std::string &row);
};

#endif // FASTIPCONVERTER_H
