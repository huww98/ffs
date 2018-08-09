#if !defined(FILE_H)
#define FILE_H

#include <cstddef>
#include <cstdint>
#include <filesystem>

#include "user.h"

using blockNum_t = std::uint32_t;

class permission
{
    virtual bool read() const = 0;
    virtual bool write() const = 0;
};

template <std::size_t offset>
class linuxStylePermission final : public permission
{
  private:
    std::uint8_t *data;
    constexpr static std::uint8_t readMask = 0b100;
    constexpr static std::uint8_t writeMask = 0b010;

  public:
    linuxStylePermission(std::uint8_t &data) : data(&data){}

    bool read() const override { return (*data >> offset) & readMask; }
    bool write() const override { return (*data >> offset) & readMask; }
};

class filePermission
{
  private:
    std::uint8_t _data;
    linuxStylePermission<6> ownerPermission;
    linuxStylePermission<0> allPermission;

  public:
    filePermission() : ownerPermission(_data), allPermission(_data){}
    permission &owner() { return this->ownerPermission; }
    permission &all() { return this->allPermission; }
    std::uint8_t data() const { return this->_data; }
};

class fileMetadata
{
  private:
    filePermission _permission;
    ffsuid_t _ownerUID;
  public:
    fileMetadata(ffsuid_t ownerUID) : _ownerUID(ownerUID) {}
    const filePermission &permission() const { return _permission; }
    ffsuid_t ownerUID() const { return _ownerUID; }
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
};

#endif // FILE_H
