#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>

#include <set>
#include <stdexcept>

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
		SQLite::Statement query(db, "INSERT INTO users VALUES (?)");
		SQLite::bind(query, uid);
		if(query.exec() == false)
			throw std::runtime_error("Failed to add user into user list");
		transaction.commit();
	}
	SQLite::Database db;
};
