///Add
#if defined(__BL_DISCORD_MESSAGE__)
#include "discord.hpp"
#endif

#if defined(__BL_DISCORD_MESSAGE__)
			CDiscordManager::Instance().SendText(EDiscordTokens::DISCORD_TOKEN_BOT_TS, 
				EDiscordChannels::DISCORD_CHANNEL_BLACK_ANNOUNCEMENTS, "**Hello World**");
#endif