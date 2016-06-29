#ifndef CONNECTION_METADATA_H
#define CONNECTION_METADATA_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>



class connection_metadata
{
public:
	typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;
	typedef websocketpp::client<websocketpp::config::asio_client> client;

	connection_metadata(int id, websocketpp::connection_hdl hdl, std::string uri);
	~connection_metadata();
	
	void on_open(websocketpp::connection_hdl hdl);
	void on_fail( websocketpp::connection_hdl hdl);
	void on_close(websocketpp::connection_hdl hdl);

	friend std::ostream & operator<<(std::ostream & out, connection_metadata const & data);
	

private:
	int m_id;
	websocketpp::connection_hdl m_hdl;
	std::string m_status;
	std::string m_uri;
	std::string m_server;
	std::string m_error_reason;

};

#endif
