#pragma once
#include <string>
namespace resip
{
	class SipMsgSegment
	{
	public:
		enum class ProcessingStatus
		{
			init = 0,
			headerIncomplete = 1,
			contentIncomplete = 2,
			complete = 3
		};
	public:
		SipMsgSegment();
		~SipMsgSegment();
		std::pair<ProcessingStatus, size_t> addBuffer(const char* ptr, size_t size);
		bool getSipMsg(std::string& msg);
	private:
		size_t findContentLenth(const std::string& msg);
	private:
		std::string m_buffer;
		std::string m_message;
		size_t m_remainContentCount = 0;
		ProcessingStatus m_status;
	};
}


