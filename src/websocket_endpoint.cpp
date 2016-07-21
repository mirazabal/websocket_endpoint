// websocket_endpoint.cpp : Defines the exported functions for the DLL application.
//
#ifdef _WIN32 // remember that this macro is defined for x86 and x64
#include "stdafx.h"
#elif __APPLE__
	"Do not compile as it has not been tested"
#elif __linux
#endif

#include "../header/websocket_endpoint.h"

using namespace mirazabal::sio;

		websocket_endpoint::websocket_endpoint() : m_next_id(0), m_connected(false)
		{
			m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

			m_endpoint.set_access_channels(websocketpp::log::alevel::connect
				| websocketpp::log::alevel::disconnect | websocketpp::log::alevel::app);

			// Initialize the Asio transport policy
			m_endpoint.init_asio();

			// Bind the clients we are using
			m_endpoint.set_open_handler([&](websocketpp::connection_hdl con)
			{
				this->on_open(con);
			});

			m_endpoint.set_close_handler([&](websocketpp::connection_hdl con)
			{
				this->on_close(con);
			});

			m_endpoint.set_fail_handler([&](websocketpp::connection_hdl con)
			{
				this->on_fail(con);
			});

			m_endpoint.set_message_handler([&](websocketpp::connection_hdl con,
					websocketpp::client<websocketpp::config::debug_asio>::message_ptr msg)
			{
				this->on_message(con, msg);
			});

			m_packet_mgr.set_decode_callback([&](sio::sio_packet const& pack)
			{
				this->on_decode(pack);
			});

			m_packet_mgr.set_encode_callback([&](bool isBinary,
				std::shared_ptr<const std::string> const& payload)
			{
				this->on_encode(isBinary, payload);
			});
		
		}

		websocket_endpoint::~websocket_endpoint()
		{
			m_endpoint.stop();
			m_network_future.get();
		}

		int websocket_endpoint::connect(std::string const & uri)
		{
			try
			{
				websocketpp::uri uo(uri);
				std::ostringstream ss;

				ss << "ws://" << uo.get_host() << ":" << uo.get_port() <<
					"/socket.io/?EIO=4&transport=websocket&t=" << time(NULL);

				m_endpoint.reset();
				m_packet_mgr.reset();

				websocketpp::lib::error_code ec;
				client::connection_ptr con = m_endpoint.get_connection(ss.str(), ec);

				if (ec)
				{
					m_endpoint.get_alog().write(websocketpp::log::alevel::app,
						"Get Connection Error: " + ec.message());
					std::cout << "> Connect initialization error: " << ec.message() << std::endl;
					return -1;
				}

				m_next_id++;
				int new_id = m_next_id;

				connection_metadata::ptr metadata_ptr(new connection_metadata(new_id, con->get_handle(), uri));
				m_connection_list[new_id] = metadata_ptr;

				auto con_ptr = m_endpoint.connect(con);

				m_network_future = std::async(std::launch::async, [&]()
				{
					try
					{
						m_endpoint.run();
						m_endpoint.reset();
						m_endpoint.get_alog().write(websocketpp::log::alevel::devel,
							"run loop end");
					}
					catch (std::exception const& ex)
					{
						std::cout << "exception thrown at run_loop" << ex.what() << std::endl;
						throw ex;
					}
					catch (...)
					{
						std::cout << "Exception not caught" << std::endl;
						throw std::runtime_error("Exception not caught from the network future");
					}

				});

				std::unique_lock<std::mutex> lk(conn_mut);
				if (! conn_cond.wait_for(lk,
					std::chrono::milliseconds(5000), [&]() {return m_connected == true;} ) )
					throw std::runtime_error("connection timeot reached");
				
				return new_id;
			}
			catch (websocketpp::exception const& ex)
			{
				std::cout << ex.what() << std::endl;
				throw ex;
			}
			catch (...)
			{
				std::cout << "Uncaught exception" << std::endl;
				throw;
			}
		}


		void websocket_endpoint::emit(std::string const& msg_name, std::string const&  msg)
		{
			sio::sio_message::ptr msg_ptr = sio::sio_message::list(msg).to_array_message(msg_name);
			int pack_id = -1;

			std::string m_nsp("/");
			sio::sio_packet p(m_nsp, msg_ptr, pack_id);

			m_packet_mgr.encode(p); // this will fire the event on_encode->send, pedazo de mierda....

		}

		void websocket_endpoint::on_encode(bool isBinary, std::shared_ptr<const std::string> const& payload)
		{
			m_endpoint.get_io_service().dispatch(
				websocketpp::lib::bind(&websocket_endpoint::send, this, payload, isBinary ? websocketpp::frame::opcode::binary : websocketpp::frame::opcode::text));
		}


		void websocket_endpoint::on_decode(sio::sio_packet const& pack)
		{
			switch (pack.get_frame())
			{

			case sio::sio_packet::frame_type::frame_message:
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				//sio_socket::ptr so_ptr = get_socket_locked(pack.get_nsp());
				//if (so_ptr)
				//	so_ptr->on_message_packet(pack);
				this->on_message_packet(pack);

				break;
			}
			case sio::sio_packet::frame_type::frame_open:
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				this->on_handshake(pack.get_message());
				break;
			case sio::sio_packet::frame_type::frame_close:
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				//FIXME how to deal?
				//this->close_impl(close::status::abnormal_close, "End by server");
				break;
			case sio::sio_packet::frame_type::frame_pong:
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				//this->on_pong();
				break;

			default:
				break;
			}
		}


		void websocket_endpoint::on_handshake(sio::sio_message::ptr const& message)
		{
			if (message && message->get_flag() == sio::sio_message::flag::flag_object)
			{
				const sio::object_message* obj_ptr = static_cast<sio::object_message*>(message.get());
				const std::map<std::string, sio::sio_message::ptr>* values = &(obj_ptr->get_map());
				auto it = values->find("sid");
				if (it != values->end()) {
					m_session_id = std::static_pointer_cast<sio::string_message>(it->second)->get_string();
				}
				else
				{
					goto failed;
				}
				it = values->find("pingInterval");
				if (it != values->end() && it->second->get_flag() == sio::sio_message::flag::flag_integer) {
					m_ping_interval = (unsigned)std::static_pointer_cast<sio::int_message>(it->second)->get_int();
				}
				else
				{
					m_ping_interval = 25000;
				}
				it = values->find("pingTimeout");

				if (it != values->end() && it->second->get_flag() == sio::sio_message::flag::flag_integer) {
					m_ping_timeout = (unsigned)std::static_pointer_cast<sio::int_message>(it->second)->get_int();
				}
				else
				{
					m_ping_timeout = 60000;
				}

				m_ping_timer.reset(new boost::asio::deadline_timer(m_endpoint.get_io_service()));
				boost::system::error_code ec;
				m_ping_timer->expires_from_now( boost::posix_time::milliseconds(m_ping_interval) , ec);

				if (ec)
					std::cout << "ec:" << ec.message() << std::endl;

				//m_ping_timer->async_wait(websocketpp::lib::bind(&Impl::ping, this, websocketpp::lib::placeholders::_1));
				std::cout << "On handshake,session_id:" << m_session_id << ",ping interval:" << m_ping_interval << ",ping timeout" << m_ping_timeout << std::endl;

				return;
			}
			else
			{
				//just close it.
				//m_endpoint.get_io_service().dispatch(websocketpp::lib::bind(&Impl::close_impl, this, close::status::policy_violation, "Handshake error"));
			}

		failed:
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//just close it.
			//m_endpoint.get_io_service().dispatch(websocketpp::lib::bind(&Impl::close_impl, this, close::status::policy_violation, "Handshake error"));
		}






