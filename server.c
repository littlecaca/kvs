#include "server.h"

#include <assert.h>

#include "reactor.h"
#include "ntyco.h"
#include "kvs_protocol.h"

static int ntyco_handler(char *msg, int length, char *response)
{
    int nwrite;
    kvs_deal_request(msg, response, NTYCO_BUFFER_LENGTH, &nwrite);
    response[nwrite] = '\0';
    return nwrite;
}

void server_start(int port, EventHandler handler)
{
#if (NETWORK_SELECT == NETWORK_REACTOR)
    reactor_start(port, handler);
#elif (NETWORK_SELECT == NETWORK_NTYCO)
    ntyco_start(port, &ntyco_handler);
#else
    static_assert(0, "Must select a network engine");
#endif
}
