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

#include "installshieldarchivev3.h"
#include <cassert>
#include <iostream>
#include <exception>
extern "C" {
    #include "blast/blast.h"
};

namespace fs = std::filesystem;

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
    uint8_t volume_total;
    uint8_t volume_number;
    uint8_t u3;
    uint32_t split_begin_address;
    uint32_t split_end_address;
    uint32_t toc_address;
    uint32_t u4;
    uint16_t dir_count;
    uint32_t u5;
    uint32_t u6;
};
static const uint32_t data_start = 255;


template<class T> T InstallShieldArchiveV3::read() {
    T re;
    fin.read(reinterpret_cast<char*>(&re), sizeof(re));
    return re;
}

std::string InstallShieldArchiveV3::readString8() {
    uint8_t len = read<uint8_t>();
    std::vector<char> buf(len);
    fin.read(&buf[0], len);
    return std::string(buf.begin(), buf.end());
}

std::string InstallShieldArchiveV3::readString16() {
    uint16_t len = read<uint16_t>();
    std::vector<char> buf(len);
    fin.read(&buf[0], len);
    return std::string(buf.begin(), buf.end());
}

InstallShieldArchiveV3::InstallShieldArchiveV3(const std::filesystem::path& path)
    : path(path)
{
    fin.open(path, std::ios::in | std::ios::binary);
    if (!fin.is_open()) {
        std::cerr << "Cannot open: " << path << std::endl;
        return;
    }
    uint64_t file_size = fs::file_size(path);
    assert(file_size > sizeof(Header));

    Header hdr;
    fin.read(reinterpret_cast<char*>(&hdr), sizeof(Header));
    assert(hdr.signature1 == 0x8C655D13 && hdr.signature2 == 0x02013a);
//    std::cout << "File count: " << hdr.file_count << std::endl;
//    std::cout << "Archive size: " << hdr.archive_size << std::endl;
//    std::cout << "Directory count: " << hdr.dir_count << std::endl;

    assert(hdr.toc_address < file_size);
    fin.seekg(hdr.toc_address, std::ios::beg);

    for (int i = 0; i < hdr.dir_count; i++) {
        uint16_t file_count = read<uint16_t>();
        uint16_t chunk_size = read<uint16_t>();
        std::string name = readString16();
        fin.ignore(chunk_size - uint16_t(name.length()) - 6);
        directories.push_back({name, file_count});
//        std::cout << "Directory: " << name << std::endl;
    }

    for (Directory& directory : directories) {
        for (int i = 0; i < directory.file_count; i++) {
            uint8_t volume_end = read<uint8_t>();
            uint16_t u1 = read<uint16_t>();
            uint32_t uncompressed_size = read<uint32_t>();
            uint32_t compressed_size = read<uint32_t>();
            uint32_t offset = read<uint32_t>();
            uint32_t datetime = read<uint32_t>();
            uint32_t u2 = read<uint32_t>();
            uint16_t chunk_size = read<uint16_t>();
            uint8_t file_attrib = read<uint8_t>();
            uint8_t is_split = read<uint8_t>();
            uint8_t u3 = read<uint8_t>();
            uint8_t volume_start = read<uint8_t>();
            std::string filename = readString8();
            fin.ignore(chunk_size - uint16_t(filename.length()) - 30);

            std::string fullpath;
            if (directory.name.length()) {
                fullpath = directory.name + "\\" + filename;
            } else {
                fullpath = filename;
            }
            m_files[fullpath] = { filename, fullpath, compressed_size, offset };
        }
    }
}


bool InstallShieldArchiveV3::exists(const std::string& full_path) const {
    return m_files.count(full_path) > 0;
}


unsigned _blast_in(void *how, unsigned char **buf) {
    std::vector<unsigned char> *inbuf = reinterpret_cast<std::vector<unsigned char>*>(how);
    *buf = inbuf->data();
    return unsigned(inbuf->size());
}

int _blast_out(void *how, unsigned char *buf, unsigned len) {
    std::vector<unsigned char> *outbuf = reinterpret_cast<std::vector<unsigned char>*>(how);
    outbuf->insert(outbuf->end(), &buf[0], &buf[len]);
    return false; // would indicate write error
}

std::vector<unsigned char> InstallShieldArchiveV3::extract(const std::string& full_path) {
    if (!exists(full_path)) {
        return {};
    }
    const File& file = m_files[full_path];
    fin.seekg(file.offset, std::ios::beg);
    std::vector<unsigned char> buf(file.compressed_size);
    std::vector<unsigned char> out;
    fin.read(reinterpret_cast<char*>(&buf[0]), file.compressed_size);
    if (fin.fail()) {
        throw std::runtime_error("Read failed");
    }

    int ret;
    unsigned left = 0;
    ret = blast(_blast_in, static_cast<void*>(&buf), _blast_out, static_cast<void*>(&out), &left, nullptr);
    if (ret != 0) {
        std::cerr << "Blast error: " << ret << std::endl;
        return {};
    }

    return out;
}
