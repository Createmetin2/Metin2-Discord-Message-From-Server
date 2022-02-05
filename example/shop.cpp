#if defined(__BL_DISCORD_MESSAGE__)
#include "discord.hpp"
#endif

// tested with english proto
#if defined(__BL_DISCORD_MESSAGE__)
		CDiscordManager::Instance().SendPrivateMessageText(EDiscordTokens::DISCORD_TOKEN_BOT_TS, 
			123456789123456789, "**%s** bought by: **%s**. You earn **%lu** yang.", item->GetName(), ch->GetName(), dwPrice);
#endif