

#include "examples/MySyncListener.h"

void MySyncListener::syncBegin(SyncEvent &event) {

	printf("SyncEvent occurred.\n");
	printf("Syncing successfully began at %ld.\n\n", event.getDate());
}

void MySyncListener::syncEnd(SyncEvent &event) {
                                                                                
        printf("SyncEvent occurred.\n");
        printf("Syncing successfully ended at %ld.\n\n", event.getDate());
}

void MySyncListener::sendInitialization( SyncEvent &event) {
                                                                                
        printf("SyncEvent occurred.\n");
        printf("Initializations done at %ld.\n\n", event.getDate());
}

void MySyncListener::sendModifications(SyncEvent &event) {
                                                                                
        printf("SyncEvent occurred.\n");
        printf("Modifications processed at %ld.\n\n", event.getDate());
}

void MySyncListener::sendFinalization(SyncEvent &event) {
                                                                                
        printf("SyncEvent occurred.\n");
        printf("final package set & processed at %ld.\n\n", event.getDate());
}

void MySyncListener::syncError(SyncEvent &event) {
                                                                                
        printf("SyncEvent occurred.\n");
        printf("Sync Error %s occured began at %ld.\n\n", event.getMessage(), event.getDate());
}


