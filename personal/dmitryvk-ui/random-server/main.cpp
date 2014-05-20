#include "server.hpp"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;

int main (int argc, char * argv[]) {
  ServerOptions options;
  options.listen_port = 9011;
  Server server(options);
  int rc = server.run();
  return rc;
}
