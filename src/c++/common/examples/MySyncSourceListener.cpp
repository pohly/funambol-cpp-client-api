
#include <stdio.h>
#include "examples/MySyncSourceListener.h"

void MySyncSourceListener::syncSourceBegin(SyncSourceEvent &event) {

	printf("SyncSourceEvent occurred.\n");
	printf("Syncing Source %s in syncmode %d successfully began at %ld.\n\n", event.getSourceURI(), event.getSyncMode(), event.getDate());
}

void MySyncSourceListener::syncSourceEnd(SyncSourceEvent &event) {
                                                                                
        printf("SyncSourceEvent occurred.\n");
	 printf("Syncing Source %s in syncmode %d successfully ended at %ld.\n\n", event.getSourceURI(), event.getSyncMode(), event.getDate());
}