void websocket_endpoint::send(std::shared_ptr<const std::string> const&  payload_ptr, websocketpp::frame::opcode::value opcode)
{
			//delay the ping, since we already have message to send.
			/*boost::system::error_code timeout_ec;
			if (m_ping_timer)
			{
			m_ping_timer->expires_from_now(milliseconds(m_ping_interval), timeout_ec);
			m_ping_timer->async_wait(websocketpp::lib::bind(&Impl::ping, this, websocketpp::lib::placeholders::_1));
			}*/

			websocketpp::lib::error_code ec;
			m_endpoint.send(m_con, *payload_ptr, opcode, ec);

			if (ec)
			{
				std::string err{ec.message()};
				std::cout << "Send failed, reason:" << err << std::endl;
				throw std::runtime_error( ec.message() );
			}
		}




		void websocket_endpoint::on_message_packet(sio::sio_packet const& p)
		{
			/*if (p.get_nsp() == m_nsp)
			{*/
			switch (p.get_type())
			{
				// Connect open
			case sio::sio_packet::type::type_connect:
			{
				std::cout << "Received Message type (Connect)" << std::endl;
				this->on_connected();
				break;
			}
			case sio::sio_packet::type::type_disconnect:
			{
				std::cout << "Received Message type (Disconnect)" << std::endl;
				//this->on_close();
				break;
			}
			case sio::sio_packet::type::type_event:
			case sio::sio_packet::type::type_binary_event:
			{
				std::cout << "Received Message type (Event)" << std::endl;
				const sio::sio_message::ptr ptr = p.get_message();
				if (ptr->get_flag() == sio::sio_message::flag::flag_array)
				{
					const sio::array_message* array_ptr = static_cast<const sio::array_message*>(ptr.get());
					if (array_ptr->get_vector().size() >= 1 &&
						array_ptr->get_vector()[0]->get_flag() == sio::sio_message::flag::flag_string)
					{
						const sio::string_message* name_ptr = static_cast<const sio::string_message*>(array_ptr->get_vector()[0].get());
						sio::sio_message::list mlist;
						for (size_t i = 1;i < array_ptr->get_vector().size();++i)
						{
							mlist.push(array_ptr->get_vector()[i]);
						}
						//this->on_socketio_event(p.get_nsp(), p.get_pack_id(), name_ptr->get_string(), std::move(mlist));
					}
				}

				break;
			}
			// Ack
			case sio::sio_packet::type::type_ack:
			case sio::sio_packet::type::type_binary_ack:
			{
				std::cout << "Received Message type (ACK)" << std::endl;
				const sio::sio_message::ptr ptr = p.get_message();
				if (ptr->get_flag() == sio::sio_message::flag::flag_array)
				{
					sio::sio_message::list msglist(ptr->get_vector());
					//	this->on_socketio_ack(p.get_pack_id(), msglist);
				}
				else
				{
					//	this->on_socketio_ack(p.get_pack_id(), sio_message::list(ptr));
				}
				break;
			}
			// Error
			case sio::sio_packet::type::type_error:
			{
				std::cout << "Received Message type (ERROR)" << std::endl;
				//	this->on_socketio_error(p.get_message());
				break;
			}
			default:
				break;
			}
			//}
		}

		void websocket_endpoint::on_connected()
		{
			if (m_connection_timer)
			{
				m_connection_timer->cancel();
				m_connection_timer.reset();
			}
			if (!m_connected)
			{
				{
				std::lock_guard<std::mutex> lk(conn_mut);
				m_connected = true;
				conn_cond.notify_one();
				}
				//	m_sio_client->on_socket_opened(m_nsp);

				while (true)
				{
					m_packet_mutex.lock();
					if (m_packet_queue.empty())
					{
						m_packet_mutex.unlock();
						return;
					}
					sio::sio_packet front_pack = std::move(m_packet_queue.front());
					m_packet_queue.pop();
					m_packet_mutex.unlock();

					m_packet_mgr.encode(front_pack);
				}
			}
		}



		void websocket_endpoint::on_open(websocketpp::connection_hdl con)
		{
			m_con = con;
			std::cout << "Connected" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		void websocket_endpoint::on_close(websocketpp::connection_hdl con)
		{
			std::cout << "on_close" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		void websocket_endpoint::on_fail(websocketpp::connection_hdl con)
		{
			std::cout << "on_fail" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		void websocket_endpoint::on_message(websocketpp::connection_hdl con,
			websocketpp::client<websocketpp::config::debug_asio>::message_ptr msg)
		{
			// Parse the incoming message according to socket.IO rules
			m_packet_mgr.put_payload(msg->get_payload()); // will call decode function
		}

		connection_metadata::ptr websocket_endpoint::get_metadata(int id) const
		{
			con_list::const_iterator metadata_it = m_connection_list.find(id);
			
			if (metadata_it == m_connection_list.end())
				return connection_metadata::ptr();
			else
				return metadata_it->second;
		}
