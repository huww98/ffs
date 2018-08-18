#include "mode_change.h"

#include <regex>
#include <deque>

using namespace std;

userTarget userTarget::instance;
otherTarget otherTarget::instance;

readPermissionAccessor readPermissionAccessor::accessor;
writePermissionAccessor writePermissionAccessor::accessor;

addChangeOperation addChangeOperation::instance;
removeChangeOperation removeChangeOperation::instance;
setChangeOperation setChangeOperation::instance;

const map<char, reference_wrapper<changeTarget>>
    changeTarget::all = {
        {'u', userTarget::instance},
        {'o', otherTarget::instance}};

const map<char, reference_wrapper<permissionAccessor>> permissionAccessor::all = {
    {'r', readPermissionAccessor::accessor},
    {'w', writePermissionAccessor::accessor}};

const map<char, reference_wrapper<modeChangeOperation>> modeChangeOperation::all = {
    {'+', addChangeOperation::instance},
    {'-', removeChangeOperation::instance},
    {'=', setChangeOperation::instance}};

void addChangeOperation::apply(permission &target, modeChange::permissions_t &permissions)
{
    for (auto &a : permissions)
    {
        a.get().set(target, true);
    }
}

void removeChangeOperation::apply(permission &target, modeChange::permissions_t &permissions)
{
    for (auto &a : permissions)
    {
        a.get().set(target, false);
    }
}

void setChangeOperation::apply(permission &target, modeChange::permissions_t &permissions)
{
    for (auto &a : permissionAccessor::all)
    {
        a.second.get().set(target, false);
    }
    for (auto &a : permissions)
    {
        a.get().set(target, true);
    }
}

void modeChange::apply(filePermission &p)
{
    for (auto &t : this->targets)
    {
        auto &target = t.get().targetPermission(p);
        this->operation.get().apply(target, this->permissions);
    }
}

void modeChanges::apply(filePermission &p)
{
    for (auto &c : this->changes)
    {
        c.apply(p);
    }
}

void addAllTarget(modeChange::targets_t &targets)
{
    for (auto &t : changeTarget::all)
    {
        targets.push_back(t.second);
    }
}

modeChanges::modeChanges(string s)
{
    stringstream inputss(s);
    string mode;
    while (getline(inputss, mode, ','))
    {
        static regex modePattern("([uoa]*)([+-=])([rw]*)");
        smatch match;
        regex_match(mode, match, modePattern); // TODO: Workaround from https://github.com/Microsoft/vscode-cpptools/issues/2328 applied
        if (match.empty())
        {
            throw runtime_error("无法识别的权限设置");
        }

        modeChange::targets_t targets;

        for (auto c : match[1].str())
        {
            if(c == 'a')
            {
                addAllTarget(targets);
            }
            else
            {
                targets.push_back(changeTarget::all.at(c));
            }
        }
        if (targets.size() == 0)
        {
            addAllTarget(targets);
        }

        char c = match[2].str()[0];
        auto &op = modeChangeOperation::all.at(c);

        modeChange::permissions_t permissions;
        for (auto c : match[3].str())
        {
            permissions.push_back(permissionAccessor::all.at(c));
        }

        this->changes.push_back(modeChange(targets, permissions, op));
    }
}
