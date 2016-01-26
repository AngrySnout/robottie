#include <iostream>
#include <signal.h>
#include <cstdlib>
#include <map>
#include <algorithm>
#include "client.h"
#include "tracker.h"

static lua_State * L = NULL;
lua_State *get_ls() {return L;}

void report_errors(lua_State *L, int status) {
  if(status!=0) {
    std::cerr << "-- " << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1); // remove error message
  }
}

int main(int argc, char* argv[]) {
  L = luaL_newstate();
  luaL_openlibs (L);
  luabridge::getGlobalNamespace(L)
    .beginNamespace("IRC")
      .beginClass<client> ("Client")
        .addConstructor <void (*) ()>()
        .addFunction("Send", &client::SendIRC)
        .addFunction("Hook", &client::HookIRCCommand)
        .addFunction("Init", &client::InitSocket)
        .addFunction("Connect", &client::Connect)
        .addFunction("Login", &client::Login)
        .addFunction("Connected", &client::Connected)
        .addFunction("Receive", &client::ReceiveData)
      .endClass()
      .beginClass<std::string>("MessageParameterType")
        .addFunction("c_str", &std::string::c_str)
      .endClass()
      .beginClass<std::vector<std::string>>("MessageParameterVector")
        .addFunction("size", &std::vector<std::string>::size) 
        .addFunction<std::vector<std::string>::reference (std::vector<std::string>::*)(std::vector<std::string>::size_type)>("at", &std::vector<std::string>::at)
      .endClass()
      .beginClass<ircmsg>("Message")
        .addData("Command", &ircmsg::command)
        .addData("Prefix", &ircmsg::prefix)
        .addData("Parameters", &ircmsg::parameters)
      .endClass()
      .beginClass<irccmdprefix>("Prefix")
        .addData("Prefix", &irccmdprefix::prefix)
        .addData("Nick", &irccmdprefix::nick)
        .addData("User", &irccmdprefix::user)
        .addData("Host", &irccmdprefix::host)
      .endClass()
    .endNamespace()
    .beginNamespace("Tracker")
	  .addFunction("GetSeggestion", &tracker::getSuggestion);
  
  int s = luaL_loadfile(L, "script.lua");

  if(s==0) {
    // execute Lua program
    s = lua_pcall(L, 0, LUA_MULTRET, 0);
  }

  report_errors(L, s);
}
