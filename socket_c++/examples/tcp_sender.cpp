
#include "../Socket.hpp"

using namespace std;

#define IP   "127.0.0.1"
#define PORT 10000

int main(void)
{
    try
    {
        Socket::TCP server;
        server.connect_to(Socket::Address(IP, PORT));
        
        cout << "sending ..." << endl;
        server.send_file("input.bmp");
    }
    catch (Socket::SocketException &e)
    {
        cout << e << endl;
    }
    
    return 0;
}
