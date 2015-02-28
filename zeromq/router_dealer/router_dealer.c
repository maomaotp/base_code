//  Task worker
//  Connects PULL socket to tcp://localhost:5557
//  Collects workloads from ventilator via that socket
//  Connects PUSH socket to tcp://localhost:5558
//  Sends results to sink via that socket

#include "zhelpers.h"

int main (void)
{
    //  Socket to receive messages on
    void *context = zmq_ctx_new ();
    void *router = zmq_socket (context, ZMQ_ROUTER);
    void *dealer = zmq_socket (context, ZMQ_PUSH);

    zmq_bind (router, "tcp://*:5557");
    zmq_bind (dealer, "tcp://*:5558");

	void *rep = zmq_socket(context, ZMQ_REP);
    zmq_connect (rep, "tcp://192.168.1.120:5558");
    char *string = s_recv (rep);
	s_send(rep, "hello");

	zmq_proxy (router, dealer, NULL);

    zmq_close (router);
    zmq_close (dealer);
    zmq_close (rep);
    zmq_ctx_destroy (context);
    return 0;
}
