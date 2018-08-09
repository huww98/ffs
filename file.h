#if !defined(FILE_H)
#define FILE_H

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>

#include "user.h"

using blockNum_t = std::uint32_t;

class permission
{
  public:
    virtual bool read() const = 0;
    virtual void read(bool r) = 0;
    virtual bool write() const = 0;
    virtual void write(bool w) = 0;
};

template <std::size_t offset>
class linuxStylePermission final : public permission
{
  private:
    std::uint8_t *data;
    constexpr static std::uint8_t readMask = 1 << 2;
    constexpr static std::uint8_t writeMask = 1 << 1;

    void setBit(uint8_t &data, uint8_t mask, bool set)
    {
        mask = mask << offset;
        if (set)
            data |= mask;
        else
            data &= ~mask;
    }

  public:
    linuxStylePermission(std::uint8_t &data) : data(&data) {}

    bool read() const override { return (*data >> offset) & readMask; }
    void read(bool r) override { setBit(*data, readMask, r); }
    bool write() const override { return (*data >> offset) & writeMask; }
    void write(bool w) override { setBit(*data, writeMask, w); }
};

class filePermission
{
  private:
    linuxStylePermission<3> ownerPermission;
    linuxStylePermission<0> allPermission;

  public:
    filePermission(std::uint8_t &data) : ownerPermission(data), allPermission(data) {}
    permission &owner() { return this->ownerPermission; }
    permission &all() { return this->allPermission; }
};

class fileMetadata
{
  private:
    uint8_t _attributeData = 0;
    filePermission _permission;
    ffsuid_t _ownerUID;

    constexpr static uint8_t isDirectoryMask = 1 << 7;

  public:
    fileMetadata(ffsuid_t ownerUID, bool isDirectory = false);
    const filePermission &permission() const { return _permission; }
    ffsuid_t ownerUID() const { return _ownerUID; }
    bool isDirectory() const { return _attributeData & isDirectoryMask; }
    auto &attributeData() const { return _attributeData; }
};

class file
{
  private:
    fileMetadata metadata;
    blockNum_t blockNum;
    file(fileMetadata metadata, blockNum_t blockNum) : metadata(metadata), blockNum(blockNum) {}

  public:
    static std::filesystem::path path(blockNum_t blockNum);
    static file create(fileMetadata metadata, blockNum_t blockNum);

    std::filesystem::path path() { return path(this->blockNum); }
    std::fstream openStream();
};

#endif // FILE_H
