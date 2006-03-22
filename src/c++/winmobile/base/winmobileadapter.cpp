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

#include <stdio.h>
#include "base/Log.h"
#include "base/util/utils.h"

struct Codepage {
    const BCHAR *name;
    int  codepage_id;
};

static Codepage encodings[] = {
    { T("_iso-2022-jp"), 50221 },
    { T("_iso-2022-jp$sio"), 50222 },
    { T("ansi_x3.4-1968"), 1252 },
    { T("ansi_x3.4-1986"), 1252 },
    { T("arabic"), 28596 },
    { T("ascii"), 1252 },
    { T("asmo-708"), 708 },
    { T("big5"), 950 },
    { T("chinese"), 936 },
    { T("cn-big5"), 950 },
    { T("cn-gb"), 936 },
    { T("codepage437"), 437 },
    { T("cp1026"), 1026 },
    { T("cp1256"), 1256 },
    { T("cp367"), 1252 },
    { T("cp437"), 437 },
    { T("cp500"), 775 },
    { T("cp819"), 1252 },
    { T("cp852"), 852 },
    { T("cp866"), 866 },
    { T("cp870"), 870 },
    { T("csascii"), 1252 },
    { T("csbig5"), 950 },
    { T("cseuckr"), 51949 },
    { T("cseucpkdfmtjapanese"), 51932 },
    { T("csgb2312"), 936 },
    { T("csgb231280"), 936 },
    { T("csiso"), 28591 },
    { T("csiso2022jp"), 50221 },
    { T("csiso2022kr"), 50225 },
    { T("csiso58gb231280"), 936 },
    { T("csisolatin2"), 28592 },
    { T("csisolatin4"), 28594 },
    { T("csisolatin5"), 28595 },
    { T("csisolatinarabic"), 28596 },
    { T("csisolatincyrillic"), 28595 },
    { T("csisolatingreek"), 28597 },
    { T("csisolatinhebrew"), 28598 },
    { T("cskoi8r"), 20866 },
    { T("csksc56011987"), 949 },
    { T("cspc8"), 437 },
    { T("csshiftjis"), 932 },
    { T("csunicode11utf7"), 65000 },
    { T("cswindows31j"), 932 },
    { T("cyrillic"), 28595 },
    { T("dos-720"), 720 },
    { T("dos-862"), 862 },
    { T("dos-874"), 874 },
    { T("ebcdic-cp-us"), 37 },
    { T("ecma-114"), 28596 },
    { T("ecma-118"), 28597 },
    { T("elot_928"), 28597 },
    { T("euc-cn"), 51936 },
    { T("euc-jp"), 51932 },
    { T("euc-kr"), 51949 },
    { T("gb2312"), 936 },
    { T("gb2312-80"), 936 },
    { T("gb231280"), 936 },
    { T("gb_2312-80"), 936 },
    { T("gbk"), 936 },
    { T("greek"), 28597 },
    { T("greek8"), 28597 },
    { T("hebrew"), 28598 },
    { T("hz-gb-2312"), 52936 },
    { T("ibm367"), 1252 },
    { T("ibm437"), 437 },
    { T("ibm737"), 737 },
    { T("ibm775"), 775 },
    { T("ibm819"), 1252 },
    { T("ibm850"), 850 },
    { T("ibm852"), 852 },
    { T("ibm857"), 857 },
    { T("ibm861"), 861 },
    { T("ibm866"), 866 },
    { T("ibm869"), 869 },
    { T("iso-2022-jp"), 50222 },
    { T("iso-2022-kr"), 50225 },
    { T("iso-8859-1"), 1252 },
    { T("iso-8859-11"), 874 },
    { T("iso-8859-15"), 28605 },
    { T("iso-8859-2"), 28592 },
    { T("iso-8859-3"), 28593 },
    { T("iso-8859-4"), 28594 },
    { T("iso-8859-5"), 28595 },
    { T("iso-8859-6"), 28596 },
    { T("iso-8859-7"), 28597 },
    { T("iso-8859-8"), 1255 },
    { T("iso-8859-8-i"), 38598 },
    { T("iso-8859-9"), 1254 },
    { T("iso-ir-100"), 1252 },
    { T("iso-ir-101"), 28592 },
    { T("iso-ir-109"), 28593 },
    { T("iso-ir-110"), 28594 },
    { T("iso-ir-126"), 28597 },
    { T("iso-ir-127"), 28596 },
    { T("iso-ir-138"), 28598 },
    { T("iso-ir-144"), 28595 },
    { T("iso-ir-148"), 1254 },
    { T("iso-ir-149"), 949 },
    { T("iso-ir-58"), 936 },
    { T("iso-ir-6"), 1252 },
    { T("iso-ir-6us"), 20127 },
    { T("iso646-us"), 1252 },
    { T("iso8859-1"), 1252 },
    { T("iso8859-2"), 28592 },
    { T("iso_646.irv:1991"), 1252 },
    { T("iso_8859-1"), 1252 },
    { T("iso_8859-15"), 28605 },
    { T("iso_8859-1:1987"), 1252 },
    { T("iso_8859-2"), 28592 },
    { T("iso_8859-2:1987"), 28592 },
    { T("iso_8859-3"), 28593 },
    { T("iso_8859-3:1988"), 28593 },
    { T("iso_8859-4"), 28594 },
    { T("iso_8859-4:1988"), 28594 },
    { T("iso_8859-5"), 28595 },
    { T("iso_8859-5:1988"), 28595 },
    { T("iso_8859-6"), 28596 },
    { T("iso_8859-6:1987"), 28596 },
    { T("iso_8859-7"), 28597 },
    { T("iso_8859-7:1987"), 28597 },
    { T("iso_8859-8"), 28598 },
    { T("iso_8859-8-i"), 1255 },
    { T("iso_8859-8:1988"), 28598 },
    { T("iso_8859-9"), 1254 },
    { T("iso_8859-9:1989"), 1254 },
    { T("johab"), 1361 },
    { T("koi"), 20866 },
    { T("koi8"), 20866 },
    { T("koi8-r"), 20866 },
    { T("koi8-ru"), 21866 },
    { T("koi8-u"), 21866 },
    { T("koi8r"), 20866 },
    { T("korean"), 949 },
    { T("ks_c_5601"), 949 },
    { T("ks_c_5601-1987"), 949 },
    { T("ks_c_5601-1989"), 949 },
    { T("ks_c_5601_1987"), 949 },
    { T("ksc5601"), 949 },
    { T("ksc_5601"), 949 },
    { T("l1"), 28591 },
    { T("l2"), 28592 },
    { T("l3"), 28593 },
    { T("l4"), 28594 },
    { T("l5"), 28599 },
    { T("l9"), 28605 },
    { T("latin1"), 1252 },
    { T("latin2"), 28592 },
    { T("latin3"), 28593 },
    { T("latin4"), 28594 },
    { T("latin5"), 1254 },
    { T("latin9"), 28605 },
    { T("logical"), 38598 },
    { T("macintosh"), 10000 },
    { T("ms_kanji"), 932 },
    { T("shift-jis"), 932 },
    { T("shift_jis"), 932 },
    { T("tis-620"), 874 },
    { T("unicode"), 1200 },
    { T("unicode-1-1-utf-7"), 65000 },
    { T("unicode-1-1-utf-8"), 65001 },
    { T("unicode-2-0-utf-8"), 65001 },
    { T("unicodefffe"), 1201 },
    { T("us"), 1252 },
    { T("us-ascii"), 1252 },
    { T("utf-16"), 1200 },
    { T("utf-7"), 65000 },
    { T("utf-8"), 65001 },
    { T("visual"), 1255 },
    { T("windows-1250"), 1250 },
    { T("windows-1251"), 1251 },
    { T("windows-1252"), 1252 },
    { T("windows-1253"), 1253 },
    { T("windows-1254"), 1254 },
    { T("windows-1255"), 1255 },
    { T("windows-1256"), 1256 },
    { T("windows-1257"), 1257 },
    { T("windows-1258"), 1258 },
    { T("windows-874"), 874 },
    { T("x-ansi"), 1252 },
    { T("x-chinese-cns"), 20000 },
    { T("x-chinese-eten"), 20002 },
    { T("x-cp1250"), 1250 },
    { T("x-cp1251"), 1251 },
    { T("x-ebcdic-arabic"), 20420 },
    { T("x-ebcdic-cp-us-euro"), 1140 },
    { T("x-ebcdic-cyrillicrussian"), 20880 },
    { T("x-ebcdic-cyrillicserbianbulgarian"), 21025 },
    { T("x-ebcdic-denmarknorway"), 20277 },
    { T("x-ebcdic-denmarknorway-euro"), 1142 },
    { T("x-ebcdic-finlandsweden"), 20278 },
    { T("x-ebcdic-finlandsweden-euro"), 1143 },
    { T("x-ebcdic-france"), 1143 },
    { T("x-ebcdic-france-euro"), 1147 },
    { T("x-ebcdic-germany"), 20273 },
    { T("x-ebcdic-germany-euro"), 1141 },
    { T("x-ebcdic-greek"), 20423 },
    { T("x-ebcdic-greekmodern"), 875 },
    { T("x-ebcdic-hebrew"), 20424 },
    { T("x-ebcdic-icelandic"), 20871 },
    { T("x-ebcdic-icelandic-euro"), 1149 },
    { T("x-ebcdic-international-euro"), 1148 },
    { T("x-ebcdic-italy"), 20280 },
    { T("x-ebcdic-italy-euro"), 1144 },
    { T("x-ebcdic-japaneseandjapaneselatin"), 50939 },
    { T("x-ebcdic-japaneseandkana"), 50930 },
    { T("x-ebcdic-japaneseanduscanada"), 50931 },
    { T("x-ebcdic-japanesekatakana"), 20290 },
    { T("x-ebcdic-koreanandkoreanextended"), 50933 },
    { T("x-ebcdic-koreanextended"), 20833 },
    { T("x-ebcdic-simplifiedchinese"), 50935 },
    { T("x-ebcdic-spain"), 20284 },
    { T("x-ebcdic-spain-euro"), 1145 },
    { T("x-ebcdic-thai"), 20838 },
    { T("x-ebcdic-traditionalchinese"), 50937 },
    { T("x-ebcdic-turkish"), 20905 },
    { T("x-ebcdic-uk"), 20285 },
    { T("x-ebcdic-uk-euro"), 1146 },
    { T("x-euc"), 51932 },
    { T("x-euc-cn"), 51936 },
    { T("x-euc-jp"), 51932 },
    { T("x-europa"), 29001 },
    { T("x-ia5"), 20105 },
    { T("x-ia5-german"), 20106 },
    { T("x-ia5-norwegian"), 20108 },
    { T("x-ia5-swedish"), 20107 },
    { T("x-iscii-as"), 57006 },
    { T("x-iscii-be"), 57003 },
    { T("x-iscii-de"), 57002 },
    { T("x-iscii-gu"), 57010 },
    { T("x-iscii-ka"), 57008 },
    { T("x-iscii-ma"), 57009 },
    { T("x-iscii-or"), 57007 },
    { T("x-iscii-pa"), 57011 },
    { T("x-iscii-ta"), 57004 },
    { T("x-iscii-te"), 57005 },
    { T("x-mac-arabic"), 10004 },
    { T("x-mac-ce"), 10029 },
    { T("x-mac-chinesesimp"), 10008 },
    { T("x-mac-chinesetrad"), 10002 },
    { T("x-mac-cyrillic"), 10007 },
    { T("x-mac-greek"), 10006 },
    { T("x-mac-hebrew"), 10005 },
    { T("x-mac-icelandic"), 10079 },
    { T("x-mac-japanese"), 10001 },
    { T("x-mac-korean"), 10003 },
    { T("x-mac-turkish"), 10081 },
    { T("x-ms-cp932"), 932 },
    { T("x-sjis"), 932 },
    { T("x-unicode-2-0-utf-7"), 65000 },
    { T("x-unicode-2-0-utf-8"), 65001 },
    { T("x-x-big5"), 950 },
    { NULL, 0 }
};


