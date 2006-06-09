

#ifndef INCL_BASE_EVENT
    #define INCL_BASE_EVENT


/* 
	Abstract Base Event Class. All event subtypes need to extend this class
*/

	class BaseEvent {
	
		// Event code
		int type;

		// Time Stamp of when the event is generated
		unsigned long date;

	  public:
		
		// Constructor
		BaseEvent(int type, unsigned long date);	
		~BaseEvent();
	
		// set time stamp / date
		void setDate(unsigned long date);
	
		// get the event code
		int getType();
		
		// get the timestamp of the event
		unsigned long getDate();

	};

#endif

