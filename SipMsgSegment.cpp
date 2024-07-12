#include "SipMsgSegment.h"
using namespace resip;
SipMsgSegment::SipMsgSegment() :m_status(ProcessingStatus::init)
{

}
SipMsgSegment::~SipMsgSegment()
{

}
bool SipMsgSegment::getSipMsg(std::string& msg)
{
	if (m_status == SipMsgSegment::ProcessingStatus::complete)
	{
		m_status = ProcessingStatus::init;
		msg = m_message;
		m_message.clear();

		return true;
	}
	return false;
}

size_t SipMsgSegment::findContentLenth(const std::string&msg)
{
	size_t _CLposStart(0);
	size_t _contentLenth(0);
	_CLposStart = msg.find("Content-Length: ");
	if (_CLposStart != std::string::npos)
	{
		size_t _CLposEnd = msg.find("\r\n", _CLposStart);
		std::string _clstr = msg.substr(_CLposStart + 16, _CLposEnd - _CLposStart - 16);
		_contentLenth = std::stoi(_clstr);
	}
	else
	{
		_CLposStart = msg.find("Content-Length:");
		if (_CLposStart != std::string::npos)
		{
			size_t _CLposEnd = msg.find("\r\n", _CLposStart);
			std::string _clstr = msg.substr(_CLposStart + 15, _CLposEnd - _CLposStart - 15);
			_contentLenth = std::stoi(_clstr);
		}
	}
	return _contentLenth;
}

std::pair<SipMsgSegment::ProcessingStatus, size_t> SipMsgSegment::addBuffer(const char* ptr, size_t size)
{
	size_t _headerEndPos(0);
	size_t _singleHeaderStartPos(0);
	size_t _CLposStart(0);
	size_t _contentLenth(0);
	size_t _count(0);
	size_t _orgBufferSize(0);

	std::pair<SipMsgSegment::ProcessingStatus, size_t> _result;
	_result.second = 0;
	_result.first = SipMsgSegment::ProcessingStatus::init;

	switch (m_status)
	{
	case SipMsgSegment::ProcessingStatus::init:
	{
		std::string _message(ptr);
		_singleHeaderStartPos = _message.find("\r\n\r\n");
		if (_singleHeaderStartPos != std::string::npos)
		{
			_headerEndPos = _singleHeaderStartPos + 3;
		}
		else
		{
			_headerEndPos = -1;
		}

		if (_headerEndPos != -1)
		{
			_contentLenth = findContentLenth(_message);
			if (_contentLenth == 0)
			{
				_count = _result.second = _headerEndPos + 1;
				m_status = SipMsgSegment::ProcessingStatus::complete;
			}
			else
			{
				if (_headerEndPos + _contentLenth + 1 <= size)
				{
					m_status = SipMsgSegment::ProcessingStatus::complete;
					_count = _result.second = _headerEndPos + _contentLenth + 1;
				}
				else
				{
					m_status = SipMsgSegment::ProcessingStatus::contentIncomplete;
					m_remainContentCount = _contentLenth - (size - (_headerEndPos + 1));
					_count = _result.second = size;
				}
			}
		}
		else
		{
			m_status = SipMsgSegment::ProcessingStatus::headerIncomplete;
			m_buffer.append(ptr);
			_count = _result.second = size;
		}
	}
		break;
	case SipMsgSegment::ProcessingStatus::headerIncomplete:
	{
		
		if (m_buffer.empty())
		{
			_orgBufferSize = 0;
		}
		else
		{
			_orgBufferSize = m_buffer.size();
		}
		
		m_buffer.append(ptr);
		_singleHeaderStartPos = m_buffer.find("\r\n\r\n");
		if (_singleHeaderStartPos != std::string::npos)
		{
			_headerEndPos = _singleHeaderStartPos + 3;
		}
		else
		{
			_headerEndPos = -1;
		}
		
		if (_headerEndPos != -1)
		{
			_contentLenth = findContentLenth(m_buffer);
			if (_contentLenth == 0)
			{
				_count = _result.second = _headerEndPos + 1 - _orgBufferSize;

				m_status = SipMsgSegment::ProcessingStatus::complete;
			}
			else
			{
				if (_headerEndPos + _contentLenth + 1 <= m_buffer.size())
				{
					m_status = SipMsgSegment::ProcessingStatus::complete;
					_count = _result.second = _headerEndPos + _contentLenth + 1- _orgBufferSize;
				}
				else
				{
					m_status = SipMsgSegment::ProcessingStatus::contentIncomplete;
					m_remainContentCount = _contentLenth - (m_buffer.size() - (_headerEndPos + 1));
					_count = _result.second = size;
				}
			}
			m_buffer.clear();
		}
		else
		{
			m_status = SipMsgSegment::ProcessingStatus::headerIncomplete;
			_count = _result.second = size;
		}
	}
		break;
	case SipMsgSegment::ProcessingStatus::contentIncomplete:
		if (m_remainContentCount <= size)
		{
			m_status = SipMsgSegment::ProcessingStatus::complete;
			_count = _result.second = m_remainContentCount;
		}
		else
		{
			m_status = SipMsgSegment::ProcessingStatus::contentIncomplete;
			m_remainContentCount = m_remainContentCount - size;
			_count = _result.second = size;
		}
		break;
	case SipMsgSegment::ProcessingStatus::complete:

		break;
	default:
		break;
	}

	m_message.append(ptr, _count);
	_result.first = m_status;
	return _result;
}
