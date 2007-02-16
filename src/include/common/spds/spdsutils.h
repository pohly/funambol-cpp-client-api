#ifndef INCL_SPDS_UTIL
#define INCL_SPDS_UTIL

#include "base/util/ArrayList.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncItemStatus.h"

/**
 * returns the SyncMode corresponding to the string,
 * SYNC_NONE if string is invalid
 *
 * @param syncMode      one of the keywords valid as config parameter (see documentation of config)
 */
SyncMode syncModeCode(const char*  syncMode) EXTRA_SECTION_02;

/**
 * returns the config keyword corresponding to the sync mode,
 * empty string if invalid
 */
const char *syncModeKeyword(SyncMode syncMode) EXTRA_SECTION_02;

/*
 * Translates an ArrayList object into an array of SyncItemStatus*.
 *
 * @param items the item list
 */
SyncItemStatus** toSyncItemStatusArray(ArrayList& items) EXTRA_SECTION_02;

/*
 * Translates an ArrayList into an array of SyncItem*.
 *
 * @param items the item list
 */
SyncItem** toSyncItemArray(ArrayList& items) EXTRA_SECTION_02;

int uudecode(const char *msg, char **binmsg, size_t *binlen);
char *uuencode(const char *msg, int len);

/**
* It converts the content using the encoding specified. If all
* is correct it saves the converted content into a file.
* @param filename the file name in which the converted data is saved
* @param s the data that will be converted
* @param encoding the encoding used to convert to data
* @return -1 if the filename is NULL or s is NULL;
*          0 if all is correct
*/ 
int convertAndSave(const char *filename, const char *str,
                  const char *encoding = "UTF-8") EXTRA_SECTION_02;

/**
* Load a file and convert its content according to encoding.
* @param filename the file name in which the converted content is saved
* @param encoding the encoding that will be used
* @return the content of the file
*/ 
char *loadAndConvert(const char *filename,
                        const char *encoding = "UTF-8") EXTRA_SECTION_02;

#endif
