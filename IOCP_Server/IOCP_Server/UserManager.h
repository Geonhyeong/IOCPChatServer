#pragma once

class User;
class UserManager
{
private:
	UserManager();
	~UserManager();

public:
	static UserManager& GetInstance();

	int32 Add(shared_ptr<User> newUser);
	void Remove(int32 id);
	
	int32 GetUserCount() { return (int32)_users.size(); }
	shared_ptr<User> GetUserRefById(int32 id);
	
private:
	USE_LOCK;
	atomic<int32> _userId = 1;
	unordered_map<int32, shared_ptr<User>> _users;
};