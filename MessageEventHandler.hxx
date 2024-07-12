#pragma once
#include <string>
namespace resip {
	class MessageEventHandler
	{
	public:
		MessageEventHandler() {};
		virtual ~MessageEventHandler() {};

		virtual void openNotify(const std::string&) = 0;
		virtual void messageNotify(const std::string& ,const std::string& msg) = 0;
		virtual void closeNotify(const std::string&) = 0;
		virtual void failNotify(const std::string&,const std::string& msg) = 0;
	};
}


