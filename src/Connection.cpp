# include "Connection.h"
# include <iostream>
# include <string>

using namespace std;
using namespace sf;

ParchisRemote::ParchisRemote(){}

ParchisClient::ParchisClient(): ParchisRemote(){
}

ParchisServer::ParchisServer(): ParchisRemote(){
}

void ParchisRemote::sendGameParameters(int player, string name, BoardConfig init_board){
    Packet packet;
    packet << GAME_PARAMETERS;
    packet << player;
    packet << name;
    packet << init_board;
    Socket::Status status = socket.send(packet);
    if(status != Socket::Done)
    {
        throw runtime_error("Error sending message");
    }
    cout << "GAME_PARAMETERS sent" << endl;
    cout << "player: " << player << " name: " << name << " init_board: " << init_board << endl;
}



void ParchisRemote::sendTestMessage(string message)
{
    Packet packet;
    packet << TEST_MESSAGE;
    packet << message;
    Socket::Status status = socket.send(packet);
    if(status != Socket::Done)
    {
        throw runtime_error("Error sending message");
    }
    cout << "TEST_MESSAGE sent" << endl;
    cout << "Message: " << message << endl;
}

void ParchisRemote::sendParchisMove(int turn, color c_piece, int id_piece, int dice){
    Packet packet;
    packet << MOVED;
    packet << turn;
    packet << c_piece;
    packet << id_piece;
    packet << dice;
    Socket::Status status = socket.send(packet);
    if(status != Socket::Done)
    {
        cout << "Error sending message" << endl;
        socket.disconnect();
    }
    cout << "301 MOVED sent" << endl;
    cout << "Move: " << turn << " " << str(c_piece) << " " << id_piece << " " << dice << endl;
}

MessageKind ParchisRemote::receive(Packet & packet)
{
    if(!isConnected()) return ERROR_DISCONNECTED;
    packet.clear();
    Socket::Status status = socket.receive(packet);

    if(status != Socket::Done)
    {
        cout << "Error receiving message" << endl;
        socket.disconnect();
    }
    
    int type;
    packet >> type;

    Packet p2(packet);
    switch(type)
    {
        case NOP:
        break;
        case GAME_PARAMETERS:
        {
            cout << "101 GAME_PARAMETERS received" << endl;
            break;
        }
        case TEST_MESSAGE:
        {
            cout << "300 TEST_MESSAGE received" << endl;
            break;
        }
        case MOVED:
        {
            cout << "301 MOVED received" << endl;
            break;
        }
        case ERROR_DISCONNECTED:
        {
            cout << "400 ERROR_DISCONNECTED received" << endl;
            break;
        }
        default:
        {
            cout << "Received unknown message" << endl;
        }
    }

    return (MessageKind)type;

}

void ParchisRemote::analyzePacket(Packet & packet, const MessageKind & kind)
{
   switch(kind)
    {
        case TEST_MESSAGE:
        {
            string message = ParchisRemote::packet2message(packet);
            cout << "300 TEST_MESSAGE received: " << message << endl;
            break;
        }
        case MOVED:
        {
            color c_piece;
            int id_piece;
            int dice;
            int turn;
            
            ParchisRemote::packet2move(packet, turn, c_piece, id_piece, dice);
            cout << "301 MOVED received: " << turn << " " << str(c_piece) << " " << id_piece << " " << dice << endl;
            break;
        }
        default:
        {
            cout << "Received unknown message" << endl;
        }
    }
}


string ParchisRemote::packet2message(Packet & packet)
{
    string message;
    packet >> message;
    cout << "Message: " << message << endl;
    return message;
}

void ParchisRemote::packet2move(Packet & packet, int & turn, color & c_piece, int & id_piece, int & dice)
{
    packet >> turn;
    int cint_piece;
    packet >> cint_piece;
    c_piece = (color)cint_piece;
    packet >> id_piece;
    packet >> dice;
    cout << "Move: " << turn << " " << str(c_piece) << " " << id_piece << " " << dice << endl;
}

void ParchisRemote::packet2gameParameters(Packet & packet, int & player, string & name, BoardConfig & init_board){
    packet >> player;
    packet >> name;
    int cint_board;
    packet >> cint_board;
    init_board = (BoardConfig) cint_board;
    cout << "player: " << player << " name: " << name << " init_board: " << init_board << endl;
}

bool ParchisRemote::isConnected()
{
    Packet packet;
    packet << TEST_ALIVE;
    Socket::Status status = socket.send(packet);

    return (status == Socket::Done);
}


void ParchisClient::startClientConnection(const string & ip_addr, const int & port){
    Socket::Status status = socket.connect(ip_addr, port);

    if(status != Socket::Done){
        throw runtime_error("Could not connect to server");
    }

    cout << "Connected to server " << ip_addr << ":" << port << endl;

}
/*
void ParchisServer::startListening(const int & port){
    if(listener.listen(port) != Socket::Done){
        throw runtime_error("Could not listen to port");
    }

    cout << "Listening on port " << port << endl;
}
*/
void ParchisServer::acceptConnection(TcpListener & listener){
    //TcpSocket *socket = new TcpSocket();
    //cout << socket << endl;
    if(listener.accept(socket) != Socket::Done){
        throw runtime_error("Could not accept connection");
    }

    cout << "Accepted connection from " << socket.getRemoteAddress() << ":" << socket.getRemotePort() << endl;
}

