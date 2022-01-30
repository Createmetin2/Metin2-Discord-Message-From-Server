/*
* Author: blackdragonx61 / Mali
* 29.01.2022
* Discord
*/

#include "stdafx.h"
#include "discord.hpp"

#include "../common/tables.h"
#include <cpr/cpr.h>

/// <Special>
	/// Bold: **message**
	/// Italic: *message*
	/// Strikethrough: ~~message~~
	/// Quote: > message
	/// Code: `message`
	/// Spoiler: ||message||
	/// @user: <@userid>
	/// #channel: <#channelid>
/// </Special>

/// <IMPORTANT!>
	/// If the token belongs to the bot account
	/// Add "Bot " to the beginning of the token!
	/// Just like in the example(DISCORD_TOKEN_BOT_TS).
/// </IMPORTANT!>

/****************Editable****************/
constexpr const char* arrTokens[EDiscordTokens::DISCORD_TOKEN_MAX] =
{
	"TOKENHERE", // DISCORD_TOKEN_ADMIN_MALI
	"TOKENHERE", // DISCORD_TOKEN_GM_BLACK
	"Bot TOKENHERE", // DISCORD_TOKEN_BOT_TS
};

constexpr std::uint64_t arrChannels[EDiscordChannels::DISCORD_CHANNEL_MAX] =
{
	123456789123456789, // DISCORD_CHANNEL_BLACK_GENERAL
	123456789123456789, // DISCORD_CHANNEL_BLACK_ANNOUNCEMENTS
};
/****************Editable****************/

CDiscordManager::CDiscordManager() :
	thread_{ std::unique_ptr<std::thread>(new std::thread(&CDiscordManager::__Loop, this)) }
{
}

CDiscordManager::~CDiscordManager()
{
	SendText(255, 255, ""); // out signal
	thread_->join();
};

void CDiscordManager::SendText(BYTE bTokenIndex, BYTE bChannelIndex, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	const int len = vsnprintf(nullptr, 0, format, args) + 1;
	if (len > 0)
	{
		const size_t size = static_cast<size_t>(len);
		auto buf = std::unique_ptr<char[]>(new char[size]);
		vsnprintf(buf.get(), size, format, args);

		auto data = std::unique_ptr<SDicordMessage>(new SDicordMessage{});
		data->bTokenIndex = bTokenIndex;
		data->bChannelIndex = bChannelIndex;
		data->sMessage = std::string(buf.get(), buf.get() + size - 1);
		
		std::lock_guard<std::mutex> l(mutex_);
		q_.push(std::move(data));
		cv_.notify_one();
	}

	va_end(args);
}

void CDiscordManager::__SendDiscordMessage(const char* szToken, std::uint64_t uChannelID, const std::string& sMessage)
{
	try {
		const std::string endpoint = "https://discord.com/api/v9/channels/" + std::to_string(uChannelID) + "/messages";
		cpr::Response x = cpr::Post(cpr::Url{ endpoint },
			cpr::Header{ {"Content-Type", "application/json"}, {"Authorization", szToken} },
			cpr::Body{ "{\"content\":\"" + sMessage + "\"}" });
	}
	catch (const std::exception& ex) {
		sys_err("%s", ex.what());
	}
}

void CDiscordManager::__Loop()
{
	while (true)
	{
		std::unique_lock<std::mutex> l(mutex_);
		cv_.wait(l, [&] { return !q_.empty(); });
		
		auto data{ std::move(q_.front()) };
		q_.pop();

		// out signal
		if (data->bTokenIndex == 255 && data->bChannelIndex == 255)
			return;

		if (data->bTokenIndex >= EDiscordTokens::DISCORD_TOKEN_MAX)
		{
			sys_err("bTokenIndex out of range! (bTokenIndex: %d, EDiscordTokens::DISCORD_TOKEN_MAX: %d)", data->bTokenIndex, EDiscordTokens::DISCORD_TOKEN_MAX);
			continue;
		}

		if (data->bChannelIndex >= EDiscordChannels::DISCORD_CHANNEL_MAX)
		{
			sys_err("bChannelIndex out of range! (bChannelIndex: %d, EDiscordChannels::DISCORD_CHANNEL_MAX: %d)", data->bChannelIndex, EDiscordChannels::DISCORD_CHANNEL_MAX);
			continue;
		}

		__SendDiscordMessage(arrTokens[data->bTokenIndex], arrChannels[data->bChannelIndex], data->sMessage);
	}
}

/** Get example

/// pkg install nlohmann-json
#include <nlohmann/json.hpp>
static bool GetChannelName(const char* token, uint64_t channelid, std::string *str)
{
	try {
		const std::string endpoint = "https://discord.com/api/v9/channels/" + std::to_string(channelid);
		cpr::Response x = cpr::Get(
			cpr::Url{ endpoint },
			cpr::Header{ {"Authorization", token } }
		);
		const auto json = nlohmann::json::parse(x.text);
		*str = json["name"].get<std::string>();
	}
	catch (const std::exception& ex)
	{
		sys_err("%s", ex.what());
		return false;
	}

	return true;
}

**/