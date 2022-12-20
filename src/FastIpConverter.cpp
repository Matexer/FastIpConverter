#include "FastIpConverter.h"
#include <arpa/inet.h>


#define MAX_IP_RANGE 16777216
#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define TIMESTAMP_SIZE 20


using namespace std;


FastIpConverter::FastIpConverter(const std::string &inputFilePath, const std::string &outputFilePath):
    inputFilePath{inputFilePath}, outputFilePath{outputFilePath}
{
    this->single = make_shared<vector<string>>();
    this->range = make_shared<vector<IpRange>>();
}

void FastIpConverter::convert()
{
    auto lines = loadLines(this->inputFilePath);
    loadRangesfromFile(lines);
    rangesToSingle();
    saveToFile();
}

void inline FastIpConverter::saveToFile() {
    ofstream outFile(this->outputFilePath);

    for (const auto &row : *this->single) {
        outFile << "host " << row << ",\n";
    }
}

bool FastIpConverter::fileExists(const string &path) {
    struct stat buffer;

    return (stat (path.c_str(), &buffer) == 0);
}

void inline FastIpConverter::rangesToSingle() {
    for (const auto &range : *this->range) {
        auto ips = getIps(range);
        this->single->insert(this->single->end(), ips->begin(), ips->end());
    }
}

void inline FastIpConverter::loadRangesfromFile(LinesListP rawLines) {
    for (uint64_t i = 0; i < rawLines->size() ; i++) {
        auto&& row = rawLines->at(i);

        if (!isLineValid(row)) {
            cout << getTimestamp() << " - "
                 << "Skipped line " << i + 1 << ": '" << row
                 << "' because it is not a valid ip-range.\n";

            continue;
        }

        if (isSingleHost(row)) {
            this->single->push_back(parseSingleIp(row));
        }
        else {
            this->range->push_back(getIpRange(row));
        }
    }
}

bool inline FastIpConverter::isLineValid(const std::string &line)
{
    if (line.find("-") == std::string::npos) {
       return false;
    }

    auto&& dash_pos = getDashPosition(line);
    auto&& start = line.substr(0, dash_pos);
    auto&& end = line.substr(dash_pos + 1, line.length());

    if (!(isIpValid(start) && isIpValid(end))) {
        return false;
    }

    return true;
}

bool inline FastIpConverter::isIpValid(const std::string &ip)
{
    struct sockaddr_in tmp_sockaddr;

    return inet_pton(AF_INET, ip.c_str(), &(tmp_sockaddr.sin_addr)) != 0;
}

shared_ptr<vector<string>> inline FastIpConverter::loadLines(const string inputFilePath) {
    ifstream file_in(inputFilePath);
    auto output = make_shared<vector<string>>();

    string line;
    while (std::getline(file_in, line)) {
        output->push_back(line);
    }

    return output;
}

shared_ptr<vector<string>> inline FastIpConverter::getIps(const IpRange &ipRange) {
    uint32_t distance = ipRange.getDistance() + 1;
    auto ips = make_shared<vector<string>>();

    if (distance > MAX_IP_RANGE) {
      cout << getTimestamp() << " - "
           << "Skipped range" << ": '"
           << inet_ntoa(ipRange.start) << "-" << inet_ntoa(ipRange.end)
           << "' because it is too wide "
           << "(" << distance << " ips).\n";

      return ips;
    }

    ips->resize(distance);

    auto addr = ipRange.start;
    auto addr_ntohl = ntohl(ipRange.start.s_addr);
    for (uint32_t i = 0; i < distance; i++) {
        addr.s_addr = htonl(addr_ntohl);
        ips->at(i) = inet_ntoa(addr);
        addr_ntohl++;
    }

    return ips;
}

FastIpConverter::IpRange inline FastIpConverter::getIpRange(const string &row) {
    IpRange ipRange;
    auto dash_pos = getDashPosition(row);

    auto&& start = row.substr(0, dash_pos);
    inet_pton(AF_INET, start.c_str(), &ipRange.start);

    auto&& end = row.substr(dash_pos + 1, row.length());
    inet_pton(AF_INET, end.c_str(), &ipRange.end);

    if (ntohl(ipRange.start.s_addr) > ntohl(ipRange.end.s_addr)) {
        auto tmp = ipRange.start;
        ipRange.start = ipRange.end;
        ipRange.end = tmp;
    }

    return ipRange;
}

bool inline FastIpConverter::isSingleHost(const string &row) {
    auto&& dash_pos = getDashPosition(row);
    auto&& first = row.substr(0, dash_pos);
    auto&& second = row.substr(dash_pos + 1, row.length());

    if (first == second) {
        return true;
    }
    return false;
}

uint8_t inline FastIpConverter::getDashPosition(const string &row) {
    return row.find('-');
}

string inline FastIpConverter::parseSingleIp(const string &row) {
    auto&& dash_pos = getDashPosition(row);
    return row.substr(0, dash_pos);
}

string inline FastIpConverter::getTimestamp()
{
    auto time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    char tmp_time[TIMESTAMP_SIZE];
    strftime(tmp_time, TIMESTAMP_SIZE, TIMESTAMP_FORMAT, gmtime(&time));
    std::string timeStr(tmp_time);
    return timeStr;
}

u_int32_t FastIpConverter::IpRange::getDistance() const {
    return ntohl(this->end.s_addr) - ntohl(this->start.s_addr);
}
