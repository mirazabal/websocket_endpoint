#ifndef SIO_PACKET_H
#define SIO_PACKET_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#include "sio_message.h"

#include <sstream>
#include <functional>

namespace mirazabal
{
	namespace sio
	{
		class sio_packet
		{

		public:

			enum class frame_type
			{
				frame_open = 0,
				frame_close = 1,
				frame_ping = 2,
				frame_pong = 3,
				frame_message = 4,
				frame_upgrade = 5,
				frame_noop = 6
			};

			enum class type
			{
				type_min = 0,
				type_connect = 0,
				type_disconnect = 1,
				type_event = 2,
				type_ack = 3,
				type_error = 4,
				type_binary_event = 5,
				type_binary_ack = 6,
				type_max = 6,
				type_undetermined = 0x10 //undetermined mask bit
			};

			//empty constructor for parse.
			sio_packet();

			explicit sio_packet(frame_type frame);

			sio_packet(std::string const& nsp, sio_message::ptr const& msg, int pack_id = -1, bool isAck = false);//message type constructor.

			sio_packet(type type, std::string const& nsp = std::string(), sio_message::ptr const& msg = sio_message::ptr());//other message types constructor.

			~sio_packet();

			frame_type get_frame() const;

			type get_type() const;

			bool parse(std::string const& payload_ptr);//return true if need to parse buffer.

			bool parse_buffer(std::string const& buf_payload);

			bool accept(std::string& payload_ptr, vector<shared_ptr<const std::string> >&buffers); //return true if has binary buffers.

			std::string const& get_nsp() const;

			sio_message::ptr const& get_message() const;

			unsigned get_pack_id() const;

			static bool is_message(string const& payload_ptr);
			static bool is_text_message(string const& payload_ptr);
			static bool is_binary_message(string const& payload_ptr);

		private:
			frame_type _frame;
			int _type;
			std::string _nsp;
			int _pack_id;
			sio_message::ptr _message;
			unsigned _pending_buffers;
			vector<shared_ptr<const std::string> > _buffers;
		};

		/*class packet_manager
		{
		public:
		typedef function<void(bool, shared_ptr<const string> const&)> encode_callback_function;
		typedef  function<void(sio_packet const&)> decode_callback_function;

		void set_decode_callback(decode_callback_function const& decode_callback);

		void set_encode_callback(encode_callback_function const& encode_callback);

		void encode(sio_packet& pack, encode_callback_function const& override_encode_callback = encode_callback_function()) const;

		void put_payload(string const& payload);

		void reset();

		private:
		decode_callback_function m_decode_callback;

		encode_callback_function m_encode_callback;

		std::unique_ptr<sio_packet> m_partial_packet;
		};*/

	} // namespace sio

} // namespace mirazabal

#endif
