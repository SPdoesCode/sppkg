#ifndef DEF_HH
#define DEF_HH

#include <string>
#include <vector>

namespace sppkg {
    const inline std::string root = "/sppkg/";
    const inline std::string buildroot = "/sppkg/build/";
    const inline std::string temproot = "/sppkg/temp/";
    const inline std::string tarroot = "/sppkg/tars/";
    const inline std::string globalfile = "/sppkg/var/global";
    const inline std::string pkgroot = "/sppkg/pkgs/";
}

/*

    Package stuff

*/

namespace pkgdef {

    typedef struct {
        std::string Name;
        std::string Version;
    } PKGDeps;

    typedef struct {
        std::string Name;
        std::string Version;
        std::string Author;
        std::string Source;
    } PKGinfo;

    typedef struct {
        PKGinfo Info;
        std::vector<PKGDeps> Deps;
        std::vector<std::string> Build;
    } Package;
}

/*

    Global file stuff

*/

namespace gbldef {

    typedef struct {
        std::string Name;
        std::string Version;
    } GBLEntry;

    typedef struct {
        std::vector<GBLEntry> Pkgs;
    } Global;

}

#endif
