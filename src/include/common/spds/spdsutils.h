#ifndef INCL_SPDS_UTIL
#define INCL_SPDS_UTIL

#include "base/util/ArrayList.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncItemStatus.h"

SyncMode syncModeCode(const char*  syncMode) EXTRA_SECTION_02;

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

/* 
 * Convert the wchar buffer 'str' according to encoding and save
 * it in filename.
 */
int convertAndSave(const char *filename, const char *str,
                  const char *encoding = "UTF-8") EXTRA_SECTION_02;

/* 
 * Load a file and convert its content according to encoding.
 */
char *loadAndConvert(const char *filename,
                        const char *encoding = "UTF-8") EXTRA_SECTION_02;

#endif
