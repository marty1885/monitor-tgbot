#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include <set>
#include <thread>

#include <cstdlib>

#include <tgbot/tgbot.h>

#include "BotDatabase.hpp"
#include "BotConfig.hpp"
#include "PasswordHash.hpp"

using namespace std;
using namespace TgBot;

bool service_alive(std::string service)
{
	return system(("systemctl is-active --quiet " + service).c_str()) == 0;
}

void send_message(Bot& bot, std::set<int64_t>& user_list, const std::string& message)
{
	for(auto uid : user_list) {
		bot.getApi().sendMessage(uid, message);
	}
}

template <typename ForwardIterator>
std::string join(ForwardIterator begin, ForwardIterator end, const std::string delim)
{
	std::string res;
	for(auto it = begin; it != end; it++)
		res += *it + delim;
	res.resize(res.size()-delim.size());
	return res;
}


void monitor(Bot& bot, std::set<int64_t>& user_list, const MonitorConfig& conf)
{
	std::set<std::string> known_down_services;
	while(true) {
		std::vector<std::string> down_services;
		const auto services = conf.services;
		for(const auto& service : services) {
			if(service_alive(service) == false) {
				if(known_down_services.find(service) == known_down_services.end())
					down_services.push_back(service);
				known_down_services.insert(service);
			}
			else {
				auto it = known_down_services.find(service);
				if(it != known_down_services.end())
					known_down_services.erase(it);
			}
		}

        if(down_services.empty() == false) {
    		std::string service_str = join(down_services.begin(), down_services.end(), ",");
	    	send_message(bot, user_list, "Hey, " + service_str + (down_services.size() == 1 ? " is" : " are") 
		    	+ " down. You might want to check that.");
        }

		std::this_thread::sleep_for(std::chrono::seconds(conf.interval));
	}
}

int main(int argc, char** argv)
{
	const Config c = parse_and_load(argc, argv);
	if(c.bot.token.empty())
		throw std::runtime_error("Error: Bot toeken have not been set. Please set it via --token or in the config file");

	BotDatabase db(c.bot.db_path);
	std::set<int64_t> user_list = db.allUserID();

	// Callbacks for handling events
	Bot bot(c.bot.token);
	bot.getEvents().onCommand("start", [&bot](Message::Ptr message) {
		bot.getApi().sendMessage(message->chat->id, "Hi! This is a server maniger bot.\n"
			"Please reply with the password so you can be added to the message chain.");
	});

	bot.getEvents().onCommand("ping", [&bot](Message::Ptr message) {
		bot.getApi().sendMessage(message->chat->id, "Pong!");
	});

	bot.getEvents().onCommand("monitered_services", [&bot, &c, &user_list](Message::Ptr message) {
		if(user_list.find(message->chat->id) == user_list.end())
			return;
		std::string res = "Currently ";
		for(const auto& service : c.monitor.services)
			res += service + ", ";
		res.resize(res.size()-2);
		res += " are being monitored";
		bot.getApi().sendMessage(message->chat->id, res);
	});

	bot.getEvents().onAnyMessage([&bot, &user_list, &db, &c](Message::Ptr message) {
		auto uid = message->chat->id;
		if(c.bot.password == "") {
			bot.getApi().sendMessage(uid, "Password have not been set. Please set the password before trying activating the bot.");
			return;
		}
		else if(StringTools::startsWith(message->text, "/")) {
			return;
		}

		auto parts = StringTools::split(c.bot.password, ':');
		if(parts.size() != 3)
			throw std::runtime_error("Error: Password hash format error");
		std::string salt = parts[1];
		std::string hash = parts[2];
		if(user_list.find(uid) == user_list.end() &&
			sha256(salt+message->text) == hash) {
			db.addUser(uid);
			user_list.insert(uid);
			bot.getApi().sendMessage(uid, "Hi! You have been added to the notifcation list!\n"
				"You'll be notified when something goes wrong");
		}
	});

	// Start the monitoring thread
	std::thread monitor_thread([&](){monitor(bot, user_list, c.monitor);});
	monitor_thread.detach();

	// Run the bot
	try {
		bot.getApi().deleteWebhook();
		TgLongPoll longPoll(bot);
		while (true) 
			longPoll.start();
	}
	catch (std::exception& e) {
		printf("error: %s\n", e.what());
	}
}
