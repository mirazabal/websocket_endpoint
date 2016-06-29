#ifdef _WIN32 // remember that this macro is defined for x86 and x64
#include "stdafx.h"
#elif __APPLE__
	"Do not compile as it has not been tested"
#elif __linux
#endif

#include "../header/sio_packet_manager.h"

#include <cassert>

namespace mirazabal
{

	namespace sio
	{
		sio_packet_manager::sio_packet_manager(void)
		{

		}


		sio_packet_manager::~sio_packet_manager(void)
		{
		}

		void sio_packet_manager::set_decode_callback(function<void(sio_packet const&)> const& decode_callback)
		{
			m_decode_callback = decode_callback;
		}

		void sio_packet_manager::set_encode_callback(function<void(bool, shared_ptr<const string> const&)> const& encode_callback)
		{
			m_encode_callback = encode_callback;
		}

		void sio_packet_manager::reset()
		{
			m_partial_packet.reset();
		}

		void sio_packet_manager::encode(sio_packet& pack, encode_callback_function const& override_encode_callback) const
		{
			shared_ptr<string> ptr = make_shared<string>();
			vector<shared_ptr<const string> > buffers;
			const encode_callback_function *cb_ptr = &m_encode_callback;
			if (override_encode_callback)
			{
				cb_ptr = &override_encode_callback;
			}
			if (pack.accept(*ptr, buffers))
			{
				// encode callback not assigned
				assert((*cb_ptr) != nullptr);
				(*cb_ptr)(false, ptr);

				for (auto it = buffers.begin();it != buffers.end();++it)
				{
					assert((*cb_ptr) != nullptr);
					(*cb_ptr)(true, *it);
				}
			}
			else
			{
				assert((*cb_ptr) != nullptr);
				(*cb_ptr)(false, ptr);

			}
		}

		void sio_packet_manager::put_payload(string const& payload)
		{
			unique_ptr<sio_packet> p;
			do
			{
				if (sio_packet::is_text_message(payload))
				{
					p.reset(new sio_packet());
					if (p->parse(payload))
					{
						m_partial_packet = std::move(p);
					}
					else
					{
						break;
					}
				}
				else if (sio_packet::is_binary_message(payload))
				{
					if (m_partial_packet)
					{
						if (!m_partial_packet->parse_buffer(payload))
						{
							p = std::move(m_partial_packet);
							break;
						}
					}
				}
				else
				{
					p.reset(new sio_packet());
					p->parse(payload);
					break;
				}
				return;
			} while (0);

			if (m_decode_callback)
			{
				m_decode_callback(*p);
			}
		}

	}
}
