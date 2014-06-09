#include "server.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

void print_usage(const string& arg0) {
  cout << "Usage: " << arg0 << " [--help] [--port 9011]" << endl;
}

ServerOptions parse_options(const vector<string>& args, bool& help_was_requested) {
  ServerOptions result;
  result.listen_port = 9011;
  auto it = args.begin();
  help_was_requested = false;
  while (it < args.end()) {
    const string& cur_arg = *it++;
    if (cur_arg == "--help") {
      help_was_requested = true;
      return result;
    } if (cur_arg == "--port") {
      result.listen_port = std::stoul(*it++);
    }
  }
  return result;
}

int main (int argc, char * argv[]) {
  bool help_was_requested;
  ServerOptions options = parse_options(vector<string>(&argv[1], &argv[argc]), help_was_requested);;
  if (help_was_requested) {
    print_usage(argv[0]);
    return 1;
  }
  Server server(options);
  int rc = server.run();
  return rc;
}
