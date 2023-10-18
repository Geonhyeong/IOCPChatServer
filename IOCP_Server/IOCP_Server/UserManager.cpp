#include "pch.h"
#include "UserManager.h"

UserManager::UserManager()
{
    SLog(L"UserManager Singleton Instance Generated.");
}

UserManager::~UserManager()
{
}

UserManager& UserManager::GetInstance()
{
    static UserManager* instance = new UserManager();
    return *instance;
}

int32 UserManager::Add(shared_ptr<User> newUser)
{
    WRITE_LOCK;
    int32 userId = _userId.fetch_add(1);
    
    _users.insert(make_pair(userId, newUser));
    return userId;
}

void UserManager::Remove(int32 id)
{
    WRITE_LOCK;

    if (_users.find(id) != _users.end())
        _users.erase(id);
}

shared_ptr<User> UserManager::GetUserRefById(int32 id)
{
    READ_LOCK;

    auto it = _users.find(id);
    if (it == _users.end())
        return nullptr;

    return it->second;
}
