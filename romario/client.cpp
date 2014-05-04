#include <iostream>
#include "client.h"

//-------------------------------------------------------------------------------------------------

Client::Client()
{
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ < 0) {
        perror("Failed to create socket\n");
        throw;
    }
}

Client::~Client()
{
    close(sockfd_);
}

bool Client::Connection(size_t port) const
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // INADDR_ANY [in production]
    if (connect(sockfd_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Connection failed");
        return false;
    }
    return true;
}

int Client::SendAll(char *buf, int len, int flags) const
{
    size_t total_sent = 0;
    while (total_sent < len) {
        int current_sent = send(sockfd_, buf + total_sent, len - total_sent, flags);
        if (current_sent == -1)
            return -1;
        total_sent += current_sent;
    }   
    return total_sent;
}

int Client::RecvAll(char *buf, int len, int flags) const
{
    size_t total_recv = 0;
    while (total_recv < len) {
        int current_recv = recv(sockfd_, buf + total_recv, len - total_recv, flags);
        if (current_recv == -1)
            return -1;
        total_recv += current_recv;
    }   
    return total_recv;
}

//-------------------------------------------------------------------------------------------------

Gamer::Gamer() {}
Gamer::~Gamer() {}

bool Gamer::StartGame()
{
    std::string start_game_request = "CLI_SUB_REQUEST";
    int send_result = client_.SendAll(start_game_request.c_str(), start_game_request.length(), 0);
    if (send_result == -1)
        return false;
    std::string start_game_answer;
    if (client_.RecvAll(start_game_answer, start_game_answer.length(), 0) == -1 ||
        start_game_answer == "FAIL")
    {
        return false;
    }
    id = start_game_answer;
    return true;
}

//-------------------------------------------------------------------------------------------------

int main(int argc, char const *argv[]) {

    size_t port = 1234;
    Gamer gamer;
    while (!gamer.client_.Connection(port)) {}

    // request to start a game
    gamer.StartGame();
    


    // get data from server
    //FieldState field_state;



    return 0;
}
