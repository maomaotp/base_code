
#include "czmq.h"

static void server_worker (zctx_t *ctx)
{
    void *worker = zsocket_new (ctx, ZMQ_DEALER);
    //zsocket_connect (worker, "inproc://backend");
    zsocket_connect (worker, "tcp:localhost//5571");

    while (true) {
        //  The DEALER socket gives us the reply envelope and message
        zmsg_t *msg = zmsg_recv (worker);
        zframe_t *identity = zmsg_pop (msg);
        zframe_t *content = zmsg_pop (msg);
        assert (content);
        zmsg_destroy (&msg);

        //  Send 0..4 replies back
        int reply, replies = randof (5);
        for (reply = 0; reply < replies; reply++) {
            //  Sleep for some fraction of a second
            zclock_sleep (randof (1000) + 1);
            zframe_send (&identity, worker, ZFRAME_REUSE + ZFRAME_MORE);
            zframe_send (&content, worker, ZFRAME_REUSE);
        }
        zframe_destroy (&identity);
        zframe_destroy (&content);
    }
}

int main()
{
	zctx_t *ctx = zctx_new ();
	server_worker(ctx);
	return 0;
}
