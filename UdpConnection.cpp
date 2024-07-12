#include "UdpConnection.h"


using boost::asio::ip::udp;
using namespace resip;

UdpConnection::UdpConnection(boost::asio::io_service* io_context,const std::string&flowKey)
	:ConnectBase(flowKey),socket_(*io_context) {

}

UdpConnection::~UdpConnection()
{
	if (m_status == ConnectBase::ConnectStatus::S_Open)
	{
		close(2,"");
	}
}

bool UdpConnection::send(const std::string& msg)
{
	if (m_status == ConnectBase::ConnectStatus::S_Open)
	{
		size_t _size = socket_.send_to(boost::asio::buffer(msg.data(), msg.size()),m_remoteEndPoint);
		if (_size == msg.size())
			return true;
	}
	return false;
}

void UdpConnection::asyncRead(boost::system::error_code ec, std::size_t length) {
	if (ec)
	{
		if (ec != boost::asio::error::operation_aborted)
		{
			m_status = ConnectBase::ConnectStatus::S_Closed;
			m_messageEventHandler->closeNotify(m_flowKey);
		}
		return;
	}

	if (length == 0)
	{
		receiveMsg();
		return;
	}
	//DebugLog(<<"*********************" << m_buffer.data());
	for (size_t pos = 0; pos < length;)
	{
		auto _result = m_msgSegment.addBuffer(m_buffer.data() + pos, length - pos);
		if (_result.first != SipMsgSegment::ProcessingStatus::complete)
		{
			//receiveMsg();
		}
		else
		{
			std::string _sipmsg;
			if (m_msgSegment.getSipMsg(_sipmsg))
			{
				m_messageEventHandler->messageNotify(m_flowKey,_sipmsg);
			}
		}
		pos += _result.second;
	}
	m_buffer.fill('\0');
	receiveMsg();
}

void UdpConnection::connect(const std::string& host, const int& port)
{
	if (m_status == ConnectBase::ConnectStatus::S_Open)
	{
		return;
	}
	m_status = ConnectBase::ConnectStatus::S_Connecting;
	m_remoteEndPoint = udp::endpoint(boost::asio::ip::address::from_string(host.c_str()), port);
}

void UdpConnection::close(short code, const std::string& reason)
{
	if (m_status == ConnectBase::ConnectStatus::S_Open)
	{
		m_status = ConnectBase::ConnectStatus::S_Closed;
		boost::system::error_code ignored_ec;
		socket_.cancel(ignored_ec);
		socket_.shutdown(boost::asio::ip::udp::socket::shutdown_both, ignored_ec);
		socket_.close();
		if (ignored_ec)
		{
			std::cerr<< "tcp close error: " << ignored_ec.message();
		}
	}
	
}

void UdpConnection::receiveMsg()
{
	if (m_status != ConnectBase::ConnectStatus::S_Open)
	{
		return;
	}
	socket_.async_receive(boost::asio::buffer(m_buffer),
		std::bind(&UdpConnection::asyncRead, this,
			std::placeholders::_1,
			std::placeholders::_2));
}

bool UdpConnection::bind(const std::string& host, const int& port)
{
	boost::system::error_code ec;
	socket_.open(boost::asio::ip::udp::v4());
	bool _cr = false;
	
	int _port = 9000;//udp::endpoint(boost::asio::ip::address::from_string("���IP��ַ")
	socket_.bind(udp::endpoint(boost::asio::ip::address::from_string(host), port), ec);

	if (ec) {
		// ��ʧ�ܣ�ec.value()�����ش����룬ec.message()�����ش�����Ϣ
		std::cerr << "Bind failed: " << ec.message();
		_port += 1;
	}
	else {
		// �󶨳ɹ�
		std::clog << "Bind successful";
		_cr = true;
	}
	

	if (_cr)
	{
		m_status = ConnectBase::ConnectStatus::S_Open;
		m_sourcePort = socket_.local_endpoint().port();
		m_sourceHost = socket_.local_endpoint().address().to_string().c_str();
		m_messageEventHandler->openNotify(m_flowKey);
		receiveMsg();
	}
	else
	{
		m_status = ConnectBase::ConnectStatus::S_Failed;
		m_messageEventHandler->failNotify(m_flowKey,ec.message());

	}

	return _cr;
}

