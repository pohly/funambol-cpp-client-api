

#include "examples/MySyncItemListener.h"

void MySyncItemListener::itemAddedByServer(SyncItemEvent &event) {

	printf("SyncItemEvent occurred.\n");
	printf("Sync Item %s from sourceURI %s successfully added from server at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void MySyncItemListener::itemDeletedByServer(SyncItemEvent &event) {
                                                                                                 
        printf("SyncItemEvent occurred.\n");
        printf("Sync Item %s from sourceURI %s successfully deleted by server at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void MySyncItemListener::itemUpdatedByServer(SyncItemEvent &event) {
                                                                                                 
        printf("SyncItemEvent occurred.\n");
        printf("Sync Item %s from sourceURI %s successfully updated from server at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void MySyncItemListener::itemAddedByClient(SyncItemEvent &event) {
                                                                                                 
        printf("SyncItemEvent occurred.\n");
        printf("Sync Item %s from sourceURI %s successfully detected by client at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void MySyncItemListener::itemUpdatedByClient(SyncItemEvent &event) {
                                                                                                 
        printf("SyncItemEvent occurred.\n");
        printf("Sync Item %s from sourceURI %s updated by client at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void MySyncItemListener::itemDeletedByClient(SyncItemEvent &event) {
                                                                                                 
        printf("SyncItemEvent occurred.\n");
        printf("Sync Item %s from sourceURI %s deleted by Client at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}


