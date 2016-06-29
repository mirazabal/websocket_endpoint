#ifndef __SIO_MESSAGE_H__
#define __SIO_MESSAGE_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif


#include <string>
#include <memory>
#include <vector>
#include <map>
#include <cassert>
#include <type_traits>

namespace mirazabal
{
	namespace sio
	{

		using namespace std;

		class sio_message
		{
		public:
			//sio_message();
			//virtual ~sio_message();
		public:
			enum class flag
			{
				flag_integer,
				flag_double,
				flag_string,
				flag_binary,
				flag_array,
				flag_object,
				flag_boolean,
				flag_null
			};


			class list;

			flag get_flag() const
			{
				return _flag;
			}

			typedef shared_ptr<sio_message> ptr;

			virtual bool get_bool() const
			{
				assert(false);
				return false;
			}

			virtual int64_t get_int() const
			{
				assert(false);
				return 0;
			}

			virtual double get_double() const
			{
				assert(false);
				return 0;
			}

			virtual string const& get_string() const
			{
				assert(false);
				static string s_empty_string;
				s_empty_string.clear();
				return s_empty_string;
			}

			virtual shared_ptr<const string> const& get_binary() const
			{
				assert(false);
				static shared_ptr<const string> s_empty_binary;
				s_empty_binary = nullptr;
				return s_empty_binary;
			}

			virtual const vector<ptr>& get_vector() const
			{
				assert(false);
				static vector<ptr> s_empty_vector;
				s_empty_vector.clear();
				return s_empty_vector;
			}

			virtual vector<ptr>& get_vector()
			{
				assert(false);
				static vector<ptr> s_empty_vector;
				s_empty_vector.clear();
				return s_empty_vector;
			}

			virtual const map<string, sio_message::ptr>& get_map() const
			{
				assert(false);
				static map<string, sio_message::ptr> s_empty_map;
				s_empty_map.clear();
				return s_empty_map;
			}

			virtual map<string, sio_message::ptr>& get_map()
			{
				assert(false);
				static map<string, sio_message::ptr> s_empty_map;
				s_empty_map.clear();
				return s_empty_map;
			}
		private:
			flag _flag;

		protected:
			explicit sio_message(flag f) :_flag(f) {}
		};

		class null_message : public sio_message
		{
		protected:
			null_message()
				:sio_message(flag::flag_null)
			{
			}

		public:
			static sio_message::ptr create()
			{
				return ptr(new null_message());
			}
		};

		class bool_message : public sio_message
		{
			bool _v;

		protected:
			explicit bool_message(bool v)
				:sio_message(flag::flag_boolean), _v(v)
			{
			}

		public:
			static sio_message::ptr create(bool v)
			{
				return ptr(new bool_message(v));
			}

			bool get_bool() const
			{
				return _v;
			}
		};

		class int_message : public sio_message
		{
			int64_t _v;
		protected:
			explicit int_message(int64_t v)
				:sio_message(flag::flag_integer), _v(v)
			{
			}

		public:
			static sio_message::ptr create(int64_t v)
			{
				return ptr(new int_message(v));
			}

			int64_t get_int() const
			{
				return _v;
			}

			double get_double() const//add double accessor for integer.
			{
				return static_cast<double>(_v);
			}
		};

		class double_message : public sio_message
		{
			double _v;
			explicit double_message(double v)
				:sio_message(flag::flag_double), _v(v)
			{
			}

		public:
			static sio_message::ptr create(double v)
			{
				return ptr(new double_message(v));
			}

			double get_double() const
			{
				return _v;
			}
		};

		class string_message : public sio_message
		{
			string _v;
			explicit string_message(string const& v)
				:sio_message(flag::flag_string), _v(v)
			{

			}

			explicit string_message(string&& v)
				:sio_message(flag::flag_string), _v(move(v))
			{
			}

		public:
			static sio_message::ptr create(string const& v)
			{
				return ptr(new string_message(v));
			}

			//virtual ~string_message();
			static sio_message::ptr create(string&& v)
			{
				return ptr(new string_message(move(v)));
			}

			string const& get_string() const
			{
				return _v;
			}
		};

		class binary_message : public sio_message
		{
			shared_ptr<const string> _v;
			explicit binary_message(shared_ptr<const string> const& v)
				:sio_message(flag::flag_binary), _v(v)
			{
			}
		public:
			static sio_message::ptr create(shared_ptr<const string> const& v)
			{
				return ptr(new binary_message(v));
			}

			shared_ptr<const string> const& get_binary() const
			{
				return _v;
			}
		};

		class array_message : public sio_message
		{
			vector<sio_message::ptr> _v;
			array_message() :sio_message(flag::flag_array)
			{
			}

		public:
			static sio_message::ptr create()
			{
				return ptr(new array_message());
			}
			//virtual ~array_message();

			void push(sio_message::ptr const& sio_message)
			{
				if (sio_message)
					_v.push_back(sio_message);
			}

			void push(const string& text)
			{
				_v.push_back(string_message::create(text));
			}

			void push(string&& text)
			{
				_v.push_back(string_message::create(move(text)));
			}

			void push(shared_ptr<string> const& binary)
			{
				if (binary)
					_v.push_back(binary_message::create(binary));
			}

			void push(shared_ptr<const string> const& binary)
			{
				if (binary)
					_v.push_back(binary_message::create(binary));
			}

			void insert(size_t pos, sio_message::ptr const& sio_message)
			{
				_v.insert(_v.begin() + pos, sio_message);
			}

			void insert(size_t pos, const string& text)
			{
				_v.insert(_v.begin() + pos, string_message::create(text));
			}

			void insert(size_t pos, string&& text)
			{
				_v.insert(_v.begin() + pos, string_message::create(move(text)));
			}

