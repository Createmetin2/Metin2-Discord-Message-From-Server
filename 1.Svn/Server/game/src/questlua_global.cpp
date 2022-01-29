///Add
#if defined(__BL_DISCORD_MESSAGE__)
#include "discord.hpp"
#endif

//Find
	int _get_locale_base_path( lua_State* L )
	{
		...
	}

///Add
#if defined(__BL_DISCORD_MESSAGE__)
	int _discord_message(lua_State* L)
	{
		if (!lua_isnumber(L, 1)) // Token Index
			return 0;

		if (!lua_isnumber(L, 2)) // Channel Index
			return 0;

		if (!lua_isstring(L, 3)) // Message
			return 0;

		const BYTE bTokenIndex = static_cast<BYTE>(lua_tonumber(L, 1));
		const BYTE bChannelIndex = static_cast<BYTE>(lua_tonumber(L, 2));
		const char* message = lua_tostring(L, 3);

		CDiscordManager::Instance().SendText(bTokenIndex, bChannelIndex, message);

		return 0;
	}
#endif

//Find
			{	"get_locale_base_path",			_get_locale_base_path			},

///Add
#if defined(__BL_DISCORD_MESSAGE__)
			{ 	"discord_message",				_discord_message				},
#endif