#include <memory>
#include <deque>
#include <map>
#include <string>

#include "file.h"

class changeTarget
{
  public:
    virtual permission &targetPermission(filePermission &) = 0;

    static const std::map<char, std::reference_wrapper<changeTarget>> all;
};
class userTarget : public changeTarget
{
  public:
    permission &targetPermission(filePermission &p) override { return p.user(); }

    static userTarget instance;
};
class otherTarget : public changeTarget
{
  public:
    permission &targetPermission(filePermission &p) override { return p.other(); }

    static otherTarget instance;
};

class permissionAccessor
{
  public:
    virtual bool get(permission &) = 0;
    virtual void set(permission &, bool) = 0;

    static const std::map<char, std::reference_wrapper<permissionAccessor>> all;
};
class readPermissionAccessor : public permissionAccessor
{
  public:
    bool get(permission &p) override { return p.read(); }
    void set(permission &p, bool b) override { p.read(b); }

    static readPermissionAccessor accessor;
};
class writePermissionAccessor : public permissionAccessor
{
  public:
    bool get(permission &p) override { return p.write(); }
    void set(permission &p, bool b) override { p.write(b); }

    static writePermissionAccessor accessor;
};
class modeChangeOperation;
class modeChange
{
  public:
    using targets_t = std::deque<std::reference_wrapper<changeTarget>>;
    using permissions_t = std::deque<std::reference_wrapper<permissionAccessor>>;
    using operation_ptr = std::reference_wrapper<modeChangeOperation>;

  private:
    targets_t targets;
    permissions_t permissions;
    const operation_ptr operation;

  public:
    modeChange(targets_t& ts, permissions_t& ps, const operation_ptr &po)
        : targets(ts), permissions(ps), operation(po) {}
    void apply(filePermission &);
};

class modeChangeOperation
{
  public:
    virtual void apply(permission &, modeChange::permissions_t&) = 0;
    static const std::map<char, std::reference_wrapper<modeChangeOperation>> all;
};
class addChangeOperation : public modeChangeOperation
{
  public:
    void apply(permission &, modeChange::permissions_t&) override;
    static addChangeOperation instance;
};
class removeChangeOperation : public modeChangeOperation
{
  public:
    void apply(permission &, modeChange::permissions_t&) override;
    static removeChangeOperation instance;
};
class setChangeOperation : public modeChangeOperation
{
  public:
    void apply(permission &, modeChange::permissions_t&) override;
    static setChangeOperation instance;
};

class modeChanges
{
  private:
    std::deque<modeChange> changes;

  public:
    modeChanges(std::string);
    void apply(filePermission &);
};
