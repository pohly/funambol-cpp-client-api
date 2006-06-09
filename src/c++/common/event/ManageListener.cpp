
#include "event/ManageListener.h"

/* Static Variables */

ManageListener * ManageListener::instance = 0;

// Private Methods
//Contructor and Destructor

ManageListener::ManageListener() {
	
	synclistener = NULL;
	transportlistener = NULL;
	syncitemlistener = NULL;
	syncstatuslistener = NULL;
	syncsourcelistener = NULL;
}

ManageListener::~ManageListener() {
		
	if(synclistener) {
		delete synclistener;
	}
	
	if(transportlistener) {
		delete transportlistener;
	}

	if(syncitemlistener) {

		delete syncitemlistener;
	}

	if(syncsourcelistener) {
		delete syncsourcelistener;
	}

	if(syncstatuslistener) {
		delete syncstatuslistener;
	}
}


//Public Methods

/*
 * Get, or create, ManageListener instance
 */
ManageListener& ManageListener::getInstance() {

	if(instance == NULL) {
		instance = new ManageListener();
	}

	return *instance;
}

void ManageListener::dispose() {

	if(instance) {
		delete instance;
	}

	instance = NULL;
}
