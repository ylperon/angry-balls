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


class IOClient
{
public:
    IOClient();
    ~IOClient();

    bool Connection(size_t port) const;

    int SendAll(const std::string& buf, int flags) const;
    int RecvAll(std::string& buf, int flags) const;

private:
    int sockfd_;

};

class Gamer
{
public:
    Gamer();
    ~Gamer();

    bool StartGame(size_t port);

private:
    IOClient client_;
    std::string id_;
};
