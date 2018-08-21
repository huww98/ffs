#if !defined(FILE_H)
#define FILE_H

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>

#include "user.h"

using blockNum_t = std::uint32_t;
extern const std::filesystem::path blocksDirPath;

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

class permissionAccessor
{
  public:
    virtual bool get(permission &) const = 0;
    virtual void set(permission &, bool) const = 0;

    static const std::map<char, std::reference_wrapper<permissionAccessor>> all;
};
class readPermissionAccessor : public permissionAccessor
{
  public:
    bool get(permission &p) const override { return p.read(); }
    void set(permission &p, bool b) const override { p.read(b); }

    static readPermissionAccessor accessor;
};
class writePermissionAccessor : public permissionAccessor
{
  public:
    bool get(permission &p) const override { return p.write(); }
    void set(permission &p, bool b) const override { p.write(b); }

    static writePermissionAccessor accessor;
};

class filePermission
{
  private:
    linuxStylePermission<3> userPermission;
    linuxStylePermission<0> otherPermission;

  public:
    filePermission(std::uint8_t &data) : userPermission(data), otherPermission(data) {}
    permission &user() { return this->userPermission; }
    const permission &user() const { return this->userPermission; }
    permission &other() { return this->otherPermission; }
    const permission &other() const { return this->otherPermission; }
};

class fileMetadataPresistent;
class fileMetadata
{
  private:
    uint8_t _attributeData = 0;
    ffsuid_t _ownerUID;

    constexpr static uint8_t isDirectoryMask = 1 << 7;

    void ensurePermission(ffsuid_t currentUid, const permissionAccessor& accessor);

  public:
    fileMetadata(ffsuid_t ownerUID, bool isDirectory = false);
    fileMetadata(fileMetadataPresistent);
    filePermission permission() { return filePermission(_attributeData); }
    ffsuid_t ownerUID() const { return _ownerUID; }
    void ownerUID(ffsuid_t uid) { _ownerUID = uid; }
    bool isDirectory() const { return _attributeData & isDirectoryMask; }
    auto attributeData() const { return _attributeData; }

    void ensureRead(ffsuid_t currentUid);
    void ensureWrite(ffsuid_t currentUid);
    void ensureOwnership(ffsuid_t currentUid);
};

class file
{
  private:
    fileMetadata _metadata;
    blockNum_t blockNum;
    file(fileMetadata metadata, blockNum_t blockNum) : _metadata(metadata), blockNum(blockNum) {}

  public:
    static std::filesystem::path path(blockNum_t blockNum);
    static file create(fileMetadata metadata, blockNum_t blockNum);
    static file open(blockNum_t blockNum);

    std::filesystem::path path() { return path(this->blockNum); }
    fileMetadata &metadata() { return _metadata; }
    std::fstream openStream();
    std::size_t size();
    void remove();
    void truncate();
    void saveMetadata();
};

#endif // FILE_H
