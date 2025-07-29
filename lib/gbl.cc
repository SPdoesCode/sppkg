#include "gbl.hh"

#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/parser.hpp>
#include <toml++/impl/table.hpp>
#include <iostream>
#include <fstream>
#include <toml++/impl/value.hpp>
#include <utility>
#include <toml++/toml.h>

gbldef::Global global::getGlobal() {
    gbldef::Global gbl;
    try {

        toml::table gbldata = toml::parse_file(sppkg::globalfile);

        auto gbltable = gbldata["Global"].as_array();
        if (!gbldata["Global"]) {
            std::cout << "ERROR: No [Global] found in global file!!\n";
            exit(1);
        }
        for (auto& gblent : *gbltable) {
            if (const auto part = gblent.as_table()) {
                gbl.Pkgs.emplace_back(gbldef::GBLEntry{
                    .Name = part->at("name").value_or("!!No global package name!!"),
                    .Version = part->at("version").value_or("!!No global package version!!")
                });
            }
        }

    } catch (const toml::parse_error& err) {
        std::cout << "ERROR: Couldnt parse global file " << err.description() << " at " << err.source().begin << "\n";
        exit(1);
    }

    return gbl;
}

bool global::inGlobal(pkgdef::Package pkg) {
    gbldef::Global gbl = global::getGlobal();

    for (gbldef::GBLEntry gpkg: gbl.Pkgs) {
        if (gpkg.Name == pkg.Info.Name && gpkg.Version == pkg.Info.Version) {
            return true;
        }
    }

    return false;

}

int global::addGlobalPkg(pkgdef::Package pkg) {

    gbldef::Global gbl = getGlobal();

    gbl.Pkgs.push_back(gbldef::GBLEntry{
       .Name = pkg.Info.Name,
       .Version = pkg.Info.Version
    });

    toml::table tbl;

    toml::array packages;

    for (const gbldef::GBLEntry& gpkg : gbl.Pkgs) {
        toml::table gpackage;
        gpackage.emplace("name", gpkg.Name);
        gpackage.emplace("version", gpkg.Version);
        packages.push_back(gpackage);
    }

    tbl.insert_or_assign("packages", std::move(packages));

    std::ofstream outFile(sppkg::globalfile);
    if (!outFile) {
        std::cout << "Couldnt write to global file ("<<sppkg::globalfile<<")!!\n";
        return 1;
    }

    outFile << tbl;

    return 0;
}
