#pragma once

#if defined(__APPLE__) || defined(__MACH__)
# ifndef MSG_NOSIGNAL
#   define MSG_NOSIGNAL SO_NOSIGPIPE
# endif
#endif