long utf8len(const BCHAR* s) {
	 return s ? bstrlen(s) : 0;
}

char* wc2utf8(const BCHAR* s, char* d, unsigned long dsize) {
    
    //
    // First of all, if s is NULL, just return NULL.
    // Then, if d is NULL, let's allocate the required memory to contain the
    // utf8 string.
    //
    if (s == NULL) {
        return NULL;
    }
    
    if (d == NULL) {
        dsize = utf8len(s);
        d = new char[dsize+1];
    }        
    
	bstrcpy( d, s );
                                    
    return d;
}

BCHAR* utf82wc(const char* s, BCHAR* d, unsigned long dsize) {
              
    //
    // First of all, if s is NULL, just return NULL.
    // Then, if d is NULL, let's allocate the required memory to contain the
    // BCHAR string.
    //
    if (s == NULL) {
        return NULL;
    }
   
    if (d == NULL) {
        // get the right lenght with a NULL dest
        dsize = strlen (s);
        d = new BCHAR[dsize+1];
    }

    bstrcpy(d, s);

    return d;    
}

/*
 * Return a filename composed by the system temp dir and the name given 
 * in input. If the file exists, try to add a digit 0-9.
 * If this fails too, return NULL (there's must be something wrong in
 * the calling program)
 *
 * @param name - a file name, without path
 * @return - a full pathname, allocated with new[], or NULL on error
 */
