#ifndef TAR_HH
#define TAR_HH

#include <archive.h>
#include <archive_entry.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace stdfs = std::filesystem;

namespace tar {

    inline void makeTar(const std::string source, const std::string output);
    inline void readTar(const std::string tarfile);

    inline void makeTar(const std::string source, const std::string output) {// simple wrapper for making a tar
        struct archive *arc = archive_write_new();
        archive_write_add_filter_gzip(arc);
        archive_write_set_format_pax(arc);

        archive_write_open_filename(arc, output.c_str());

        for (const auto& file: stdfs::recursive_directory_iterator(source)) {
            stdfs::path fullpath = file.path();
            stdfs::path closepath = stdfs::relative(fullpath, source);

            struct archive_entry * entry = archive_entry_new();

            if (stdfs::is_directory(fullpath)) {
                        archive_entry_set_filetype(entry, AE_IFDIR);
                        archive_entry_set_perm(entry, 0755);
                        archive_write_header(arc, entry);
            } else if (stdfs::is_regular_file(fullpath)) {
                archive_entry_set_filetype(entry, AE_IFREG);
                archive_entry_set_size(entry, stdfs::file_size(fullpath));
                archive_entry_set_perm(entry, 0644);
                archive_write_header(arc, entry);

                std::ifstream outFile(fullpath, std::ios::binary);
                char buffer[8192];
                while (outFile.read(buffer, sizeof(buffer)) || outFile.gcount()) {
                    archive_write_data(arc, buffer, outFile.gcount());
                }
            }

            archive_entry_free(entry);

        }

        archive_write_close(arc);
        archive_write_free(arc);

    }

}

#endif
