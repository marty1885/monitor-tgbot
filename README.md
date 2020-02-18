# monitor-tgbot

This a light weight [Telegram](https://telegram.org/) bot that watches your systemd enabled Linux machine for service failures.

## Dependency

* [tbgot-cpp](https://github.com/reo7sp/tgbot-cpp)
* [Lua](https://www.lua.org/)
* [Sol2](https://github.com/ThePhD/sol2)
* [systemd](https://freedesktop.org/wiki/Software/systemd/)
* [SQLiteCpp](https://github.com/SRombauts/SQLiteCpp)
* A C++17 capable compiler

## How to build
```
git clone https://github.com/marty1885/monitor-tgbot
cd monitor-tgbot
mkdir build
cd build
cmake ..
make
```

### CMake parameters
|Name          |Description                                |
|--------------|-------------------------------------------|
|LUA_VERSION   |The Lua version to use (defaults to 5.3)   |

## How to use

Before using the bot. A few configuations have to be provided by the user. First, you'll need to provide the token that the bot is attached to. If you don't have one, follow [Telegram's guide](https://core.telegram.org/bots#3-how-do-i-create-a-bot) to create one. With your token, you'll need to put the token into a confuration file. The bot will attempt to load `~/.config/monitor-tgbot/monitorbot_config.lua` as it's configuration. You may also make one yourself then ask the bot to load it using the `-f` parameter.

```Lua
c.bot.token = "132648513:haeohNLANDKban21ja8coanLANlnca64530" -- this is a made up token
```

Then you'll need to generate a password for the bot. The password authenticaes an account to be added to the list of brodcat. Then you will be notified when services goes down.

```bash
❯ cd /path/to/monitor-tgbot/build
❯ ./monitor-tgbot --gen-password
Password:

SHA256:yrmU1VSZtYkC6Oli:a1d9f71497f779b467d4b9b3519b9de06a87725e3487917a89c97bbcdb90517e
```

Don't worry. I generated the hash by a random monkey pressing randomly on my keyboard. Now put the password into the confuguration file.

```Lua
c.bot.password = "SHA256:yrmU1VSZtYkC6Oli:a1d9f71497f779b467d4b9b3519b9de06a87725e3487917a89c97bbcdb90517e"
```

Then just specsify which services you want the bot monitor and how often the bot should check.

```Lua
c.monitor.services = {"tor", "sshd"}
c.monitor.interval = 300
```

There are other configurations you can do or overwrite the values via command line. See `monitorbot_config.lua` and `./monitor-tgbot` for more information`

