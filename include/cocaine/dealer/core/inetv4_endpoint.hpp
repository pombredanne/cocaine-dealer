/*
    Copyright (c) 2011-2012 Rim Zaidullin <creator@bash.org.ru>
    Copyright (c) 2011-2012 Other contributors as noted in the AUTHORS file.

    This file is part of Cocaine.

    Cocaine is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Cocaine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>. 
*/

#ifndef _COCAINE_DEALER_INETV4_ENDPOINT_HPP_INCLUDED_
#define _COCAINE_DEALER_INETV4_ENDPOINT_HPP_INCLUDED_

#include <string>
#include <map>

#include "boost/lexical_cast.hpp"

#include "cocaine/dealer/core/inetv4_host.hpp"

namespace cocaine {
namespace dealer {

enum transport_type {
	TRANSPORT_UNDEFINED = 0,
	TRANSPORT_INPROC,
	TRANSPORT_IPC,
	TRANSPORT_TCP,
	TRANSPORT_PGM,
	TRANSPORT_EPGM
};

class inetv4_endpoint_t {
public:
	inetv4_endpoint_t() :
		port(0) {
			init_transport_literals();
		}

	inetv4_endpoint_t(const inetv4_host_t& host_) :
		transport(TRANSPORT_UNDEFINED),
		host(host_),
		port(0) {
			init_transport_literals();
		}

	inetv4_endpoint_t(const inetv4_endpoint_t& rhs) :
		transport(rhs.transport),
		host(rhs.host),
		port(rhs.port) {
			init_transport_literals();
		}

	inetv4_endpoint_t(const inetv4_host_t& host_,
					  unsigned short port_,
					  enum transport_type transport_ = TRANSPORT_UNDEFINED) :
		transport(transport_),
		host(host_),
		port(port_) {
			init_transport_literals();
		}

	inetv4_endpoint_t(unsigned int ip_,
					  unsigned short port_,
					  enum transport_type transport_ = TRANSPORT_UNDEFINED) :
		transport(transport_),
		host(inetv4_host_t(ip_)),
		port(port_) {
			init_transport_literals();
		}

	inetv4_endpoint_t(const std::string& ip_,
					  const std::string& port_,
					  enum transport_type transport_ = TRANSPORT_UNDEFINED) :
		transport(transport_),
		host(inetv4_host_t(ip_))
	{
		init_transport_literals();
		port = boost::lexical_cast<unsigned short>(port_);
	}

	inetv4_endpoint_t(unsigned int ip_,
					  const std::string& port_,
					  enum transport_type transport_ = TRANSPORT_UNDEFINED) :
		transport(transport_),
		host(inetv4_host_t(ip_))
	{
		init_transport_literals();
		port = boost::lexical_cast<unsigned short>(port_);
	}

	bool operator == (const inetv4_endpoint_t& rhs) const {
		return (host == rhs.host &&
				port == rhs.port &&
				transport == rhs.transport);
	}

	bool operator != (const inetv4_endpoint_t& rhs) const {
		return (!(*this == rhs));
	}

	bool operator < (const inetv4_endpoint_t& rhs) const {
		return (as_string() < rhs.as_string());
	}

	std::string as_string() const {
		return as_connection_string() + " (" + host.hostname + ")";
	}

	std::string as_connection_string() const {
		std::string connection_string = transport_literals[transport];
		connection_string += "://" + nutils::ipv4_to_str(host.ip);
		connection_string += ":" + boost::lexical_cast<std::string>(port);

		return connection_string;
	}

	static enum transport_type transport_from_string(const std::string& transport_string) {
		init_transport_literals();

		std::map<std::string, enum transport_type>::iterator it;
		it = transport_back_literals.find(transport_string);

		if (it != transport_back_literals.end()) {
			return it->second;
		}

		return TRANSPORT_UNDEFINED;
	}

	static std::string string_from_transport(enum transport_type type) {
		init_transport_literals();
		
		std::map<enum transport_type, std::string>::iterator it;
		it = transport_literals.find(type);

		if (it != transport_literals.end()) {
			return it->second;
		}

		return "";
	}

	enum transport_type transport;
	inetv4_host_t		host;
	unsigned short		port;

private:
	static std::map<enum transport_type, std::string> transport_literals;
	static std::map<std::string, enum transport_type> transport_back_literals;

	static void init_transport_literals() {
		static bool initialized = false;

		if (initialized) {
			return;
		}

		transport_literals[TRANSPORT_UNDEFINED]	= "";
		transport_literals[TRANSPORT_INPROC]	= "inproc";
		transport_literals[TRANSPORT_IPC]		= "ipc";
		transport_literals[TRANSPORT_TCP]		= "tcp";
		transport_literals[TRANSPORT_PGM]		= "pgm";
		transport_literals[TRANSPORT_EPGM]		= "epgm";

		transport_back_literals[""]			= TRANSPORT_UNDEFINED;
		transport_back_literals["inproc"]	= TRANSPORT_INPROC;
		transport_back_literals["ipc"]		= TRANSPORT_IPC;
		transport_back_literals["tcp"]		= TRANSPORT_TCP;
		transport_back_literals["pgm"]		= TRANSPORT_PGM;
		transport_back_literals["epgm"]		= TRANSPORT_EPGM;

		initialized = true;
	}
};

} // namespace dealer
} // namespace cocaine

#endif // _COCAINE_DEALER_INETV4_ENDPOINT_HPP_INCLUDED_
