#ifndef _IRC_H
#define _IRC_H

#include <string>
#include <vector>
#include <list>
#include "sock.h"
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>  /* the Lua interpreter */
}
#include "LuaBridge/LuaBridge.h"

class client;
extern std::vector<std::string> split(std::string const&, char);
struct irccmdprefix {
  void Parse(std::string data) {
    if (data == "")
      return;

    prefix = data.substr(1, data.find(" ") - 1);
    std::vector<std::string> tokens;

    if (prefix.find("@") != std::string::npos) {
      tokens = split(prefix, '@');
      nick = tokens.at(0);
      host = tokens.at(1);
    }
    if (nick != "" && nick.find("!") != std::string::npos) {
      tokens = split(nick, '!');
      nick = tokens.at(0);
      user = tokens.at(1);
    }
  };

  std::string prefix;
  std::string nick;
  std::string user;
  std::string host;
};

struct ircmsg {
  ircmsg();
  ircmsg(std::string cmd, irccmdprefix p, std::vector<std::string> params) :
    command(cmd), prefix(p), parameters(params) {};

  std::string command;
  irccmdprefix prefix;
  std::vector<std::string> parameters;
};

struct cmdhook {
  cmdhook(lua_State *L) : function(L) {};

  std::string command;
  luabridge::LuaRef function;
};

class client {
public:
  client() : _debug(false) {};

  bool InitSocket();
  bool Connect(const char* /*host*/, int /*port*/);
  void Disconnect();
  bool Connected() { return _socket.Connected(); };

  bool SendIRC(std::string /*data*/);
  bool Login(std::string /*nick*/, std::string /*user*/);
  void ReceiveData();
  void HookIRCCommand(std::string /*command*/, luabridge::LuaRef);
  void Parse(std::string /*data*/);
  void Debug(bool debug) { _debug = debug; };

private:
  void HandleCommand(ircmsg /*message*/);
  void CallHook(std::string /*command*/, ircmsg /*message*/);

  sock _socket;
  std::list<cmdhook> _hooks;
  std::string _nick;
  std::string _user;
  bool _debug;
};

#endif
