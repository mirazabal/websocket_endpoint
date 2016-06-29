#ifndef _SIO_PACKET_MANAGER_H
#define _SIO_PACKET_MANAGER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#include "sio_packet.h"

#include <functional>
#include <memory>

namespace mirazabal
{
	namespace sio
	{
		class sio_packet_manager
		{
		public:

			typedef std::function<void(bool, std::shared_ptr<const std::string> const&)> encode_callback_function;
			typedef  std::function<void(sio_packet const&)> decode_callback_function;

			sio_packet_manager(void);
			~sio_packet_manager(void);

			void set_decode_callback(decode_callback_function const& decode_callback);

			void set_encode_callback(encode_callback_function const& encode_callback);

			void encode(sio_packet& pack, encode_callback_function const& override_encode_callback = encode_callback_function()) const;

			void put_payload(std::string const& payload);

			void reset();

		private:
			decode_callback_function m_decode_callback;

			encode_callback_function m_encode_callback;

			std::unique_ptr<sio_packet> m_partial_packet;

		};

	}
}
#endif
