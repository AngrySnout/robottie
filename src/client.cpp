#include <iostream>
#include <algorithm>
#include "sock.h"
#include "client.h"

using namespace luabridge;
std::vector<std::string> split(std::string const& text, char sep) {
  std::vector<std::string> tokens;
  size_t start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));
  return tokens;
}

bool client::InitSocket() {
  return _socket.Init();
}

bool client::Connect(const char* host, int port) {
  return _socket.Connect(host, port);
}

void client::Disconnect() {
  _socket.Disconnect();
}

bool client::SendIRC(std::string data) {
  data.append("\n");
  return _socket.SendData(data.c_str());
}

bool client::Login(std::string nick, std::string user) {
  _nick = nick;
  _user = user;

  if (SendIRC("HELLO"))
    if (SendIRC("NICK " + nick))
      if (SendIRC("USER " + user + " 8 * :Cpp IRC Client"))
        return true;

  return false;
}

void client::ReceiveData() {
  std::string buffer = _socket.ReceiveData();

  std::string line;
  std::istringstream iss(buffer);
  while(getline(iss, line)) {
    if (line.find("\r") != std::string::npos)
      line = line.substr(0, line.size() - 1);
    Parse(line);
  }
}

void client::Parse(std::string data) {
  std::string original(data);
  irccmdprefix cmdPrefix;

  // if command has prefix
  if (data.substr(0, 1) == ":") {
    cmdPrefix.Parse(data);
    data = data.substr(data.find(" ") + 1);
  }

  std::string command = data.substr(0, data.find(" "));
  std::transform(command.begin(), command.end(), command.begin(), towupper);
  if (data.find(" ") != std::string::npos)
    data = data.substr(data.find(" ") + 1);
  else
    data = "";

  std::vector<std::string> parameters;

  if (data != "") {
    if (data.substr(0, 1) == ":")
      parameters.push_back(data.substr(1));
    else {
      size_t pos1 = 0, pos2;
      while ((pos2 = data.find(" ", pos1)) != std::string::npos) {
        parameters.push_back(data.substr(pos1, pos2 - pos1));
        pos1 = pos2 + 1;
        if (data.substr(pos1, 1) == ":") {
          parameters.push_back(data.substr(pos1 + 1));
          break;
        }
      }
      if (parameters.empty())
        parameters.push_back(data);
    }
  }

  if (command == "ERROR") {
    std::cout << original << std::endl;
    Disconnect();
    return;
  }

  if (command == "PING") {
    std::cout << "Ping? Pong!" << std::endl;
    SendIRC("PONG :" + parameters.at(0));
    ircmsg ircMessage("PING", cmdPrefix, parameters);
    CallHook(command, ircMessage);
    return;
  }

  ircmsg ircMessage(command, cmdPrefix, parameters);

  if (_debug)
    std::cout << original << std::endl;

  // Try to call hook (if any matches)
  if(ircMessage.parameters.at(ircMessage.parameters.size() - 1)[0] == '\001') {command = "CTCP";}
  CallHook(command, ircMessage);
}

extern lua_State *get_ls();
void client::HookIRCCommand(std::string command, LuaRef callback) {
  cmdhook hook(get_ls());

  hook.command = command;
  hook.function = callback;

  _hooks.push_back(hook);
}

void client::CallHook(std::string command, ircmsg message) {
  if (_hooks.empty())
    return;

  for (std::list<cmdhook>::const_iterator itr = _hooks.begin(); itr != _hooks.end(); ++itr) {
    if (itr->command == command) {
      (itr->function)(this, message);
      break;
    }
  }
}