/*
* Author: blackdragonx61 / Mali
* 29.01.2022
* Discord
*/

#ifndef __INC_METIN_II_DISCORD_HPP__
#define __INC_METIN_II_DISCORD_HPP__

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

class CDiscordManager : public singleton<CDiscordManager>
{
private:
	enum class EDiscordEventType : std::uint8_t
	{
		MESSAGE,
		PRIVATEMESSAGE,
		QUIT
	};

	struct SDicordMessage
	{
		EDiscordEventType EEventType;
		BYTE bTokenIndex;
		BYTE bChannelIndex;
		std::string sMessage;
		std::uint64_t uExtraID; // For private message
	};

public:
	CDiscordManager();
	~CDiscordManager();

	void SendText(BYTE bTokenIndex, BYTE bChannelIndex, const char* format, ...);
	void SendPrivateMessageText(BYTE bTokenIndex, std::uint64_t uUserID, const char* format, ...);
	void AddEvent(std::unique_ptr<SDicordMessage>&& info);

private:
	void __SendDiscordMessage(const std::unique_ptr<SDicordMessage>& info);
	void __SendPrivateMessage(const std::unique_ptr<SDicordMessage>& info);
	void __Loop();

	std::unique_ptr<std::thread> thread_;
	std::mutex mutex_;
	std::condition_variable cv_;
	std::queue<std::unique_ptr<SDicordMessage>> q_;
};

#endif