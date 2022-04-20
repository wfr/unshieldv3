# UnshieldV3
Extract InstallShield V3 (Z) archives.

## About InstallShield v3 (Z) archives
InstallShield Z format is a compressed archive format used by version 3 of the InstallShield installation software.

Z files can be recognized by their magic marker: `13 5D 65 8C 3A 01 02 00`.

Contained files are compressed with the "PKware implode" algorithm.



## Usage
```
usage:
  unshieldv3 help                        Produce this message
  unshieldv3 list [-v] ARCHIVE.Z         List ARCHIVE contents
  unshieldv3 extract ARCHIVE.Z DESTDIR   Extract ARCHIVE to DESTDIR
```

## Building
Requirements: GCC, cmake
```
cd build/
cmake ..
make
```

## References
* Original proprietary (de)compressor: [ICOMP95.EXE](https://www.sac.sk/files.php?d=7&l=I).
* Veit Kannegieser reverse-engineered the file format and wrote
  ["stix"](https://github.com/DeclanHoare/stix/) in a mixture of Pascal and x86
  Assembly.
* [Clean C implementation of the "implode"
  algo](https://github.com/madler/zlib/tree/master/contrib/blast) by [Mark
  Adler](https://github.com/madler/).
* [C# unpacker](https://github.com/OpenRA/OpenRA/pull/3342) in OpenRA.
* [C++ unpacker "isextract"](https://github.com/OmniBlade/isextract) by OmniBlade.
* [Python unpacker "idecomp"](https://github.com/lephilousophe/idecomp) by lephilousophe.

### See also
Later InstallShield archives can be extracted with [Unshield](https://github.com/twogood/unshield).

## Authors
Licensed under Apache 2.0. © Wolfgang Frisch.

### Contributors
* @OmniBlade
* @ligfx
* @adrium
