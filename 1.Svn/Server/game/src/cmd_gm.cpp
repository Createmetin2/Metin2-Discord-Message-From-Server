///Add
#if defined(__BL_DISCORD_MESSAGE__)
#include "discord.hpp"

ACMD(do_discord)
{
	char arg1[256], arg2[256], arg3[256];

	const char* line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: /discord <token index> <channel index> <message>");
		return;
	}

	BYTE bTokenIndex;
	str_to_number(bTokenIndex, arg1);
	
	BYTE bChannelIndex;
	str_to_number(bChannelIndex, arg2);

#define DISCORD_STRING(x) #x

	if (bTokenIndex >= EDiscordTokens::DISCORD_TOKEN_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Invalid Token Index");
		ch->ChatPacket(CHAT_TYPE_INFO, "Available Tokens: ");

		BYTE bIndex{};
		ch->ChatPacket(CHAT_TYPE_INFO, "%d: %s", bIndex++, DISCORD_STRING(DISCORD_TOKEN_ADMIN_MALI));
		ch->ChatPacket(CHAT_TYPE_INFO, "%d: %s", bIndex++, DISCORD_STRING(DISCORD_TOKEN_GM_BLACK));
		ch->ChatPacket(CHAT_TYPE_INFO, "%d: %s", bIndex++, DISCORD_STRING(DISCORD_TOKEN_BOT_TS));

		return;
	}
	
	if (bChannelIndex >= EDiscordChannels::DISCORD_CHANNEL_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Invalid Channel Index");
		ch->ChatPacket(CHAT_TYPE_INFO, "Available Channels: ");

		BYTE bIndex{};
		ch->ChatPacket(CHAT_TYPE_INFO, "%d: %s", bIndex++, DISCORD_STRING(DISCORD_CHANNEL_BLACK_GENERAL));
		ch->ChatPacket(CHAT_TYPE_INFO, "%d: %s", bIndex++, DISCORD_STRING(DISCORD_CHANNEL_BLACK_ANNOUNCEMENTS));

		return;
	}

	CDiscordManager::Instance().SendText(bTokenIndex, bChannelIndex, line);
}
#endif