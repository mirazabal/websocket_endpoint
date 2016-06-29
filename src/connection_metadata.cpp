#ifdef _WIN32 // remember that this macro is defined for x86 and x64
#include "stdafx.h"
#elif __APPLE__
	"Do not compile as it has not been tested"
#elif __linux
#endif

#include "../header/connection_metadata.h"

connection_metadata::connection_metadata(int id, websocketpp::connection_hdl hdl, 
											std::string uri)
	: m_id(id)
	, m_hdl(hdl)
	, m_status("Connecting")
	, m_uri(uri)
	, m_server("N/A")
{

}


connection_metadata::~connection_metadata()
{
}


//void connection_metadata::on_open(std::string const& server)
//{
//	m_status = "Open";
//	m_server = server;
//}
//
//void connection_metadata::on_fail(std::string const& server, std::string const& error_reason)
//{
//	m_status = "Failed";
//
//	m_error_reason = error_reason;
//}


void connection_metadata::on_open(websocketpp::connection_hdl hdl)
{
	m_status = "Open";

	//client::connection_ptr con = c->get_con_from_hdl(hdl);
	//m_server = con->get_response_header("Server");
}

void connection_metadata::on_fail(websocketpp::connection_hdl hdl)
{
	m_status = "Failed";

	/*client::connection_ptr con = c->get_con_from_hdl(hdl);
	m_server = con->get_response_header("Server");
	m_error_reason = con->get_ec().message();
*/
	std::cout << "connection failed, reason = " + m_error_reason << std::endl;
}


void connection_metadata::on_close( websocketpp::connection_hdl hdl)
{
	m_status = "Closed";
	/*client::connection_ptr con = c->get_con_from_hdl(hdl);
	std::stringstream s;
	s << "close code: " << con->get_remote_close_code() << " ("
		<< websocketpp::close::status::get_string(con->get_remote_close_code())
		<< "), close reason: " << con->get_remote_close_reason();
	m_error_reason = s.str();*/
}
