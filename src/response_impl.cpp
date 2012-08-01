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

#include <stdexcept>
#include <iostream>

#include <boost/current_function.hpp>
#include <boost/bind.hpp>

#include "cocaine/dealer/dealer.hpp"
#include <cocaine/dealer/core/response_impl.hpp>
#include <cocaine/dealer/core/dealer_impl.hpp>
#include <cocaine/dealer/utils/error.hpp>
#include <cocaine/dealer/utils/progress_timer.hpp>

namespace cocaine {
namespace dealer {

response_impl_t::response_impl_t(const std::string& uuid, const message_path_t& path) :
	m_uuid(uuid),
	m_path(path),
	m_response_finished(false),
	m_message_finished(false),
	m_caught_error(false)
{}

response_impl_t::~response_impl_t() {
	boost::mutex::scoped_lock lock(m_mutex);
	m_message_finished = true;
	m_response_finished = true;
	m_chunks.clear();
}

bool
response_impl_t::get(chunk_data& data, double timeout) {
	boost::mutex::scoped_lock lock(m_mutex);

	// no more chunks?
	if (m_message_finished && m_chunks.size() == 0) {
		if (!m_caught_error) {
			return false;
		}
	}

	// block until received callback
	if (!m_message_finished) {
		if (timeout < 0.0) {
			while (!m_response_finished) { // handle spurrious wakes
				m_cond_var.wait(lock);
			}
		}
		else {
			boost::system_time t = boost::get_system_time();
			t += boost::posix_time::milliseconds(timeout * 1000);

			progress_timer pt;

			while (!m_response_finished && pt.elapsed().as_double() < timeout) { // handle spurrious wakes
				m_cond_var.timed_wait(lock, t);
			}
		}

		if (m_response_finished) {
			m_response_finished = false;
		}
	}
	else {
		if (m_chunks.size() > 0) {
			data = m_chunks[0];
			m_chunks.erase(m_chunks.begin());
			return true;
		}
		else {
			m_message_finished = true;

			if (m_caught_error) {
				m_caught_error = false;
				throw dealer_error(static_cast<cocaine::dealer::error_code>(m_resp_info.code), m_resp_info.error_msg);
			}

			return false;
		}
	}

	if (timeout >= 0.0 && m_chunks.empty() && !m_caught_error) {
		return false;
	}

	// expecting another chunk
	if (m_chunks.size() > 0) {
		data = m_chunks[0];
		m_chunks.erase(m_chunks.begin());
		return true;
	}

	if (m_caught_error) {
		m_caught_error = false;
		throw dealer_error(static_cast<cocaine::dealer::error_code>(m_resp_info.code), m_resp_info.error_msg);
	}

	m_message_finished = true;
	return false;
}

void
response_impl_t::add_chunk(const chunk_data& data, const chunk_info& info) {
	boost::mutex::scoped_lock lock(m_mutex);

	if (m_message_finished) {
		return;
	}

	if (info.code == response_code::message_choke) {
		m_message_finished = true;
	}
	else if (info.code == response_code::message_chunk) {
		m_chunks.push_back(data);
	}
	else {
		m_caught_error = true;
		m_resp_info = info; // remember error data
		m_message_finished = true;
	}

	m_response_finished = true;

	lock.unlock();
	m_cond_var.notify_one();
}

} // namespace dealer
} // namespace cocaine
