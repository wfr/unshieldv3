# UnshieldV3
C++ code and simple CLI tool for reading InstallShield V3 (Z) archives.

## About InstallShield v3 (Z) archives
InstallShield Z format is a compressed archive format used by version 3 of the InstallShield installation software.

Files begin with bytes `13 5D 65 8C 3A 01 02 00`.

It uses the proprietary "implode" compression algorithm by PKware.
Beware that PKware used the term "implode" for various algorithms.

## Usage
Compile with qmake and GCC 7 or later.
```
./unshieldv3 list ARCHIVE.Z
./unshieldv3 extract ARCHIVE.Z DESTINATION/
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

### See also
Later InstallShield archives can be extracted with [Unshield](https://github.com/twogood/unshield).
[OmniBlade](https://github.com/OmniBlade/) implemented a very similar program years ago, which I was not aware of: https://github.com/OmniBlade/isextract

## Authors
Wolfgang Frisch
