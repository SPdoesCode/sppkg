#include "tar.hh"

#include <archive.h>
#include <archive_entry.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <toml++/toml.h>

namespace stdfs = std::filesystem;

void tar::makeTar(const std::string source, const std::string output) {
    struct archive *arc = archive_write_new();
    archive_write_add_filter_gzip(arc);
    archive_write_set_format_pax(arc);

    archive_write_open_filename(arc, output.c_str());

    for (const auto& file: stdfs::recursive_directory_iterator(source)) {
        stdfs::path fullpath = file.path();
        stdfs::path closepath = stdfs::relative(fullpath, source);

        struct archive_entry * entry = archive_entry_new();
        archive_entry_set_pathname(entry, fullpath.string().c_str());

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

std::vector<std::string> tar::readTar(const std::string tarfile) {
    std::vector<std::string> contents;

    struct archive* archive = archive_read_new();
    struct archive_entry* entry;

    archive_read_support_format_tar(archive);
    archive_read_support_filter_all(archive);

    if (archive_read_open_filename(archive, tarfile.c_str(), 10240) != ARCHIVE_OK) {
        std::cout << "Failed to open archive: " << archive_error_string(archive) << "\n";
        archive_read_free(archive);
        return contents;
    }

    while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
       size_t size = archive_entry_size(entry);
       std::string content;
       content.resize(size);

       ssize_t read_size = archive_read_data(archive, content.data(), size);
       if (read_size < 0) {
           std::cout << "Failed to read data: " << archive_error_string(archive) << "\n";
       } else {
           contents.push_back(std::move(content));
       }
    }

    archive_read_close(archive);
    archive_read_free(archive);

    return contents;
}
