#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include <set>

struct BotDatabase
{
	BotDatabase(const std::string& path)
		: db(path, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE)
	{
		// Build the database if it does not exist
		if(db.tableExists("users") == false) {
			SQLite::Transaction transaction(db);
			db.exec(R"(CREATE TABLE users (uid INTEGER NOT NULL PRIMARY KEY);)");
			transaction.commit();
		}
	}

	std::set<int64_t> allUserID()
	{
		std::set<int64_t> user_list;
		SQLite::Statement query(db, "SELECT uid FROM users;");
		while(query.executeStep())
			user_list.insert(int64_t(query.getColumn(0)));
		return user_list;
	}

	void addUser(int64_t uid)
	{
		SQLite::Transaction transaction(db);
		db.exec(R"(INSERT INTO users VALUES ()"+std::to_string(uid)+");");
		transaction.commit();
	}
	SQLite::Database db;
};