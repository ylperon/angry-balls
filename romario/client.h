#include <iostream>
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

    int SendAll(int sockfd, char *buf, int len, int flags) const;
    int RecvAll(int sockfd, char *buf, int len, int flags) const;

private:
    int sockfd_;

};
