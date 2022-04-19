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
#include <string>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;


void usage() {
    cerr << "unshieldv3 version " << CMAKE_PROJECT_VER << endl;
    cerr << "usage: " << endl;
    cerr << "  unshieldv3 list ARCHIVE.Z" << endl;
    cerr << "  unshieldv3 extract ARCHIVE.Z DESTINATION" << endl;
}

void info(const ISArchiveV3& archive) {
}

void list(const ISArchiveV3& archive) {
    for (auto file : archive.files()) {
        cout << file.full_path << endl;
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
    for (auto file : archive.files()) {
        cout << file.full_path << endl;
        cout << "      Compressed size: " << setw(10) << file.compressed_size << endl;
        auto contents = archive.decompress(file.full_path);
        cout << "    Uncompressed size: " << setw(10) << contents.size() << endl;

        string fp = file.full_path;
        replace(fp.begin(), fp.end(), '\\', fs::path::preferred_separator);
        fs::path dest = destination / fp;
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


int main(int argc, char** argv) {
    std::string action;
    fs::path archive_path;
    fs::path destination;

    vector<string> args;
    for (int i = 0; i < argc; i++) {
        args.push_back(argv[i]);
    }

    if (args.size() < 3 || args[1] == "-h" || args[1] == "--help") {
        usage();
        return 1;
    }

    if (args.size() >= 3) {
        action = args[1];
        archive_path = args[2];
    }
    if (args.size() >= 4) {
        destination = args[3];
    }

    if (!fs::exists(archive_path)) {
        cerr << "Archive not found: " << archive_path << endl;
        return 1;
    }

    ISArchiveV3 archive(archive_path);

    if (action == "list") {
        list(archive);
    } else
    if (action == "extract") {
        bool success = extract(archive, destination);
        if (!success) {
            cerr << "ERROR: extraction failed" << endl;
            return 1;
        }
    } else {
        usage();
        return 1;
    }
    return 0;
}
