// netproxy_boost.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "UdpConnection.h"
#include "MessageEventHandler.hxx"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <thread>

const std::string flowkey_opensips = "opensips";
const std::string flowkey_freeswitch = "freeswitch";
class netproxy :public resip::MessageEventHandler
{
public:
	netproxy():m_opensips(&io_service, flowkey_opensips), m_freeswitch(&io_service, flowkey_freeswitch)
	{
		m_opensips.connectEventHandler(this);
		m_freeswitch.connectEventHandler(this);
	}
	~netproxy()
	{
	
	}

	bool init()
	{
		m_opensips.connect("172.16.0.61", 5860);
		if (!m_opensips.bind("127.0.0.1", 5006))
		{
			return false;
		}
		m_freeswitch.connect("172.16.0.61", 9080);
		if (!m_freeswitch.bind("172.16.0.61", 5005))
		{
			return false;
		}
	}
	
	void run()
	{
		io_service.run();
	}
public:
	void openNotify(const std::string&) final override{};
	void messageNotify(const std::string&flowid, const std::string& msg) final override {
		//std::cout<<"recive "<<flowid<<" msg "<<std::endl<<std::endl<<msg;
		if (flowid == flowkey_opensips)
		{
			m_freeswitch.send(msg);
		}
		else if (flowid == flowkey_freeswitch) {
			m_opensips.send(msg);
		}
	
	};
	void closeNotify(const std::string&) final override {};
	void failNotify(const std::string&, const std::string& msg) final override {};
	
private:
	boost::asio::io_service io_service;
	resip::UdpConnection m_opensips;
	resip::UdpConnection m_freeswitch;
};

int main()
{
	netproxy _p;
	if (_p.init())
	{
		_p.run();
	}
	
    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
