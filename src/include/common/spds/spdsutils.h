#ifndef INCL_SPDS_UTIL
#define INCL_SPDS_UTIL

#include "base/util/ArrayList.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncItemStatus.h"

SyncMode syncModeCode(const wchar_t* syncMode) EXTRA_SECTION_02;

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

int uudecode(const wchar_t *msg, char **binmsg, size_t *binlen);
wchar_t *uuencode(const char *msg, int len);

/* 
 * Convert the wchar buffer 'str' according to encoding and save
 * it in filename.
 */
int convertAndSave(const wchar_t *filename, const wchar_t *str,
                  const wchar_t *encoding = TEXT("UTF-8")) EXTRA_SECTION_02;

/* 
 * Load a file and convert its content according to encoding.
 */
wchar_t *loadAndConvert(const wchar_t *filename,
                        const wchar_t *encoding = TEXT("UTF-8")) EXTRA_SECTION_02;

#endif
