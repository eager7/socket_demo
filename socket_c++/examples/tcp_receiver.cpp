#include "../Socket.hpp"

using namespace std;

int main(void)
{
    try
    {
        Socket::TCP server;
        
        server.listen_on_port(10000);
        Socket::TCP client = server.accept_client();
        
        cout << "receiving ..." << endl;
        client.receive_file("output.bmp");
    }
    catch (Socket::SocketException &e)
    {
        cout << e << endl;
    }
    
    return 0;
}
