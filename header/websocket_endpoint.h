#ifndef WEBSOCKET_ENDPOINT_H
#define WEBSOCKET_ENDPOINT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif 


#ifdef _WIN32
	#ifdef WEBSOCKET_ENDPOINT_EXPORTS
		#define WEBSOCKET_ENDPOINT_API __declspec(dllexport)
	#else
		#define WEBSOCKET_ENDPOINT_API __declspec(dllimport)
	#endif
#else
	#define WEBSOCKET_ENDPOINT_API
#endif

#define BOOST_ALL_NO_LIB
#define _WEBSOCKETPP_CPP11_THREAD_
#define _WEBSOCKETPP_NO_CPP11_FUNCTIONAL_

#include "connection_metadata.h"
#include "sio_packet_manager.h"
#include "sio_packet.h"

#include <websocketpp/client.hpp>
#include <websocketpp/config/debug_asio_no_tls.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include <boost/asio/deadline_timer.hpp>

#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <future>
#include <string>
#include <queue>
#include <atomic>


namespace mirazabal
{
	namespace sio
	{
		// This class is exported from the websocket_endpoint.dll
		class WEBSOCKET_ENDPOINT_API websocket_endpoint
		{
		public:
			websocket_endpoint();
			virtual ~websocket_endpoint();

			// returns the connection ID
			int connect(std::string const & uri);
			void emit(std::string const& msg_name, std::string const&  msg);

		private:

			typedef websocketpp::client<websocketpp::config::debug_asio> client;
			typedef std::unordered_map< int, connection_metadata::ptr> con_list;

			// second thread for .run boost.ASIO
			std::future<void> m_network_future;
			connection_metadata::ptr get_metadata(int id) const;

			// Functions subscribed to functors
			void on_open(websocketpp::connection_hdl con);
			void on_close(websocketpp::connection_hdl con);
			void on_fail(websocketpp::connection_hdl con);
			void on_message(websocketpp::connection_hdl con,
				websocketpp::client<websocketpp::config::debug_asio>::message_ptr msg);
			void on_encode(bool isBinary, std::shared_ptr<const std::string> const& payload);
			void on_decode(sio::sio_packet const& pack);
			void on_message_packet(sio::sio_packet const& p);
			void on_handshake(sio_message::ptr const& message);
			void on_connected();
			void send(std::shared_ptr<const std::string> const&  payload_ptr,
				websocketpp::frame::opcode::value opcode);

			// Connection pointer for client functions.
			websocketpp::client<websocketpp::config::debug_asio> m_endpoint;
			websocketpp::connection_hdl m_con;

			// Packet manager
			std::mutex m_packet_mutex;
			std::queue<sio::sio_packet> m_packet_queue;
			sio_packet_manager m_packet_mgr;

			int m_next_id;
			std::string m_session_id;
			std::unordered_map< int, connection_metadata::ptr> m_connection_list;

			// connection 
			std::atomic<bool> m_connected;
			std::mutex conn_mut;
			std::condition_variable conn_cond;

			// ping values
			unsigned int m_ping_interval;
			unsigned int m_ping_timeout;

			std::unique_ptr<boost::asio::deadline_timer> m_ping_timer;
			std::unique_ptr<boost::asio::deadline_timer> m_ping_timeout_timer;
			std::unique_ptr<boost::asio::deadline_timer> m_reconn_timer;
			std::unique_ptr<boost::asio::deadline_timer> m_connection_timer;
		};

	} // namespace sio
} // namespace mirazabal

#endif
