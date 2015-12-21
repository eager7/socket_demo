
/*
 * Address.cpp
 * This file is part of VallauriSoft
 *
 * Copyright (C) 2012 - Comina Francesco
 *
 * VallauriSoft is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * VallauriSoft is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VallauriSoft; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef _ADDRESS_CPP_

#define _ADDRESS_CPP_

#include "Socket.hpp"

namespace Socket
{
    void Address::_address(Ip ip, Port port)
    {
        this->sin_family = AF_INET;
        this->ip(ip);
        this->port(port);
    }

    Address::Address()
    {
        _address("0.0.0.0", 0);
    }

    Address::Address(Port port)
    {
        _address("0.0.0.0", port);
    }

    Address::Address(Ip ip, Port port)
    {
        _address(ip, port);
    }

    Address::Address(struct sockaddr_in address)
    {
        _address(inet_ntoa(address.sin_addr), address.sin_port);
    }

    Address::Address(const Address &address)
    {
        this->sin_family = address.sin_family;
        this->sin_addr = address.sin_addr;
        this->sin_port = address.sin_port;
    }

    Ip Address::ip(void)
    {
        return inet_ntoa(this->sin_addr);
    }

    Ip Address::ip(Ip ip)
    {
#ifdef WINDOWS
        unsigned long address = inet_addr(ip.c_str());

        if (address == INADDR_NONE)
        {
            stringstream error;
            error << "[ip] with [ip=" << ip << "] Invalid ip address provided";
            throw SocketException(error.str());
        }
        else
        {
            this->sin_addr.S_un.S_addr = address;
        }
#else
        if (inet_aton(ip.c_str(), &this->sin_addr) == 0)
        {
            stringstream error;
            error << "[ip] with [ip=" << ip << "] Invalid ip address provided";
            throw SocketException(error.str());
        }
#endif
        return this->ip();
    }

    Port Address::port(void)
    {
        return ntohs(this->sin_port);
    }

    Port Address::port(Port port)
    {
        this->sin_port = htons(port);
        return this->port();
    }

    ostream& operator<< (ostream &out, Address &address)
    {
        out << address.ip() << ":" << address.port();
        return out;
    }
}

#endif
