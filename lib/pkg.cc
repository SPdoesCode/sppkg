#include "pkg.hh"

#include <cstdlib>
#include <fstream>
#include <toml++/toml.h>
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/parser.hpp>
#include <toml++/impl/table.hpp>
#include <iostream>
#include <string>

#include "gbl.hh"
#include "tar.hh"

pkgdef::Package pkg::parsePkg(const std::string pkgname) { // Parse the PKG file and return a struct with the data
    pkgdef::Package pkg;

    try {

        toml::table pkgdata = toml::parse_file(pkgname);
        // Get the basic info
        if (!pkgdata["Info"]) {
            std::cout << "ERROR: No [Info] found in global file("<< pkgname <<")!!\n";
            exit(1);
        }

        if (!pkgdata["Info"]["name"]) {
            std::cout << "ERROR: No name in [Info] found in global file("<< pkgname <<")!!\n";
            exit(1);
        }
        pkg.Info.Name = pkgdata["Info"]["name"].value_or("!!No Name!!");

        if (!pkgdata["Info"]["version"]) {
            std::cout << "ERROR: No version in [Info] found in package file ("<< pkgname <<")!!\n";
            exit(1);
        }
        pkg.Info.Version = pkgdata["Info"]["version"].value_or("!!No Version!!");

        if (!pkgdata["Info"]["source"]) {
            std::cout << "ERROR: No source in [Info] found in package file ("<< pkgname <<")!!\n";
            exit(1);
        }
        pkg.Info.Source = pkgdata["Info"]["source"].value_or("!!No Source!!");

        if (!pkgdata["Info"]["author"]) {
            std::cout << "ERROR: No author in [Info] found in package file ("<< pkgname <<")!!\n";
            exit(1);
        }
        pkg.Info.Author = pkgdata["Info"]["author"].value_or("!!No Author!!");
        // Get the Deps
        if (!pkgdata["Build"]["deps"]) {
            std::cout << "ERROR: No deps in [Build] found in package file ("<< pkgname <<")!!\n";
            exit(1);
        }
        auto deps = pkgdata["Build"]["deps"].as_array();
        for (auto& dep : *deps) {
            if (const auto part = dep.as_table()) {
                pkg.Deps.emplace_back(pkgdef::PKGDeps{
                    .Name = part->at("name").value_or("!!No dep name!!"),
                    .Version = part->at("version").value_or("!!No dep version!!")
                });
            }
        }
        //Get the build info
        if (!pkgdata["Build"]["steps"]) {
            std::cout << "ERROR: No steps in [Build] found in package file ("<< pkgname <<")!!\n";
            exit(1);
        }
        auto build = pkgdata["Build"]["steps"].as_array();
        for (const auto& step : *build) {
            if (auto value = step.as<std::string>()) {
                pkg.Build.push_back(std::string(*value));
            }
        }

    } catch (const toml::parse_error& err) {
        std::cout << "ERROR: Couldnt parse package file ("+pkgname+") " << err.description() << " at " << err.source().begin << "\n";
        exit(1);
    }

    return pkg;
}

void pkg::installPkg(pkgdef::Package pkg) {
    const std::string buildfile = sppkg::buildroot+"build.sh";
    const std::string tarfile = sppkg::tarroot+pkg.Info.Name+pkg.Info.Version+".tar.gz";

    std::fstream outFile(buildfile);

    for (const pkgdef::PKGDeps& dep: pkg.Deps) {

    }

    outFile << "#!/bin/sh\n";
    outFile << "set -e\n";
    outFile << "cd "+sppkg::buildroot+" || exit 1\n";

    for (const std::string& step: pkg.Build ) {
        std::cout << "Putting step " << step << " into " << buildfile << "\n";
        outFile << step << "\n";
    }

    const std::string buildscript = buildfile+" \""+sppkg::temproot+"\" ";

    std::cout << "Installing package "<< pkg.Info.Name <<"...\n";

    int err = std::system(buildscript.c_str());

    if (err != 0) {
        std::cout << "Package build failed and errored with exit " << err << "\n";
    }

    std::cout << "Done...\n";

    std::cout << "Making archive to "<< tarfile <<"...\n";

    tar::makeTar(sppkg::temproot, tarfile);

    std::cout << "Done...\n";

    err = global::addGlobalPkg(pkg);

    if (err != 0) {
        std::cout << "Error adding package "+pkg.Info.Name+" to global file ("<<sppkg::globalfile<<")!!\n";
        exit(0);
    }

}

void pkg::lookUp(const std::string name) {

}
