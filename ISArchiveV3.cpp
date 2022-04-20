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

#include "ISArchiveV3.h"
#include <cassert>
#include <iostream>
#include <exception>
extern "C" {
    #include "blast.h"
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


class Directory {
public:
    std::string name;
    uint16_t file_count;
};


ISArchiveV3::ISArchiveV3(const std::filesystem::path& apath)
    : path(apath)
{
    std::vector<Directory> directories;

    fin.open(apath, std::ios::in | std::ios::binary);
    if (!fin.is_open()) {
        std::ostringstream os;
        os << "Cannot open archive: " << apath;
        throw std::runtime_error(os.str());
    }
    uint64_t file_size = fs::file_size(apath);
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
        if (!isValidName(name)) {
            throw std::runtime_error(std::string("Invalid directory name: ") + name);
        }
        directories.push_back({name, file_count});
//        std::cout << "Directory: " << name << std::endl;
    }

    for (Directory& directory : directories) {
        for (int i = 0; i < directory.file_count; i++) {
            File f;

            f.volume_end = read<uint8_t>();
            uint16_t u1 = read<uint16_t>();
            (void)u1;
            f.uncompressed_size = read<uint32_t>();
            f.compressed_size = read<uint32_t>();
            f.offset = read<uint32_t>();
            f.datetime = read<uint32_t>();
            uint32_t u2 = read<uint32_t>();
            (void)u2;
            uint16_t chunk_size = read<uint16_t>();
            f.attrib = read<uint8_t>();
            f.is_split = read<uint8_t>();
            uint8_t u3 = read<uint8_t>();
            (void)u3;
            f.volume_start = read<uint8_t>();
            f.name = readString8();
            fin.ignore(chunk_size - uint16_t(f.name.length()) - 30);

            if (directory.name.length()) {
                f.full_path = directory.name + "\\" + f.name;
            } else {
                f.full_path = f.name;
            }
            if (!isValidName(f.full_path)) {
                throw std::runtime_error(std::string("Invalid file path: ") + f.full_path);
            }

            m_files.push_back(f);
        }
    }
}

const std::vector<ISArchiveV3::File>& ISArchiveV3::files() const {
    return m_files;
}

const ISArchiveV3::File* ISArchiveV3::fileByPath(const std::string& full_path) const {
    // std::map would be more efficient but let's keep it simple for now.
    for (const auto& file : m_files) {
        if (file.full_path == full_path) {
            return &file;
        }
    }
    return nullptr;
}

bool ISArchiveV3::exists(const std::string& full_path) const {
    return fileByPath(full_path) != nullptr;
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

std::vector<uint8_t> ISArchiveV3::decompress(const std::string& full_path) {
    if (!exists(full_path)) {
        std::ostringstream os;
        os << "decompress() called with invalid path: " << full_path;
        throw std::runtime_error(os.str());
    }
    const File* file = fileByPath(full_path);
    assert(file != nullptr);
    fin.seekg(file->offset, std::ios::beg);
    std::vector<unsigned char> buf(file->compressed_size);
    std::vector<unsigned char> out;
    fin.read(reinterpret_cast<char*>(&buf[0]), file->compressed_size);
    if (fin.fail()) {
        throw std::runtime_error("Read failed");
    }

    int ret;
    unsigned left = 0;
    ret = blast(_blast_in, static_cast<void*>(&buf), _blast_out, static_cast<void*>(&out), &left, nullptr);
    if (ret != 0) {
        std::ostringstream os;
        os << "Blast decompression error: " << ret;
        throw std::runtime_error(os.str());
        return {};
    }

    return out;
}

template<class T> T ISArchiveV3::read() {
    T re;
    fin.read(reinterpret_cast<char*>(&re), sizeof(re));
    return re;
}

std::string ISArchiveV3::readString8() {
    uint8_t len = read<uint8_t>();
    std::vector<char> buf(len);
    fin.read(&buf[0], len);
    return std::string(buf.begin(), buf.end());
}

std::string ISArchiveV3::readString16() {
    uint16_t len = read<uint16_t>();
    std::vector<char> buf(len);
    fin.read(&buf[0], len);
    return std::string(buf.begin(), buf.end());
}

bool ISArchiveV3::isValidName(const std::string& name) const {
    if (name.find("..\\") != std::string::npos) {
        return false;
    }
    if (name.find("../") != std::string::npos) {
        return false;
    }
    return true;
}