BCHAR *mkTempFileName(const BCHAR *name)
{
    wchar_t tmpPath[64];
    wchar_t tmpFileName[MAX_PATH];  // System constants for the path

    GetTempPath(64, tmpPath);
    int ret = GetTempFileName(tmpPath, L"fun", 0, tmpFileName);
    
    if (ret == 0) { // function GetTempFileName fails 
        LOG.error("mkTempFileName: error in GetTempFileName");
        return 0;
    }
    return toMultibyte(tmpFileName);
}


size_t fgetsize(FILE *f)
{
    size_t size;

    fseek(f, 0, SEEK_END);
    size=ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

bool readFile(const char* path, char **message, size_t *len, bool binary)
{
    FILE *f;
    size_t msglen=0;
    char *msg=0;
    const char *mode = binary ? "rb" : "r" ;

    f = fopen(path, mode);
    if ( !f ) {
        return false;
    }
    msglen = fgetsize(f);
    msg = new char[msglen+1];
    memset(msg, 0, msglen);

    *len=fread(msg, sizeof(char), msglen, f);
    if(ferror(f)){
        delete [] msg;
        return false;
    }
    fclose(f);

    // Set return parameters
    *message= msg ;
    *len=msglen;

    return true;
}


bool saveFile(const char *filename,
              const char *buffer,
              size_t len, bool binary)
{
    const char *mode = binary ? "wb" : "w" ;
    FILE *f = fopen(filename, mode);

    if(!f)
        return false;

    if (fwrite(buffer, sizeof(char), len, f) != len) {
        fclose(f);
        return false;
    }
    fclose(f);

    return true;
}

static int findCodePage(const BCHAR *encoding)
{
    if (encoding){
  	    for(int i=0; encodings[i].name; i++) {
	        if(bstricmp(encodings[i].name, encoding) == 0) {
                // Found
                return encodings[i].codepage_id;
	        }
        }
        // Not found
        bsprintf(logmsg, T("Invalid encoding: %s"), encoding);
        LOG.error(logmsg);
    }
    // Default encoding
    return CP_UTF8;
}

static size_t getLenEncoding(const wchar_t* s, int codepage)
{
    if (!s)
        return 0;

    int len = wcslen(s);

	if (!len)
        return 0;

	long k = WideCharToMultiByte (codepage, 0, s, len, 0, 0, 0, 0);

    return (k != 0) ? (long)k : -1;
}

size_t getLenEncoding(const WCHAR* s, const BCHAR* encoding)
{
    return getLenEncoding( s, findCodePage(encoding) );
}

char* toMultibyte(const WCHAR *wc, const BCHAR *encoding)
{
	if (!wc) {
        return NULL;
    }

    char *ret;
    size_t wlen = wcslen(wc);

    if (!wlen) {
        ret = new char[1];
        ret[0] = 0;
        return ret;
    }

    int codepage = findCodePage(encoding);
    size_t blen = getLenEncoding(wc, codepage);

    if(blen <= 0)
        return 0;

    ret = new char[blen+1];

	blen = WideCharToMultiByte(codepage, 0, wc, wlen, ret, blen, 0, 0);	
	ret[blen] = 0;

    return ret;
}

wchar_t* toWideChar(const char *mb, const BCHAR *encoding) {

	if (mb == NULL) {
        return NULL;
    }

    unsigned long dsize = strlen(mb); 
    wchar_t *ret = new wchar_t[dsize+1];
    memset(ret, 0, (dsize + 1)*sizeof(wchar_t));
    
    if (!dsize)
        return ret;

    int codepage = findCodePage(encoding);
	unsigned long k = 0;
    
	k = MultiByteToWideChar(codepage, 0, mb, strlen(mb), ret, dsize + 1);
    if(k == 0) {
        delete [] ret; ret = NULL;
    }
	
    return ret;
}

