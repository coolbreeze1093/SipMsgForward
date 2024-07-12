#pragma once
// Asynchronous echo client.

#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include <string>
#include "boost/asio/ip/udp.hpp"
#include <boost/asio/connect.hpp>
#include <boost/asio/write.hpp>
#include "ConnectBase.h"
#include "SipMsgSegment.h"

	// Use async_resolve() or not.
#define RESOLVE_ASYNC 1
// Only resolve IPv4.
#define RESOLVE_IPV4_ONLY 1

namespace resip
{
	
	class UdpConnection :public ConnectBase {
	public:
		UdpConnection(boost::asio::io_service* io_context, const std::string& flowKey);
		~UdpConnection();
		bool bind(const std::string& host, const int& port);
		void connect(const std::string& host, const int& port)override;
		void close(short code, const std::string& reason)override;
		bool send(const std::string& msg)override;
	private:
		void asyncRead(boost::system::error_code ec, std::size_t length);

		void receiveMsg();
	private:
		boost::asio::ip::udp::socket socket_;

		boost::asio::ip::udp::endpoint m_remoteEndPoint;

		enum { BUF_SIZE = 4096 };
		std::array<char, BUF_SIZE> m_buffer;

		SipMsgSegment m_msgSegment;
	};
}
