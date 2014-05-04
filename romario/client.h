#include <vector>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


//#include "strategy.h" [in production]
//#include "basics.h" [in production]
//#include "../permanentstudent/strategy.h"
//#include "../yazevnul/basics.h"


class Client
{
public:
    Client();
    ~Client();

    bool Connection(size_t port) const;

    int SendAll(char *buf, int len, int flags) const;
    int RecvAll(char *buf, int len, int flags) const;

private:
    int sockfd_;

};

class Gamer
{
public:
    Gamer();
    ~Gamer();

    bool StartGame();

private:
    Client client_;
    string id;
};
