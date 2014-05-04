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

int Client::SendAll(int sockfd, char *buf, int len, int flags) const
{
    size_t total_sent = 0;
    while (total_sent < len) {
        int current_sent = send(sockfd, buf + total_sent, len - total_sent, flags);
        if (current_sent == -1)
            return -1;
        total_sent += current_sent;
    }   
    return total_sent;
}

int Client::RecvAll(int sockfd, char *buf, int len, int flags) const
{
    size_t total_recv = 0;
    while (total_recv < len) {
        int current_recv = recv(sockfd, buf + total_recv, len - total_recv, flags);
        if (current_recv == -1)
            return -1;
        total_recv += current_recv;
    }   
    return total_recv;
}

//-------------------------------------------------------------------------------------------------

int main(int argc, char const *argv[]) {

    Client client;
    while (!client.Connection(1234)) {}

    // request to start a game
    //send(sock, message, sizeof(message), 0);



    // get data from server
    //FieldState field_state;



    // socket close
    //close(sockfd);

    return 0;
}
