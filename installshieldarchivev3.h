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

#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <map>

/* InstallShield V3 .Z archive reader.
 *
 * Reference (de)compressor: https://www.sac.sk/download/pack/icomp95.zip
 *
 * Assumes little-endian byte order. Don't bother,
 * until someone verifies that blast.c works on big-endian. */
class InstallShieldArchiveV3 {
public:
    InstallShieldArchiveV3(const std::filesystem::path& path);

    class File {
    public:
        std::string name;
        std::string fullpath;
        uint32_t compressed_size;
        uint32_t _offset;
    };

    const std::map<std::string, File>& files() const {
        return m_files;
    }

    bool exists(const std::string& full_path) const;

    std::vector<unsigned char> extract(const std::string& full_path);

protected:
    const std::filesystem::path& path;
    std::ifstream fin;

    template<class T> T read();
    std::string readString8();
    std::string readString16();

    class Directory {
    public:
        std::string name;
        uint16_t file_count;
    };
    std::vector<Directory> directories;

    // <full_path, File>
    std::map<std::string, File> m_files;
};

