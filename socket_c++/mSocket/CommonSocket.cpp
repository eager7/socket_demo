
/*
 * CommonSocket.cpp
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

#include "mSocket.hpp"

namespace mSocket
{
    CommonSocket::CommonSocket(void) {
    }

    CommonSocket::CommonSocket(int socket_type) {
        this->_socket_type = socket_type;
        this->_opened = false;
        this->_binded = false;
        this->open();
    }

    CommonSocket::~CommonSocket(void) {
    }

    void CommonSocket::open(void) {
        if (!this->_opened) {
            if ((this->_socket_id = socket(AF_INET, this->_socket_type, 0)) == -1)
                throw SocketException("[open] Cannot create socket");
            this->_opened = true;
            this->_binded = false;
        }
    }

    void CommonSocket::close(void) {
        if (this->_opened)
            shutdown(this->_socket_id, SHUT_RDWR);

        this->_opened = false;
        this->_binded = false;
    }
    
    void CommonSocket::listen_on_port(unsigned int port) {
        if (this->_binded)
            throw SocketException("[listen_on_port] Socket already binded to a port, close the socket before to re-bind");

        if (!this->_opened) this->open();

        Address address(port);

        if (bind(this->_socket_id, (struct sockaddr*)&address, sizeof(struct sockaddr)) == -1) {
            stringstream error;
            error << "[listen_on_port] with [port=" << port << "] Cannot bind socket";
            throw SocketException(error.str());
        }

        this->_binded = true;
    }
}

