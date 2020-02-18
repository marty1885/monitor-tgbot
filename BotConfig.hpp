#pragma once

#include <vector>
#include <string>

struct MonitorConfig
{
	std::vector<std::string> services;
	int interval = 300;
};

struct BotConfig
{
	std::string token;
	std::string db_path;
	std::string password;
};

struct Config
{
	MonitorConfig monitor;
	BotConfig bot;

};

Config parse_and_load(int argc, char** argv);