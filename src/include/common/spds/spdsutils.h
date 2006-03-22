#ifndef INCL_SPDS_UTIL
#define INCL_SPDS_UTIL

#include "base/util/ArrayList.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncItemStatus.h"

SyncMode syncModeCode(const BCHAR* syncMode) EXTRA_SECTION_02;

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

int uudecode(const BCHAR *msg, char **binmsg, size_t *binlen);
BCHAR *uuencode(const char *msg, int len);

/* 
 * Convert the wchar buffer 'str' according to encoding and save
 * it in filename.
 */
int convertAndSave(const BCHAR *filename, const BCHAR *str,
                  const BCHAR *encoding = T("UTF-8")) EXTRA_SECTION_02;

/* 
 * Load a file and convert its content according to encoding.
 */
BCHAR *loadAndConvert(const BCHAR *filename,
                        const BCHAR *encoding = T("UTF-8")) EXTRA_SECTION_02;

#endif
