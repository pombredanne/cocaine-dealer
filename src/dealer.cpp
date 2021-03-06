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

#include <boost/current_function.hpp>

#include "cocaine/dealer/dealer.hpp"
#include "cocaine/dealer/core/dealer_impl.hpp"

#include "cocaine/dealer/utils/error.hpp"

namespace cocaine {
namespace dealer {

dealer_t::dealer_t(const std::string& config_path) {
	m_impl.reset(new dealer_impl_t(config_path));
}

dealer_t::~dealer_t() {
	m_impl.reset();
}

boost::shared_ptr<response_t>
dealer_t::send_message(const message_t& message) {
    return m_impl->send_message(message);   
}

boost::shared_ptr<response_t>
dealer_t::send_message(const void* data,
                       size_t size,
                       const message_path_t& path,
                       const message_policy_t& policy)
{
	return m_impl->send_message(data, size, path, policy);
}

boost::shared_ptr<response_t>
dealer_t::send_message(const void* data,
                       size_t size,
                       const message_path_t& path)
{
    return m_impl->send_message(data, size, path);
}

std::vector<boost::shared_ptr<response_t> >
dealer_t::send_messages(const void* data,
                        size_t size,
                        const message_path_t& path,
                        const message_policy_t& policy)
{
    return m_impl->send_messages(data, size, path, policy);
}

std::vector<boost::shared_ptr<response_t> >
dealer_t::send_messages(const void* data,
                        size_t size,
                        const message_path_t& path)
{
    return m_impl->send_messages(data, size, path);
}

boost::shared_ptr<response_t>
dealer_t::send_message(const std::string& data,
                       const message_path_t& path,
                       const message_policy_t& policy)
{
    return m_impl->send_message(data.data(), data.size(), path, policy);
}

boost::shared_ptr<response_t>
dealer_t::send_message(const std::string& data,
                       const message_path_t& path)
{
    return m_impl->send_message(data.data(), data.size(), path);
}

std::vector<boost::shared_ptr<response_t> >
dealer_t::send_messages(const std::string& data,
                        const message_path_t& path,
                        const message_policy_t& policy)
{
    return m_impl->send_messages(data.data(), data.size(), path, policy);
}

std::vector<boost::shared_ptr<response_t> >
dealer_t::send_messages(const std::string& data,
                        const message_path_t& path)
{
    return m_impl->send_messages(data.data(), data.size(), path);
}

message_policy_t
dealer_t::policy_for_service(const std::string& service_alias) {
    return m_impl->policy_for_service(service_alias);
}

size_t
dealer_t::stored_messages_count(const std::string& service_alias) {
    return m_impl->stored_messages_count(service_alias);
}

void
dealer_t::remove_stored_message(const message_t& message) {
    m_impl->remove_stored_message(message);   
}

void
dealer_t::remove_stored_message_for(const response_ptr_t& response) {
    m_impl->remove_stored_message_for(response);
}

void
dealer_t::get_stored_messages(const std::string& service_alias,
                              std::vector<message_t>& messages)
{
    m_impl->get_stored_messages(service_alias, messages);
}

} // namespace dealer
} // namespace cocaine
