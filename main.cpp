/* unshieldv3 -- extract InstallShield V3 archives.
Copyright (c) 2019 Wolfgang Frisch <wfrisch@riseup.net>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "config.h"
#include "ISArchiveV3.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>
#include <chrono>

using namespace std;
namespace fs = std::filesystem;


void info(const ISArchiveV3& archive) {
    auto hdr = archive.header();
    cout << "Archive: " << archive.path().string() << endl;
    cout << "File count: " << hdr.file_count << endl;
    cout << "Compressed size: " << hdr.compressed_size << endl;
    cout << "Uncompressed size: " << hdr.uncompressed_size << endl;

    if (hdr.is_multivolume) {
        cout << "This is a multi-volume archive: " << endl;
        cout << "  - volume_number: " << int(hdr.volume_number) << endl;
        cout << "  - volume_total:  " << int(hdr.volume_total) << endl;
        cout << "  - split_begin_address: " << hdr.split_begin_address << endl;
        cout << "  - split_end_address:   " << hdr.split_end_address << endl;
    }
}

void list(const ISArchiveV3& archive, bool verbose = false) {
    size_t max_path = 0;
    if (verbose) {
        for (auto& f : archive.files()) {
            max_path = max(f.full_path.size(), max_path);
        }
        cout << left << setw(max_path) << "Path" << "  "
            << right << setw(8) << "Size" << "  "
            << "Date  "
            << endl;
        cout << left << setw(max_path) << "----" << "  "
            << right << setw(8) << "----" << "  "
            << "----"
            << endl;
    }

    for (auto f : archive.files()) {
        if (verbose) {
            std::tm tm = f.tm();
            cout << left << setw(max_path) << f.full_path << "  "
                << right << setw(8) << f.uncompressed_size << "  "
                << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "  "
                << endl;
        } else {
            cout << f.full_path << endl;
        }
    }
}

bool extract(ISArchiveV3& archive, const fs::path& destination) {
    if (destination.empty()) {
        cerr << "Please specify a destination directory." << endl;
        return false;
    }
    if (!fs::exists(destination)) {
        cerr << "Destination directory not found: " << destination << endl;
        return false;
    }
    for (auto& file : archive.files()) {
        cout << file.full_path << endl;
        cout << "      Compressed size: " << setw(10) << file.compressed_size << endl;
        auto contents = archive.decompress(file.full_path);
        cout << "    Uncompressed size: " << setw(10) << contents.size() << endl;

        fs::path dest = destination / file.path();
        fs::path dest_dir = dest.parent_path();
        if (!fs::create_directories(dest_dir)) {
            if (!fs::exists(dest_dir)) {
                cerr << "Could not create directory: " << dest_dir << endl;
                return false;
            }
        }
        ofstream fout(dest, ios::binary | ios::out);
        if (fout.fail()) {
            cerr << dest << endl;
            cerr << "Could not create file: " << dest << endl;
            return false;
        }
        fout.write(reinterpret_cast<char*>(contents.data()), long(contents.size()));
        if (fout.fail()) {
            cerr << "Could not write to: " << dest << endl;
            return false;
        }
        fout.close();
    }
    return true;
}

int cmd_help(deque<string> subargs = {}) {
    cerr << "unshieldv3 version " << CMAKE_PROJECT_VER << endl;
    cerr << "usage: " << endl;
    cerr << "  unshieldv3 help                        Produce this message" << endl;
    cerr << "  unshieldv3 info ARCHIVE.Z              Show archive metadata" << endl;
    cerr << "  unshieldv3 list [-v] ARCHIVE.Z         List ARCHIVE contents" << endl;
    cerr << "  unshieldv3 extract ARCHIVE.Z DESTDIR   Extract ARCHIVE to DESTDIR" << endl;
    return 1;
}

int cmd_info(deque<string> subargs) {
    string apath;

    if (subargs.size() != 1) {
        return cmd_help();
    }

    apath = subargs[0];
    if (!fs::exists(apath)) {
        cerr << "Archive not found: " << apath << endl;
        return 1;
    }

    ISArchiveV3 archive(apath);
    info(archive);
    return 0;
}

int cmd_list(deque<string> subargs) {
    bool verbose = false;
    string apath;

    if (subargs.size() == 0) {
        return cmd_help();
    }
    if (subargs[0] == "-v") {
        verbose = true;
        subargs.pop_front();
    }
    if (subargs.size() == 1) {
        apath = subargs[0];
    } else {
        return cmd_help();
    }
    if (!fs::exists(apath)) {
        cerr << "Archive not found: " << apath << endl;
        return 1;
    }
    ISArchiveV3 archive(apath);
    list(archive, verbose);
    return 0;
}

int cmd_extract(deque<string> subargs) {
    fs::path apath;
    fs::path destdir;

    if (subargs.size() != 2) {
        return cmd_help();
    }

    apath = subargs[0];
    destdir = subargs[1];
    if (!fs::exists(apath)) {
        cerr << "Archive not found: " << apath << endl;
        return 1;
    }
    ISArchiveV3 archive(apath);
    return extract(archive, destdir) ? 0 : 1;
}

int main(int argc, char** argv) {

    vector<string> args;
    for (int i = 0; i < argc; i++) {
        args.push_back(argv[i]);
    }
    if (args.size() <= 1) {
        return cmd_help();
    }

    deque<string> subargs = deque<string> {args.begin() + 2, args.end()};

    if (args[1] == "help") {
        return cmd_help(subargs);
    }

    if (args[1] == "info") {
        return cmd_info(subargs);
    }

    if (args[1] == "list") {
        return cmd_list(subargs);
    }

    if (args[1] == "extract") {
        return cmd_extract(subargs);
    }

    cmd_help();
    return 1;
}
