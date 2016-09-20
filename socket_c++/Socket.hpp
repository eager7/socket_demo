
/*
 * Socket.hpp
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

#ifndef _SOCKET_HPP_

#define _SOCKET_HPP_

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>
#include <fstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define SOCKET_MAX_BUFFER_LEN 1024

using namespace std;

namespace mSocket
{
    typedef int SocketId;
    typedef string Ip;
    typedef unsigned int Port;
    
    class SocketException : public exception
    {
    private:
        string _error;
    
    public:
        SocketException(const string&);
        ~SocketException() throw();
	
	    virtual const char* what() const throw();
        friend ostream& operator<< (ostream &out, SocketException &e);
    };

    struct Address : protected sockaddr_in
    {
    private:
        void _address(string, unsigned int);

    public:
        Address();
        Address(unsigned int);
        Address(string, unsigned int);
        Address(struct sockaddr_in);
        Address(const Address&);

        string ip(void);
        string ip(string);

        unsigned int port(void);
        unsigned int port(unsigned int);

        friend ostream& operator<< (ostream&, Address&);
    };

    template <class DataType>
    struct Datagram
    {
    public:
        Address address;
        DataType data;
        unsigned int received_bytes;
        unsigned int received_elements;

        template <class T>
        void operator= (const Datagram<T>&);
    };

    class CommonSocket
    {
    protected:
        int _socket_id;
        int _socket_type;
        bool _opened;
        bool _binded;

    public:
        CommonSocket(void);
        CommonSocket(int);

        ~CommonSocket(void);

        void open(void);
        void close(void);
        
        virtual void listen_on_port(Port);
    };

    class UDP : public CommonSocket
    {
    public:
        UDP(void);
        UDP(const UDP&);
        
        template <class T> int send(string, unsigned int, const T*, size_t);
        template <class T> int send(Address, const T*, size_t);
        template <class T> int send(string, unsigned int, T);
        template <class T> int send(Address, T);
        template <class T> int send(string, unsigned int, vector<T>);
        template <class T> int send(Address, vector<T>);
        
        template <class T> int receive(Address*, T*, size_t, unsigned int*);
        template <class T> Datagram<T*> receive(T*, size_t len = SOCKET_MAX_BUFFER_LEN);
        template <class T, size_t N> Datagram<T[N]> receive(size_t len = N);
        template <class T> Datagram<T> receive(void);
        template <class T> Datagram<vector<T> > receive(size_t);
    };

    class TCP : public CommonSocket
    {
    private:
        Address _address;
    public:
        TCP(void);
        TCP(const TCP&);
        
        Ip ip(void);
        Port port(void);
        Address address(void);
        
        void listen_on_port(Port, unsigned int listeners = 1);
        void connect_to(Address);
        
        TCP accept_client(void);
        
        template <class T> int send(const T*, size_t);
        template <class T> int receive(T*, size_t);
        
        void send_file(string);
        void receive_file(string);
    };
}

#endif

