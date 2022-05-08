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
#include <chrono>

class ISArchiveV3 {
public:
    ISArchiveV3(const std::filesystem::path& apath);

    class  __attribute__ ((packed)) Header {
    public:
        uint32_t signature1;
        uint32_t signature2;
        uint16_t u1;
        uint16_t is_multivolume;
        uint16_t file_count;
        uint32_t datetime;
        uint32_t compressed_size;
        uint32_t uncompressed_size;
        uint32_t u2;
        uint8_t volume_total;  // set in first vol only, zero in subsequent vols
        uint8_t volume_number; // [1...n]
        uint8_t u3;
        uint32_t split_begin_address;
        uint32_t split_end_address;
        uint32_t toc_address;
        uint32_t u4;
        uint16_t dir_count;
        uint32_t u5;
        uint32_t u6;
    };

    class File {
    public:
        std::string name;
        std::string full_path; // Directory separator: \ (Windows)
        uint16_t index; // position in archive
        uint32_t compressed_size;
        uint32_t uncompressed_size;
        uint32_t datetime;
        uint8_t attrib;
        uint32_t offset;
        uint8_t is_split;
        uint8_t volume_start, volume_end;

        enum Attributes : uint8_t {
            READONLY     = 0x01,
            HIDDEN       = 0x02,
            SYSTEM       = 0x04,
            UNCOMPRESSED = 0x10,
            ARCHIVE      = 0x20
        };

        std::tm tm() const;
        std::filesystem::path path() const;
        std::string attribString() const;
    };

    const std::vector<File>& files() const;
    bool exists(const std::string& full_path) const;
    std::vector<uint8_t> decompress(const std::string& full_path);
    std::filesystem::path path() const {
        return m_path;
    }
    Header header() const {
        return hdr;
    }

protected:
    template<class T> T read();
    std::string readString8();
    std::string readString16();
    bool isValidName(const std::string& name) const;
    const File* fileByPath(const std::string& full_path) const;

    const std::filesystem::path m_path;
    std::ifstream fin;
    std::vector<File> m_files;
    Header hdr;
};

