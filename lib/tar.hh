#ifndef TAR_HH
#define TAR_HH

#include <string>
#include <vector>

namespace tar {

    void makeTar(const std::string source, const std::string output);
    std::vector<std::string> readTar(const std::string tarfile);

}

#endif
