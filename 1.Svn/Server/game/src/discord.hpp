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

class CDiscordManager : public singleton<CDiscordManager>
{
public:
	CDiscordManager();
	~CDiscordManager();

	void SendText(BYTE bTokenIndex, BYTE bChannelIndex, const char* format, ...);

private:
	void __SendDiscordMessage(const char* szToken, std::uint64_t uChannelID, const std::string& sMessage);
	void __Loop();

private:
	struct SDicordMessage
	{
		BYTE bTokenIndex;
		BYTE bChannelIndex;
		std::string sMessage;
	};

	std::unique_ptr<std::thread> thread_;
	std::mutex mutex_;
	std::condition_variable cv_;
	std::queue<std::unique_ptr<SDicordMessage>> q_;
};

#endif