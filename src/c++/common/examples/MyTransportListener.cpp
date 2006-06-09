
#include "stdio.h"
#include "examples/MyTransportListener.h"

void MyTransportListener::sendDataBegin(TransportEvent &event) {

	printf("TransportEvent occurred.\n");
	printf("Sending Data of size %d began at %ld.\n\n",event.getDataSize(), event.getDate());
}

void MyTransportListener::receiveDataBegin(TransportEvent &event) {
                                                                                                 
        printf("TransportEvent occurred.\n");
        printf("Begun Receiving Data of size %d began at %ld.\n\n",event.getDataSize(), event.getDate());
}

void MyTransportListener::receivingData(TransportEvent &event) {
                                                                                                 
        printf("TransportEvent occurred.\n");
        printf("Receiving Data of size %d began at %ld.\n\n",event.getDataSize(), event.getDate());
}

void MyTransportListener::receiveDataEnd(TransportEvent &event) {
                                                                                                 
        printf("TransportEvent occurred.\n");
        printf("Receiving Data done of size %d began at %ld.\n\n",event.getDataSize(), event.getDate());
}

