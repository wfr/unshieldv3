# UnshieldV3
Extract InstallShield V3 (Z) archives.

## About InstallShield v3 (Z) archives
InstallShield Z format is a compressed archive format used by version 3 of the InstallShield installation software.

Z files can be recognized by their magic marker: `13 5D 65 8C 3A 01 02 00`.

Contained files are compressed with the "PKware implode" algorithm.

## Usage
```
./unshieldv3 list ARCHIVE.Z
./unshieldv3 extract ARCHIVE.Z DESTINATION/
```

## Building
Requirements: GCC, cmake
```
cd build/
cmake ..
make
```

## History / Credits
[ICOMP95.EXE](https://www.sac.sk/files.php?d=7&l=I) is the original proprietary (de)compressor.

Veit Kannegieser reverse-engineered the file format
and implemented an extractor called ["stix"](https://github.com/DeclanHoare/stix/),
written in a mixture of Pascal and x86 Assembly.

[Mark Adler](https://github.com/madler/) reverse-engineered the compression algorithm,
and provided a clean C implementation as part of [zlib/contrib](https://github.com/madler/zlib/tree/master/contrib/blast).

[baxtor](https://github.com/baxtor) reverse-engineered the original Pascal code
for the [OpenRA project](https://github.com/OpenRA/OpenRA/pull/3342).

[OmniBlade](https://github.com/OmniBlade/) wrote a similar program years ago,
which I was not aware of: <https://github.com/OmniBlade/isextract>

### See also
Later InstallShield archives can be extracted with [Unshield](https://github.com/twogood/unshield).

## Authors
Wolfgang Frisch
