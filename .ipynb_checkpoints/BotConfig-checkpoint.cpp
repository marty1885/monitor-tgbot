#include "BotConfig.hpp"
#include "PasswordHash.hpp"

#include <unistd.h>

#include <filesystem>

#include <cxxopts.hpp>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace fs = std::filesystem;

template <typename T>
static void set_if_comtains(const cxxopts::ParseResult& args, const std::string& name, T& val)
{
	static_assert(std::is_const_v<T> == false);

	if(args.count(name) == 0) 
		return;
	
	using ValueType = typename std::decay_t<T>;
	const auto& value = args[name];
	val = value.template as<ValueType>();
}

Config parse_and_load(int argc, char** argv)
{
	const std::string home = getenv("HOME"); // C/C++ does not respect the ~ symbol
	const std::string default_config_dir = home+"/.config/monitor-tgbot";
	const std::string default_config_file = default_config_dir+"/monitorbot_config.lua";
	// Parse the command line otions
	cxxopts::Options options(argv[0], "A server managment Telegram bot.");
	options.add_options()
		("f,config-file", "The location of the config file.", cxxopts::value<std::string>()->default_value(""))
		("t,token", "The token of your Telegram bot")
		("dbpath", "The SQLIte DB which stores information")
		("interval", "How long do we mointor the service status once.")
		("services", "The services to monitor.",  cxxopts::value<std::vector<std::string>>())
		("gen-password", "The program will ask you for a password and hash it.")
		("password", "(Recommend not to use) set the password used.")
		("h,help", "Print usage");
	auto args = options.parse(argc, argv);
	if (args.count("help")) {
		std::cout << options.help() << std::endl;
		exit(0);
	}
	else if(args.count("gen-password")) {
		std::string password(getpass("Password:"));
		if(password != "")
			std::cout << "\n" << gen_password(password) << std::endl;
		else
			std::cerr << "Error: password cannot be empty";
		exit(0);
	}

	// Make sure the default configuration folder exists
	if(fs::exists(default_config_dir) == false) 
		fs::create_directory(default_config_dir);

	std::string config_file = args["config-file"].as<std::string>();

	// Load from config file if needed
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	// Make the Lua binding
	lua.new_usertype<MonitorConfig>("MonitorConfig"
		, "services", &MonitorConfig::services
		, "interval", &MonitorConfig::interval);
	lua.new_usertype<BotConfig>("BotConfig"
		, "token", &BotConfig::token
		, "db_path", &BotConfig::db_path
		, "password", &BotConfig::password);
	lua.new_usertype<Config>("Config"
		, "bot", &Config::bot
		, "monitor", &Config::monitor);
	lua.set("c", Config());
	if(config_file.empty()) {
		// Check if the file exists
		if(fs::exists(default_config_file))
			lua.script_file(default_config_file);
	}
	else
		lua.script_file(config_file);
	Config c = lua["c"];
	
	// Overrite options from commandline
	set_if_comtains(args, "token", c.bot.token);
	set_if_comtains(args, "dbpath", c.bot.db_path);
	set_if_comtains(args, "password", c.bot.password);

	set_if_comtains(args, "interval", c.monitor.interval);
	set_if_comtains(args, "services", c.monitor.services);

	if(c.bot.db_path.empty())
		c.bot.db_path = default_config_dir+"/monitorbot.sqlite";
	
	return c;
}