

#include "examples/MySyncStatusListener.h"

void MySyncStatusListener::statusSending(SyncStatusEvent &event) {

	printf("SyncStatusEvent occurred.\n");
	printf("Client Status %s with code %d for Server at %ld.\n\n", event.getCommand(), event.getStatusCode(), event.getDate());
}
