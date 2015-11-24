/**************************************************************************
 *	HyGuiComms.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyGuiComms_h__
#define __HyGuiComms_h__

#include "Afx/HyStdAfx.h"
#include "Afx/HyInteropAfx.h"

#include "Diagnostics/HyGuiMessage.h"

#ifndef HY_PLATFORM_GUI

// TODO: These below defines might only be necessary for MSVC 2013 (or is it to disable boost?)
#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#define ASIO_STANDALONE
#include "asio/asio.hpp"
using asio::ip::tcp;

typedef std::shared_ptr<HyGuiMessage> HyGuiMessage_Ptr;

class HyGuiSession;

class HyGuiComms
{
	static HyGuiComms *		sm_pInstance;
	HyFileIOInterop &		m_FileIORef;

	asio::io_service		m_IOService;
	tcp::acceptor			m_Acceptor;
	tcp::socket				m_Socket;

	typedef std::shared_ptr<HyGuiSession> Session_Ptr;
	std::set<Session_Ptr>	participants_;

	std::map<uint32, std::vector<std::string> >	m_ReloadMap;

public:
	HyGuiComms(uint16 uiPort, HyFileIOInterop &fileIORef);
	~HyGuiComms(void);

	void AcceptGuiConnection();
	void DisconnectGui(Session_Ptr participant);

	void ProcessMessage(HyGuiMessage &msgRef);

	static void Broadcast(eHyPacketType eType, uint32 uiDataSize, const void *pDataToCopy);
	//static void Log(const char *szMessage, uint32 uiType);


	void Update();
};

class HyGuiSession : public std::enable_shared_from_this < HyGuiSession >
{
	HyGuiComms &					m_GuiCommsRef;
	tcp::socket						m_Socket;

	HyGuiMessage					read_msg_;
	std::deque<HyGuiMessage_Ptr>	write_msgs_;

public:
	HyGuiSession(HyGuiComms &guiCommsRef, tcp::socket socket) : m_GuiCommsRef(guiCommsRef),
		m_Socket(std::move(socket))
	{
	}

	void Init()
	{
		StartReadHeader();
	}

	void QueueMessage(HyGuiMessage_Ptr msg)
	{
		bool bWriteInProgress = !write_msgs_.empty();
		write_msgs_.push_back(msg);

		if(bWriteInProgress == false)
			StartWrite();
	}

private:
	void StartReadHeader()
	{
		auto self(shared_from_this());
		asio::async_read(m_Socket, asio::buffer(read_msg_.GetData(), HyGuiMessage::HeaderSize),	[this, self](std::error_code ec, std::size_t /*length*/)
																								{
																									if(!ec && read_msg_.decode_header())
																									{
																										StartReadBody();
																									}
																									else
																									{
																										m_GuiCommsRef.DisconnectGui(shared_from_this());
																									}
																								});
	}

	void StartReadBody()
	{
		auto self(shared_from_this());
		asio::async_read(m_Socket, asio::buffer(read_msg_.body(), read_msg_.body_length()),	[this, self](std::error_code ec, std::size_t /*length*/)
																							{
																								if(!ec)
																								{
																									m_GuiCommsRef.ProcessMessage(read_msg_);
																									StartReadHeader();
																								}
																								else
																								{
																									m_GuiCommsRef.DisconnectGui(shared_from_this());
																								}
																							});
	}

	void StartWrite()
	{
		auto self(shared_from_this());
		asio::async_write(m_Socket, asio::buffer(write_msgs_.front()->GetData(), write_msgs_.front()->GetTotalSize()),	[this, self](std::error_code ec, std::size_t /*length*/)
																													{
																														if(!ec)
																														{
																															write_msgs_.pop_front();
																															if(!write_msgs_.empty())
																															{
																																StartWrite();
																															}
																														}
																														else
																														{
																															m_GuiCommsRef.DisconnectGui(shared_from_this());
																														}
																													});
	}
};

// TODO: Have an option to disable logs
#define HyLog(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	HyGuiComms::Broadcast(HYPACKET_LogNormal, static_cast<uint32>(strlen(ss.str().c_str())), ss.str().c_str()); }
#define HyLogWarning(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	HyGuiComms::Broadcast(HYPACKET_LogWarning, static_cast<uint32>(strlen(ss.str().c_str())), ss.str().c_str()); }
#define HyLogError(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	HyGuiComms::Broadcast(HYPACKET_LogError, static_cast<uint32>(strlen(ss.str().c_str())), ss.str().c_str()); }
#define HyLogInfo(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	HyGuiComms::Broadcast(HYPACKET_LogInfo, static_cast<uint32>(strlen(ss.str().c_str())), ss.str().c_str()); }
#define HyLogTitle(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	HyGuiComms::Broadcast(HYPACKET_LogTitle, static_cast<uint32>(strlen(ss.str().c_str())), ss.str().c_str()); }

#else
class HyGuiComms
{
public:
	HyGuiComms(uint16 uiPort, HyFileIOInterop &fileIORef)
	{ }

	void Update()
	{ }
};

#define HyLog(msg) do { } while (false)
#define HyLogWarning(msg) do { } while (false)
#define HyLogError(msg) do { } while (false)
#define HyLogInfo(msg) do { } while (false)
#define HyLogTitle(msg) do { } while (false)
#endif

#endif /* __HyGuiComms_h__ */