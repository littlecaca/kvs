#include "server.h"
#include "reactor.h"

void server_start(int port, EventHandler *handler)
{
#ifdef NETWORK_REACTOR
    reactor_start(port, handler);
#endif
}