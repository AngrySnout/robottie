io.output(io.stdout)

Client = IRC.Client()

local test = function(f,s) if f then print(s) else return true end end

if test(Client:Init(), "Socket initialized") then return end
if test(Client:Connect("Burstfire.UK.EU.GameSurge.net", 6667), "Connected to IRC; Logging in") then return end
if test(Client:Login("robottie", "robottie"), "Logged in") then return end

function table.contains(table, element)
  for key, value in pairs(table) do
    if key == element then
      return true
    end
  end
  return false
end
function split(p,d)
  local t, ll
  t={}
  ll=0
  if(#p == 1) then return {p} end
    while true do
      l=string.find(p,d,ll,true)
      if l~=nil then
        table.insert(t, string.sub(p,ll,l-1))
        ll=l+1
      else
        table.insert(t, string.sub(p,ll))
        break
      end
    end
  return t
end
function string.slice(array, S, E)
  local result = {}
  local length = #array
  S = S or 1
  E = E or length
  if E < 0 then
    E = length + E + 1
  elseif E > length then
    E = length
  end
  if S < 1 or S > length then
    return {}
  end
  local i = 1
  for j = S, E do
    result[i] = array[j]
    i = i + 1
  end
  return result
end

local channels = {}
local sendType = function(t,n,s,...)
  local msg = "requests a " .. t
  if s and s ~= "" then msg = msg .. " at " .. s end
  if #{...} > 0 and ({...})[1] ~= "" then msg = msg .. " (" .. table.concat({...}) .. ")" end
  for i,c in pairs(channels) do
    Client:Send("PRIVMSG " .. c .. " :" .. n .. " " .. msg .. "; join #sauercom to respond")
  end
end
local commands = {
  duel = function(user, channel, server, ...) 
    sendType("duel", user.Nick:c_str(), server, ...)
  end,
  clanwar = function(user, channel, server, ...) 
    sendType("clanwar", user.Nick:c_str(), server, ...)
  end,
  mix = function(user, channel, server, ...) 
    sendType("mix", user.Nick:c_str(), server, ...)
  end,
  help = function(user)
    Client:Send("NOTICE " .. user.Nick:c_str() .. " :Commands: .duel .clanwar .mix | Usage: .command <server> [list of comments]")
  end
}
function is_valid_command(cmd)
  return table.contains(commands, cmd)
end

local pcount = false
local hooks = {
  PRIVMSG = function(self, msg)
    parts = split(msg.Parameters:at(1):c_str(), " ")

    if string.sub(parts[1], 1, 1) == "." then
      local cmd, args = string.gsub(parts[1], ".", "", 1), string.slice(parts, 2)

      if is_valid_command(cmd) then
        commands[cmd](msg.Prefix, msg.Parameters:at(0):c_str(), table.unpack(args))
      else
        Client:Send("PRIVMSG " .. msg.Parameters:at(0):c_str() .." :red<Invalid command>")
      end
    end
  end,
  --[[PING = function() if pcount then return end; pcount = true; for k,i in pairs({""}) do Client:Send("JOIN #" .. i) end end,]]
  CTCP = function(self, msg) Client:Send("NOTICE " .. msg.Prefix.Nick:c_str() .. " :\001VERSION C++ Lua bot by gear4 \001") end,
  JOIN = function(self, msg) if msg.Prefix.Nick:c_str() == "robottie" then print(msg.Parameters:at(0):c_str()); 
channels[#channels + 1] = 
msg.Parameters:at(0):c_str() end end
}
for h,c in pairs(hooks) do
  Client:Hook(h, c)
end

while true do
  Client:Receive()
end
