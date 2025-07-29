#ifndef GBL_H
#define GBL_H

#include "def.hh"

namespace global {

    int addGlobalPkg(pkgdef::Package pkg);
    int rmGlobalPkg(pkgdef::Package pkg);
    bool inGlobal(pkgdef::Package pkg);
    gbldef::Global getGlobal();

}

#endif
