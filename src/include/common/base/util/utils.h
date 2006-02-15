/*
 * Copyright (C) 2003-2006 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

    #ifndef INCL_BASE_UTILS
    #define INCL_BASE_UTILS

    #include "base/fscapi.h"
    #include "base/util/ArrayList.h"
    #include "base/md5.h"
    #include "base/base64.h"

    // Default len for stringdup (means: use source string len)
    #define STRINGDUP_NOLEN 0xFFFFFFFF

    #define B64_ENCODING        TEXT("b64")
    #define TEXT_PLAIN_ENCODING TEXT("text/plain")


	struct codepage {
		const wchar_t *name;
		int  codepage_id;
	};

	static codepage encodings[] = {
		{ TEXT("_iso-2022-jp"), 50221 },
		{ TEXT("_iso-2022-jp$sio"), 50222 },
		{ TEXT("ansi_x3.4-1968"), 1252 },
		{ TEXT("ansi_x3.4-1986"), 1252 },
		{ TEXT("arabic"), 28596 },
		{ TEXT("ascii"), 1252 },
		{ TEXT("asmo-708"), 708 },
		{ TEXT("big5"), 950 },
		{ TEXT("chinese"), 936 },
		{ TEXT("cn-big5"), 950 },
		{ TEXT("cn-gb"), 936 },
		{ TEXT("codepage437"), 437 },
		{ TEXT("cp1026"), 1026 },
		{ TEXT("cp1256"), 1256 },
		{ TEXT("cp367"), 1252 },
		{ TEXT("cp437"), 437 },
		{ TEXT("cp500"), 775 },
		{ TEXT("cp819"), 1252 },
		{ TEXT("cp852"), 852 },
		{ TEXT("cp866"), 866 },
		{ TEXT("cp870"), 870 },
		{ TEXT("csascii"), 1252 },
		{ TEXT("csbig5"), 950 },
		{ TEXT("cseuckr"), 51949 },
		{ TEXT("cseucpkdfmtjapanese"), 51932 },
		{ TEXT("csgb2312"), 936 },
		{ TEXT("csgb231280"), 936 },
		{ TEXT("csiso"), 28591 },
		{ TEXT("csiso2022jp"), 50221 },
		{ TEXT("csiso2022kr"), 50225 },
		{ TEXT("csiso58gb231280"), 936 },
		{ TEXT("csisolatin2"), 28592 },
		{ TEXT("csisolatin4"), 28594 },
		{ TEXT("csisolatin5"), 28595 },
		{ TEXT("csisolatinarabic"), 28596 },
		{ TEXT("csisolatincyrillic"), 28595 },
		{ TEXT("csisolatingreek"), 28597 },
		{ TEXT("csisolatinhebrew"), 28598 },
		{ TEXT("cskoi8r"), 20866 },
		{ TEXT("csksc56011987"), 949 },
		{ TEXT("cspc8"), 437 },
		{ TEXT("csshiftjis"), 932 },
		{ TEXT("csunicode11utf7"), 65000 },
		{ TEXT("cswindows31j"), 932 },
		{ TEXT("cyrillic"), 28595 },
		{ TEXT("dos-720"), 720 },
		{ TEXT("dos-862"), 862 },
		{ TEXT("dos-874"), 874 },
		{ TEXT("ebcdic-cp-us"), 37 },
		{ TEXT("ecma-114"), 28596 },
		{ TEXT("ecma-118"), 28597 },
		{ TEXT("elot_928"), 28597 },
		{ TEXT("euc-cn"), 51936 },
		{ TEXT("euc-jp"), 51932 },
		{ TEXT("euc-kr"), 51949 },
		{ TEXT("gb2312"), 936 },
		{ TEXT("gb2312-80"), 936 },
		{ TEXT("gb231280"), 936 },
		{ TEXT("gb_2312-80"), 936 },
		{ TEXT("gbk"), 936 },
		{ TEXT("greek"), 28597 },
		{ TEXT("greek8"), 28597 },
		{ TEXT("hebrew"), 28598 },
		{ TEXT("hz-gb-2312"), 52936 },
		{ TEXT("ibm367"), 1252 },
		{ TEXT("ibm437"), 437 },
		{ TEXT("ibm737"), 737 },
		{ TEXT("ibm775"), 775 },
		{ TEXT("ibm819"), 1252 },
		{ TEXT("ibm850"), 850 },
		{ TEXT("ibm852"), 852 },
		{ TEXT("ibm857"), 857 },
		{ TEXT("ibm861"), 861 },
		{ TEXT("ibm866"), 866 },
		{ TEXT("ibm869"), 869 },
		{ TEXT("iso-2022-jp"), 50222 },
		{ TEXT("iso-2022-kr"), 50225 },
		{ TEXT("iso-8859-1"), 1252 },
		{ TEXT("iso-8859-11"), 874 },
		{ TEXT("iso-8859-15"), 28605 },
		{ TEXT("iso-8859-2"), 28592 },
		{ TEXT("iso-8859-3"), 28593 },
		{ TEXT("iso-8859-4"), 28594 },
		{ TEXT("iso-8859-5"), 28595 },
		{ TEXT("iso-8859-6"), 28596 },
		{ TEXT("iso-8859-7"), 28597 },
		{ TEXT("iso-8859-8"), 1255 },
		{ TEXT("iso-8859-8-i"), 38598 },
		{ TEXT("iso-8859-9"), 1254 },
		{ TEXT("iso-ir-100"), 1252 },
		{ TEXT("iso-ir-101"), 28592 },
		{ TEXT("iso-ir-109"), 28593 },
		{ TEXT("iso-ir-110"), 28594 },
		{ TEXT("iso-ir-126"), 28597 },
		{ TEXT("iso-ir-127"), 28596 },
		{ TEXT("iso-ir-138"), 28598 },
		{ TEXT("iso-ir-144"), 28595 },
		{ TEXT("iso-ir-148"), 1254 },
		{ TEXT("iso-ir-149"), 949 },
		{ TEXT("iso-ir-58"), 936 },
		{ TEXT("iso-ir-6"), 1252 },
		{ TEXT("iso-ir-6us"), 20127 },
		{ TEXT("iso646-us"), 1252 },
		{ TEXT("iso8859-1"), 1252 },
		{ TEXT("iso8859-2"), 28592 },
		{ TEXT("iso_646.irv:1991"), 1252 },
		{ TEXT("iso_8859-1"), 1252 },
		{ TEXT("iso_8859-15"), 28605 },
		{ TEXT("iso_8859-1:1987"), 1252 },
		{ TEXT("iso_8859-2"), 28592 },
		{ TEXT("iso_8859-2:1987"), 28592 },
		{ TEXT("iso_8859-3"), 28593 },
		{ TEXT("iso_8859-3:1988"), 28593 },
		{ TEXT("iso_8859-4"), 28594 },
		{ TEXT("iso_8859-4:1988"), 28594 },
		{ TEXT("iso_8859-5"), 28595 },
		{ TEXT("iso_8859-5:1988"), 28595 },
		{ TEXT("iso_8859-6"), 28596 },
		{ TEXT("iso_8859-6:1987"), 28596 },
		{ TEXT("iso_8859-7"), 28597 },
		{ TEXT("iso_8859-7:1987"), 28597 },
		{ TEXT("iso_8859-8"), 28598 },
		{ TEXT("iso_8859-8-i"), 1255 },
		{ TEXT("iso_8859-8:1988"), 28598 },
		{ TEXT("iso_8859-9"), 1254 },
		{ TEXT("iso_8859-9:1989"), 1254 },
		{ TEXT("johab"), 1361 },
		{ TEXT("koi"), 20866 },
		{ TEXT("koi8"), 20866 },
		{ TEXT("koi8-r"), 20866 },
		{ TEXT("koi8-ru"), 21866 },
		{ TEXT("koi8-u"), 21866 },
		{ TEXT("koi8r"), 20866 },
		{ TEXT("korean"), 949 },
		{ TEXT("ks_c_5601"), 949 },
		{ TEXT("ks_c_5601-1987"), 949 },
		{ TEXT("ks_c_5601-1989"), 949 },
		{ TEXT("ks_c_5601_1987"), 949 },
		{ TEXT("ksc5601"), 949 },
		{ TEXT("ksc_5601"), 949 },
		{ TEXT("l1"), 28591 },
		{ TEXT("l2"), 28592 },
		{ TEXT("l3"), 28593 },
		{ TEXT("l4"), 28594 },
		{ TEXT("l5"), 28599 },
		{ TEXT("l9"), 28605 },
		{ TEXT("latin1"), 1252 },
		{ TEXT("latin2"), 28592 },
		{ TEXT("latin3"), 28593 },
		{ TEXT("latin4"), 28594 },
		{ TEXT("latin5"), 1254 },
		{ TEXT("latin9"), 28605 },
		{ TEXT("logical"), 38598 },
		{ TEXT("macintosh"), 10000 },
		{ TEXT("ms_kanji"), 932 },
		{ TEXT("shift-jis"), 932 },
		{ TEXT("shift_jis"), 932 },
		{ TEXT("tis-620"), 874 },
		{ TEXT("unicode"), 1200 },
		{ TEXT("unicode-1-1-utf-7"), 65000 },
		{ TEXT("unicode-1-1-utf-8"), 65001 },
		{ TEXT("unicode-2-0-utf-8"), 65001 },
		{ TEXT("unicodefffe"), 1201 },
		{ TEXT("us"), 1252 },
		{ TEXT("us-ascii"), 1252 },
		{ TEXT("utf-16"), 1200 },
		{ TEXT("utf-7"), 65000 },
		{ TEXT("utf-8"), 65001 },
		{ TEXT("visual"), 1255 },
		{ TEXT("windows-1250"), 1250 },
		{ TEXT("windows-1251"), 1251 },
		{ TEXT("windows-1252"), 1252 },
		{ TEXT("windows-1253"), 1253 },
		{ TEXT("windows-1254"), 1254 },
		{ TEXT("windows-1255"), 1255 },
		{ TEXT("windows-1256"), 1256 },
		{ TEXT("windows-1257"), 1257 },
		{ TEXT("windows-1258"), 1258 },
		{ TEXT("windows-874"), 874 },
		{ TEXT("x-ansi"), 1252 },
		{ TEXT("x-chinese-cns"), 20000 },
		{ TEXT("x-chinese-eten"), 20002 },
		{ TEXT("x-cp1250"), 1250 },
		{ TEXT("x-cp1251"), 1251 },
		{ TEXT("x-ebcdic-arabic"), 20420 },
		{ TEXT("x-ebcdic-cp-us-euro"), 1140 },
		{ TEXT("x-ebcdic-cyrillicrussian"), 20880 },
		{ TEXT("x-ebcdic-cyrillicserbianbulgarian"), 21025 },
		{ TEXT("x-ebcdic-denmarknorway"), 20277 },
		{ TEXT("x-ebcdic-denmarknorway-euro"), 1142 },
		{ TEXT("x-ebcdic-finlandsweden"), 20278 },
		{ TEXT("x-ebcdic-finlandsweden-euro"), 1143 },
		{ TEXT("x-ebcdic-france"), 1143 },
		{ TEXT("x-ebcdic-france-euro"), 1147 },
		{ TEXT("x-ebcdic-germany"), 20273 },
		{ TEXT("x-ebcdic-germany-euro"), 1141 },
		{ TEXT("x-ebcdic-greek"), 20423 },
		{ TEXT("x-ebcdic-greekmodern"), 875 },
		{ TEXT("x-ebcdic-hebrew"), 20424 },
		{ TEXT("x-ebcdic-icelandic"), 20871 },
		{ TEXT("x-ebcdic-icelandic-euro"), 1149 },
		{ TEXT("x-ebcdic-international-euro"), 1148 },
		{ TEXT("x-ebcdic-italy"), 20280 },
		{ TEXT("x-ebcdic-italy-euro"), 1144 },
		{ TEXT("x-ebcdic-japaneseandjapaneselatin"), 50939 },
		{ TEXT("x-ebcdic-japaneseandkana"), 50930 },
		{ TEXT("x-ebcdic-japaneseanduscanada"), 50931 },
		{ TEXT("x-ebcdic-japanesekatakana"), 20290 },
		{ TEXT("x-ebcdic-koreanandkoreanextended"), 50933 },
		{ TEXT("x-ebcdic-koreanextended"), 20833 },
		{ TEXT("x-ebcdic-simplifiedchinese"), 50935 },
		{ TEXT("x-ebcdic-spain"), 20284 },
		{ TEXT("x-ebcdic-spain-euro"), 1145 },
		{ TEXT("x-ebcdic-thai"), 20838 },
		{ TEXT("x-ebcdic-traditionalchinese"), 50937 },
		{ TEXT("x-ebcdic-turkish"), 20905 },
		{ TEXT("x-ebcdic-uk"), 20285 },
		{ TEXT("x-ebcdic-uk-euro"), 1146 },
		{ TEXT("x-euc"), 51932 },
		{ TEXT("x-euc-cn"), 51936 },
		{ TEXT("x-euc-jp"), 51932 },
		{ TEXT("x-europa"), 29001 },
		{ TEXT("x-ia5"), 20105 },
		{ TEXT("x-ia5-german"), 20106 },
		{ TEXT("x-ia5-norwegian"), 20108 },
		{ TEXT("x-ia5-swedish"), 20107 },
		{ TEXT("x-iscii-as"), 57006 },
		{ TEXT("x-iscii-be"), 57003 },
		{ TEXT("x-iscii-de"), 57002 },
		{ TEXT("x-iscii-gu"), 57010 },
		{ TEXT("x-iscii-ka"), 57008 },
		{ TEXT("x-iscii-ma"), 57009 },
		{ TEXT("x-iscii-or"), 57007 },
		{ TEXT("x-iscii-pa"), 57011 },
		{ TEXT("x-iscii-ta"), 57004 },
		{ TEXT("x-iscii-te"), 57005 },
		{ TEXT("x-mac-arabic"), 10004 },
		{ TEXT("x-mac-ce"), 10029 },
		{ TEXT("x-mac-chinesesimp"), 10008 },
		{ TEXT("x-mac-chinesetrad"), 10002 },
		{ TEXT("x-mac-cyrillic"), 10007 },
		{ TEXT("x-mac-greek"), 10006 },
		{ TEXT("x-mac-hebrew"), 10005 },
		{ TEXT("x-mac-icelandic"), 10079 },
		{ TEXT("x-mac-japanese"), 10001 },
		{ TEXT("x-mac-korean"), 10003 },
		{ TEXT("x-mac-turkish"), 10081 },
		{ TEXT("x-ms-cp932"), 932 },
		{ TEXT("x-sjis"), 932 },
		{ TEXT("x-unicode-2-0-utf-7"), 65000 },
		{ TEXT("x-unicode-2-0-utf-8"), 65001 },
		{ TEXT("x-x-big5"), 950 }
	};

    /*
     * Deletes the given wchar_t*[] buffer if it is not NULL
     * and sets the pointer to NULL
     *
     */
    void safeDelete(wchar_t* p[]) EXTRA_SECTION_00;

    void safeDel(wchar_t** p) EXTRA_SECTION_00;

    void timestampToAnchor(unsigned long timestamp, wchar_t* anchor) EXTRA_SECTION_00;

    wchar_t* stringdup(const wchar_t* s, size_t len = STRINGDUP_NOLEN) EXTRA_SECTION_00 ;
    
    wchar_t* wcstolower(const wchar_t *s) EXTRA_SECTION_00;

    wchar_t* wcstoupper(const wchar_t *s) EXTRA_SECTION_00;

    /**
     * Returns TRUE is the given string is NULL or zero-length
     */
    inline BOOL isEmpty(const wchar_t* s) {
        return ((s == NULL) || (wcslen(s) == 0));
    }

    /**
     * Returns TRUE is the given string is NULL or zero-length
     */
    inline BOOL isNotEmpty(const wchar_t* s) {
        return (s && (wcslen(s) > 0));
    }

    /**
     * Returns the number of bytes required to store the UTF-8 string
     * corresponding to the given wchar_t string. If the string could not be
     * converted, -1 is returned.
     *
     * @param s the original string
     */
    long int utf8len(const wchar_t* s) EXTRA_SECTION_00;

    /**
     * Converts the given wchar string in UTF-8. The real implementation
     * is platform specific and should go in <platform>adapter.cpp.
     * It is supposed that the destination buffer is either big enough to
     * contain the converted data or NULL; in the latter case the needed
     * space is allocated using the new C++ operator and must be discarded
     * by the caller with the delete operator.
     * This function return the pointer to the destination buffer (d or the new
     * allocated one) in case of success or NULL in case the conversion was not
     * possible.
     *
     * @param s source string
     * @param d destination buffer or NULL
     * @param dsize size of the destination buffer (ignored if d is null)
     *
     */
    char* wc2utf8(const wchar_t* s, char* d = NULL, unsigned long dsize = 0) EXTRA_SECTION_00;

    /**
     * Converts the given UTF-8 string in WCHAR_T. The real implementation
     * is platform specific and should go in <platform>adapter.cpp.
     * It is supposed that the destination buffer is either big enough to
     * contain the converted data or NULL; in the latter case the needed
     * space is allocated using the new C++ operator and must be discarded
     * by the caller with the delete operator.
     * This function return the pointer to the destination buffer (d or the new
     * allocated one) in case of success or NULL in case the conversion was not
     * possible.
     *
     * @param s source string
     * @param d destination buffer or NULL
     * @param dsize size of the destination buffer (ignored if d is null)
     *
     */
    wchar_t* utf82wc(const char* s, wchar_t* d = NULL, unsigned long dsize = 0) EXTRA_SECTION_00;

    /**
     * Converts the given UTF-8 string, with the given lenght, in WCHAR_T.
     * This function just make the source string NULL-terminated, then calls
     * the system-specific utf82wc.
     *
     * @param s source buffer
     * @param s source buffer lenght
     * @param d destination buffer or NULL
     * @param dsize size of the destination buffer (ignored if d is null)
     *
     */
    wchar_t* charBuf2wc(const char* s, unsigned long ssize,
                     wchar_t* d = 0, unsigned long dsize = 0) EXTRA_SECTION_00;

    /*
    * compare two wchar_t array ignoring the case of the char
    */
    bool wcscmpIgnoreCase(const wchar_t* p, const wchar_t* q);

    /*
    * Converts a integer into a wchar_t*
    */
    wchar_t* itow(int i);

    /*
    * Converts a integer into a wchar_t*
    */
    wchar_t* ltow(long i);

    /*
    * Method to create the cred data given the username, password and nonce
    * It uses the calculateMD5 to calculate the MD5 using the alghoritm.
    */
    wchar_t* MD5CredentialData(wchar_t* userName, wchar_t* password, wchar_t* nonce);

    /*
    * Calculates the digest given the token and its lenght
    */
    char* calculateMD5(void* token, int len, char* wdigest);

    /*
     * Return a filename composed by the system temp dir and the name given
     * in input. If the file exists, try to add a digit 0-9.
     * If this fails too, return NULL (there's must be something wrong in
     * the calling program)
     *
     * @param name - a file name, without path
     * @return - a full pathname, allocated with new[], or NULL on error
     */
    wchar_t *mkTempFileName(const wchar_t *name);

    /*
     * Write len bytes from buffer to the file 'filename'.
     *
     * @param name - the file name
     * @param buffer - pointer to the buffer to write
     * @param len - the number of bytes to write
     * @param binary - if true the file will be opened in binary mode
     *
     * @return - true if file is successfully saved
     */
    bool saveFile(const char *filename, const char *buffer, size_t len,
                  bool binary = false );

    /*
     * Get the size of the file, in bytes
     *
     * @param f - the file to evaluate
     * @return - the length of the file
     */
    size_t fgetsize(FILE *f); 

    /*
     * Read the content
     *
     * @param name - the file name
     * @param buffer (out) - new allocated buffer with the file content
     * @param len - length of the read content
     * @param binary - if true the file has to be opened in binary mode
     *
     * @return - true if file is succesfully read
     */
    bool readFile(const char* path, char **message, size_t *len, bool binary = false );
	
	long int getLenEncoding(const wchar_t* s, wchar_t* encoding);
	char *toMultibyte(const wchar_t *wc, const wchar_t *encoding = TEXT("UTF-8") );
	wchar_t *toWideChar(const char *mb, const wchar_t *encoding = TEXT("UTF-8") );

#endif
