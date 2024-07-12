#pragma once
#include "string"
#include "MessageEventHandler.hxx"
namespace resip
{
	class MessageEventHandler;
	class ConnectBase
	{
	public:
		ConnectBase(const std::string& flowKey) :m_flowKey(flowKey) {}
		virtual ~ConnectBase() {};
		enum class ConnectStatus
		{
			S_NULL = -1,
			S_Connecting = 0,
			S_Open = 1,
			S_Closed = 2,
			S_Failed = 3
		};
	public:
		inline void connectEventHandler(MessageEventHandler* handler) { m_messageEventHandler = handler; };
		inline void disConnectEventHandler() { m_messageEventHandler = nullptr; };
		virtual void connect(const std::string& host, const int& port)=0;
		virtual void close(short code, const std::string& reason)=0;
		virtual bool send(const std::string& msg)=0;
		inline ConnectStatus getStatus() const {return m_status;}
		inline const std::string& getErrorReason()const { return m_error_reason; }
		inline const int& getSourcePort() const {return m_sourcePort; }
		inline const std::string& getSourceHost() const {return m_sourceHost; }
	protected:
		ConnectStatus m_status;
		std::string m_error_reason;
		MessageEventHandler* m_messageEventHandler = nullptr;

		int m_sourcePort;
		std::string m_sourceHost;
		std::string m_flowKey;
	};
}

