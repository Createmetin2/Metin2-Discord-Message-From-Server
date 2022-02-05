/*
* Author: blackdragonx61 / Mali
* 29.01.2022
* Discord
*/

#include "stdafx.h"
#include "discord.hpp"

#include "../common/tables.h"

#include <cpr/cpr.h> // https://github.com/libcpr/cpr

#include <nlohmann/json.hpp> // https://github.com/nlohmann/json
using json = nlohmann::json;

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
	AddEvent(std::unique_ptr<SDicordMessage>(new SDicordMessage{ EDiscordEventType::QUIT }));
	thread_->join();
};

static std::string FormatString(const char* message, va_list args)
{
	const int len = vsnprintf(nullptr, 0, message, args) + 1;
	if (len < 1)
		return std::string();

	const size_t size = static_cast<size_t>(len);
	auto buf = std::unique_ptr<char[]>(new char[size]);
	vsnprintf(buf.get(), size, message, args);

	return std::string(buf.get(), buf.get() + size - 1);
}

void CDiscordManager::SendText(BYTE bTokenIndex, BYTE bChannelIndex, const char* format, ...)
{
	if (bTokenIndex >= EDiscordTokens::DISCORD_TOKEN_MAX)
	{
		sys_err("bTokenIndex out of range! (bTokenIndex: %d, EDiscordTokens::DISCORD_TOKEN_MAX: %d)", bTokenIndex, EDiscordTokens::DISCORD_TOKEN_MAX);
		return;
	}

	if (bChannelIndex >= EDiscordChannels::DISCORD_CHANNEL_MAX)
	{
		sys_err("bChannelIndex out of range! (bChannelIndex: %d, EDiscordChannels::DISCORD_CHANNEL_MAX: %d)", bChannelIndex, EDiscordChannels::DISCORD_CHANNEL_MAX);
		return;
	}

	va_list args;
	va_start(args, format);

	AddEvent(std::unique_ptr<SDicordMessage>(
		new SDicordMessage{
			EDiscordEventType::MESSAGE,
			bTokenIndex,
			bChannelIndex,
			FormatString(format, args)
		}
	));

	va_end(args);
}

void CDiscordManager::SendPrivateMessageText(BYTE bTokenIndex, std::uint64_t uUserID, const char* format, ...)
{
	if (bTokenIndex >= EDiscordTokens::DISCORD_TOKEN_MAX)
	{
		sys_err("bTokenIndex out of range! (bTokenIndex: %d, EDiscordTokens::DISCORD_TOKEN_MAX: %d)", bTokenIndex, EDiscordTokens::DISCORD_TOKEN_MAX);
		return;
	}

	va_list args;
	va_start(args, format);
	
	AddEvent(std::unique_ptr<SDicordMessage>(
		new SDicordMessage{
			EDiscordEventType::PRIVATEMESSAGE,
			bTokenIndex,
			0,
			FormatString(format, args),
			uUserID
		}
	));

	va_end(args);
}

void CDiscordManager::AddEvent(std::unique_ptr<SDicordMessage>&& info)
{
	std::lock_guard<std::mutex> l(mutex_);
	q_.push(std::move(info));
	cv_.notify_one();
}

void CDiscordManager::__SendDiscordMessage(const std::unique_ptr<SDicordMessage>& info)
{
	try {
		cpr::Header mapHeader{
			{ "Content-Type", "application/json" },
			{ "Authorization", arrTokens[info->bTokenIndex] }
		};

		const json jBody{
			{ "content", info->sMessage }
		};

		std::string sChannelID = info->EEventType == EDiscordEventType::PRIVATEMESSAGE ? 
			std::to_string(info->uExtraID) : std::to_string(arrChannels[info->bChannelIndex]);
		
		std::string sEndPoint = "https://discord.com/api/v9/channels/" + std::move(sChannelID) + "/messages";
		cpr::Post(
			cpr::Url{ std::move(sEndPoint) },
			std::move(mapHeader),
			cpr::Body{ jBody.dump() }
		);
	}
	catch (const std::exception& ex) {
		sys_err("%s", ex.what());
	}
}

void CDiscordManager::__SendPrivateMessage(const std::unique_ptr<SDicordMessage>& info)
{
	try {
		cpr::Header mapHeader{
			{ "Content-Type", "application/json" },
			{ "Authorization", arrTokens[info->bTokenIndex] }
		};

		const json jBody{
			{ "recipient_id", info->uExtraID }
		};

		std::string sEndPoint = "https://discord.com/api/v9/users/@me/channels";
		
		const cpr::Response cPost = cpr::Post(
			cpr::Url{ std::move(sEndPoint) },
			std::move(mapHeader),
			cpr::Body{ jBody.dump() }
		);

		const json jGet = nlohmann::json::parse(cPost.text);
		const std::string& sPrivateMessageChannelID = jGet.at("id");
		info->uExtraID = std::stoull(sPrivateMessageChannelID);

		__SendDiscordMessage(info);
	}
	catch (const std::exception& ex)
	{
		sys_err("%s", ex.what());
	}
}

void CDiscordManager::__Loop()
{
	while (true)
	{
		std::unique_lock<std::mutex> l(mutex_);
		cv_.wait(l, [&] { return !q_.empty(); });

		auto info{ std::move(q_.front()) };
		q_.pop();

		switch (info->EEventType)
		{
		case EDiscordEventType::MESSAGE:
			__SendDiscordMessage(info);
			break;
		case EDiscordEventType::PRIVATEMESSAGE:
			__SendPrivateMessage(info);
			break;
		case EDiscordEventType::QUIT:
			return;
		default:
			continue;
		}
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