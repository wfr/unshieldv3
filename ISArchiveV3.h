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

class ISArchiveV3 {
public:
    ISArchiveV3(const std::filesystem::path& apath);

    class File {
    public:
        std::string name;
        std::string full_path; // Directory separator: \ (Windows)
        uint32_t compressed_size;
        uint32_t uncompressed_size;
        uint32_t datetime;
        uint8_t attrib;
        uint32_t offset;
        uint8_t is_split;
        uint8_t volume_start, volume_end;
    };

    const std::vector<File>& files() const;
    bool exists(const std::string& full_path) const;
    std::vector<uint8_t> decompress(const std::string& full_path);

protected:
    template<class T> T read();
    std::string readString8();
    std::string readString16();
    bool isValidName(const std::string& name) const;
    const File* fileByPath(const std::string& full_path) const;

    const std::filesystem::path& path;
    std::ifstream fin;
    std::vector<File> m_files;
};

