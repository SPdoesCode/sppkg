#ifndef PKG_HH
#define PKG_HH

#include "def.hh"

namespace pkg {

    pkgdef::Package parsePkg(std::string filepath);
    void installPkg(pkgdef::Package pkg);
    void unstallPkg(pkgdef::Package pkg);
    void lookUp(const std::string name);

}

#endif
