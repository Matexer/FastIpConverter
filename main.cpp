#include "src/FastIpConverter.h"


using namespace std;


int main (int argc, char *argv[]) {

    const string& inputPath = argv[1];
    const string& outputPath = argv[2];

    if (!FastIpConverter::fileExists(inputPath)) {
      cout<<"File "<<inputPath<<" doesn't exists.";
      return 1;
    }

    auto converter = FastIpConverter(inputPath, outputPath);
    converter.convert();

    return 0;
}

