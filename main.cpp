#include "src/FastIpConverter.h"


#define RED "\x1B[31m"
#define WHITE "\033[0m"


using namespace std;


int main (int argc, char *argv[]) {

    const string& inputPath = argv[1];
    const string& outputPath = argv[2];

    if (!FastIpConverter::fileExists(inputPath)) {
        cerr << RED << "Input file '"
             << WHITE << inputPath
             << RED << "' doesn't exists. Operation aborted.\n"
             << WHITE;
      return 1;
    }

    if (FastIpConverter::fileExists(outputPath)) {
        cerr << RED << "Output file '"
             << WHITE << outputPath
             << RED << "' already exists. Operation aborted.\n"
             << WHITE;
      return 2;
    }

    auto converter = FastIpConverter(inputPath, outputPath);
    converter.convert();

    return 0;
}