			void insert(size_t pos, shared_ptr<string> const& binary)
			{
				if (binary)
					_v.insert(_v.begin() + pos, binary_message::create(binary));
			}

			void insert(size_t pos, shared_ptr<const string> const& binary)
			{
				if (binary)
					_v.insert(_v.begin() + pos, binary_message::create(binary));
			}

			size_t size() const
			{
				return _v.size();
			}

			const sio_message::ptr& at(size_t i) const
			{
				return _v[i];
			}

			const sio_message::ptr& operator[] (size_t i) const
			{
				return _v[i];
			}

			vector<ptr>& get_vector()
			{
				return _v;
			}

			const vector<ptr>& get_vector() const
			{
				return _v;
			}
		};

		class object_message : public sio_message
		{
			map<string, sio_message::ptr> _v;
			object_message() : sio_message(flag::flag_object)
			{
			}
		public:
			static sio_message::ptr create()
			{
				return ptr(new object_message());
			}

			void insert(const string & key, sio_message::ptr const& sio_message)
			{
				_v[key] = sio_message;
			}

			void insert(const string & key, const string& text)
			{
				_v[key] = string_message::create(text);
			}

			void insert(const string & key, string&& text)
			{
				_v[key] = string_message::create(move(text));
			}

			void insert(const string & key, shared_ptr<string> const& binary)
			{
				if (binary)
					_v[key] = binary_message::create(binary);
			}

			void insert(const string & key, shared_ptr<const string> const& binary)
			{
				if (binary)
					_v[key] = binary_message::create(binary);
			}

			bool has(const string & key)
			{
				return _v.find(key) != _v.end();
			}

			const sio_message::ptr& at(const string & key) const
			{
				static shared_ptr<sio_message> not_found;

				map<string, sio_message::ptr>::const_iterator it = _v.find(key);
				if (it != _v.end()) return it->second;
				return not_found;
			}

			const sio_message::ptr& operator[] (const string & key) const
			{
				return at(key);
			}

			bool has(const string & key) const
			{
				return _v.find(key) != _v.end();
			}

			map<string, sio_message::ptr>& get_map()
			{
				return _v;
			}

			const map<string, sio_message::ptr>& get_map() const
			{
				return _v;
			}
		};

		class sio_message::list
		{
		public:
			list()
			{
			}

			explicit list(nullptr_t)
			{
			}

			explicit list(sio_message::list&& rhs) :
				m_vector(std::move(rhs.m_vector))
			{

			}

			list & operator= (const sio_message::list && rhs)
			{
				m_vector = std::move(rhs.m_vector);
				return *this;
			}

			template <typename T>
			list(T&& content,
				typename enable_if<is_same<vector<sio_message::ptr>, typename remove_reference<T>::type>::value>::type* = 0) :
				m_vector(std::forward<T>(content))
			{
			}

			explicit list(sio_message::list const& rhs) :
				m_vector(rhs.m_vector)
			{

			}

			explicit list(sio_message::ptr const& sio_message)
			{
				if (sio_message)
					m_vector.push_back(sio_message);

			}

			explicit list(const string& text)
			{
				m_vector.push_back(string_message::create(text));
			}

			explicit list(string&& text)
			{
				m_vector.push_back(string_message::create(move(text)));
			}

			explicit list(shared_ptr<string> const& binary)
			{
				if (binary)
					m_vector.push_back(binary_message::create(binary));
			}

			explicit list(shared_ptr<const string> const& binary)
			{
				if (binary)
					m_vector.push_back(binary_message::create(binary));
			}

			void push(sio_message::ptr const& sio_message)
			{
				if (sio_message)
					m_vector.push_back(sio_message);
			}

			void push(const string& text)
			{
				m_vector.push_back(string_message::create(text));
			}

			void push(string&& text)
			{
				m_vector.push_back(string_message::create(move(text)));
			}

			void push(shared_ptr<string> const& binary)
			{
				if (binary)
					m_vector.push_back(binary_message::create(binary));
			}

			void push(shared_ptr<const string> const& binary)
			{
				if (binary)
					m_vector.push_back(binary_message::create(binary));
			}

			void insert(size_t pos, sio_message::ptr const& sio_message)
			{
				m_vector.insert(m_vector.begin() + pos, sio_message);
			}

			void insert(size_t pos, const string& text)
			{
				m_vector.insert(m_vector.begin() + pos, string_message::create(text));
			}

			void insert(size_t pos, string&& text)
			{
				m_vector.insert(m_vector.begin() + pos, string_message::create(move(text)));
			}

			void insert(size_t pos, shared_ptr<string> const& binary)
			{
				if (binary)
					m_vector.insert(m_vector.begin() + pos, binary_message::create(binary));
			}

			void insert(size_t pos, shared_ptr<const string> const& binary)
			{
				if (binary)
					m_vector.insert(m_vector.begin() + pos, binary_message::create(binary));
			}

			size_t size() const
			{
				return m_vector.size();
			}

			const sio_message::ptr& at(size_t i) const
			{
				return m_vector[i];
			}

			const sio_message::ptr& operator[] (size_t i) const
			{
				return m_vector[i];
			}

			sio_message::ptr to_array_message(string const& event_name) const
			{
				sio_message::ptr arr = array_message::create();
				arr->get_vector().push_back(string_message::create(event_name));
				arr->get_vector().insert(arr->get_vector().end(), m_vector.begin(), m_vector.end());
				return arr;
			}

			sio_message::ptr to_array_message() const
			{
				sio_message::ptr arr = array_message::create();
				arr->get_vector().insert(arr->get_vector().end(), m_vector.begin(), m_vector.end());
				return arr;
			}

		private:
			vector<sio_message::ptr> m_vector;
		};

	}
}

#endif
