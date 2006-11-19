/*
 * Copyright (C) 2006 Funambol
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

#include <base/test.h>

#include "base/util/BasicTime.h"

BasicTime::BasicTime() {
    year = 1970;
    month = 1;
    day = 1;
    weekday = 0;
    hour = 0;
    min = 0;
    sec = 0;
    tzHour = 0;
    tzMin = 0;
}

int BasicTime::set(int yy, int mon, int dd, int wd,
                   int hh, int mm, int ss, int tzh, int tzm)
{
    //  TODO
    return 0;
}

/**
 * Parse the date in RF 822 format
 *
 * Some examples:
 * Date: Fri, 01 Aug 2003 14:04:55 +0800
 * Date: Wed, 30 Jul 2003 13:24:21 -0700
 * Date: 20 Jun 2003 15:42:12 -0500
 *
 * RFC822 date time
 *
 *   date-time   =  [ day "," ] date time        ; dd mm yy
 *                                              ;  hh:mm:ss zzz
 *
 *   day         =  "Mon"  / "Tue" /  "Wed"  / "Thu"
 *               /  "Fri"  / "Sat" /  "Sun"
 *
 *   date        =  1*2DIGIT month 2DIGIT        ; day month year
 *                                               ;  e.g. 20 Jun 82
 *
 *   month       =  "Jan"  /  "Feb" /  "Mar"  /  "Apr"
 *               /  "May"  /  "Jun" /  "Jul"  /  "Aug"
 *               /  "Sep"  /  "Oct" /  "Nov"  /  "Dec"
 *
 *   time        =  hour zone                    ; ANSI and Military
 *
 *   hour        =  2DIGIT ":" 2DIGIT [":" 2DIGIT]
 *                                               ; 00:00:00 - 23:59:59
 *
 *   zone        =  "UT"  / "GMT"                ; Universal Time
 *                                               ; North American : UT
 *               /  "EST" / "EDT"                ;  Eastern:  - 5/ - 4
 *               /  "CST" / "CDT"                ;  Central:  - 6/ - 5
 *               /  "MST" / "MDT"                ;  Mountain: - 7/ - 6
 *               /  "PST" / "PDT"                ;  Pacific:  - 8/ - 7
 *               /  1ALPHA                       ; Military: Z = UT;
 *                                               ;  A:-1; (J not used)
 *                                               ;  M:-12; N:+1; Y:+12
 *               / ( ("+" / "-") 4DIGIT )        ; Local differential
 *                                               ;  hours+min. (HHMM)
**/
int BasicTime::parseRfc822(const char *date)
{
	const char *days[] = {
        T("Sun"), T("Mon"), T("Tue"), T("Wed"),
        T("Thu"), T("Fri"), T("Sat")
    };
	const char *months[] = {
        T("Jan"), T("Feb"), T("Mar"), T("Apr"),
        T("May"), T("Jun"), T("Jul"), T("Aug"),
        T("Sep"), T("Oct"), T("Nov"), T("Dec")
    };
	char dayOfWeek[6] = T("---,");
	char mon[4] = T("---");
	char time[10] = T("00:00:00");
	char timeZone[20] = T("GMT");

    int ret;
    
    // Wed Feb 01 14:40:45 Europe/Amsterdam 2006
	// do we have day of week?
    const char *pdate = strstr( date, T(",") );
	if ( pdate == 0 ) {
		ret=sscanf(date, T("%d %s %d %s %s"),
            &day, mon, &year, time, timeZone);
    }
	else {
		ret=sscanf(date, T("%s %d %s %d %s %s"),
            dayOfWeek, &day, mon, &year, time, timeZone);
    }
    // Trap parsing error
    if(ret == EOF || ret == 0){
        return -1;
    }
    if (year > 3000 || day < 0 || day > 31){
        *this = BasicTime();
        return -1;
    }

    // Get month
    int i;
	for (i = 0; i < 12; i++) {
		if ( strcmp(months[i], mon) == 0 ) {
            month = i+1;
			break;
        }
	}
    // Trap parsing error
    if (i==13)
        return -1;

	// Year ---------------------------------
	if (year < 100) year += 1900;

	// hh:mm:ss -------------------------
	// do we have sec?
	if (strlen(time) > 6 && time[5] == ':')
		sscanf(time, T("%d:%d:%d"), &hour, &min, &sec);
	else
		sscanf(time, T("%d:%d"), &hour, &min);

	// Timezone ---------------------------------
    if ( strcmp(timeZone, T("GMT")) ) {
		// is this explicit time?
		if ( timeZone[0] == '+' || timeZone[0]== '-' ) {
			char wcH[4] = T("+00");
			char wcM[4] = T("00");

			// get hour
			if ( strlen(timeZone) > 3) {
				wcH[0] = timeZone[0];
				wcH[1] = timeZone[1];
				wcH[2] = timeZone[2];
				wcH[3] = '\0';
			}
			// get min
			if ( strlen(timeZone) >= 5)	{
				wcM[0] = timeZone[3];
				wcM[1] = timeZone[4];
				wcM[2] = '\0';
			}
			tzHour = atoi(wcH);
			tzMin = atoi(wcM);
		}
		// otherwise it could be one string with the time
		else if ( strcmp(timeZone, T("EDT")) )
			tzHour = -4;
		else if ( strcmp(timeZone, T("EST"))
                  ||  strcmp(timeZone, T("CDT")) )
			tzHour = -5;
		else if ( strcmp(timeZone, T("CST"))
                  ||  strcmp(timeZone, T("MDT")) )
			tzHour = -6;
		else if ( strcmp(timeZone, T("MST"))
                  ||  strcmp(timeZone, T("PDT")) )
			tzHour = -7;
		else if ( strcmp(timeZone, T("PST")) )
			tzHour = -8;
	}

	// clean up
	return 0;
}

// Date: Fri, 01 Aug 2003 14:04:55 +0800
char *BasicTime::formatRfc822() const {
	const char *days[] = {
        T("Sun"), T("Mon"), T("Tue"), T("Wed"),
        T("Thu"), T("Fri"), T("Sat"), T("Sun")
    };
	const char *months[] = {
        T("Jan"), T("Feb"), T("Mar"), T("Apr"),
        T("May"), T("Jun"), T("Jul"), T("Aug"),
        T("Sep"), T("Oct"), T("Nov"), T("Dec")
    };
    char *ret = new char[60]; // FIXME: avoid sprintf and static size

    sprintf(ret, T("%s, %d %s %d %02d:%02d:%02d %+03d%02d"),
                  days[weekday], day, months[month-1], year, hour, min, sec,
                  tzHour, tzMin);

    return ret;
}

ArrayElement *BasicTime::clone() {
    return new BasicTime(*this);
};

BasicTime& BasicTime::operator=(const BasicTime& o) {
    year = o.year;
    month = o.month;
    day = o.day;
    weekday = o.weekday;
    hour = o.hour;
    min = o.min;
    sec = o.sec;
    tzHour = o.tzHour;
    tzMin = o.tzMin;
    
    return *this;
}

bool BasicTime::operator==(const BasicTime& o) const {
    return (
        year == o.year &&
        month == o.month &&
        day == o.day &&
        weekday == o.weekday &&
        hour == o.hour &&
        min == o.min &&
        sec == o.sec &&
        tzHour == o.tzHour &&
        tzMin == o.tzMin
    );
}

#ifdef ENABLE_UNIT_TESTS


class BasicTimeTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(BasicTimeTest);
    CPPUNIT_TEST(testEqual);
    CPPUNIT_TEST(testConversion);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        // millenium.set(2000, 01, 01, 6,
        //              00, 00, 00,
        //              00, 00);
        millenium.setYear(2000);
        buffer = NULL;
    }
    void tearDown() {
        if (buffer) {
            delete [] buffer;
        }
    }

protected:
    void testEqual() {
        BasicTime empty;
        CPPUNIT_ASSERT(empty != millenium);

        BasicTime copy(millenium);
        CPPUNIT_ASSERT(millenium == copy);
        copy = millenium;
        CPPUNIT_ASSERT(millenium == copy);
    }
    
    void testConversion() {
        buffer = millenium.formatRfc822();

        BasicTime copy;
        CPPUNIT_ASSERT_EQUAL(0, copy.parseRfc822(buffer));
        CPPUNIT_ASSERT(millenium == copy);
        delete [] buffer; buffer = NULL;

        CPPUNIT_ASSERT_EQUAL(-1, copy.parseRfc822("this is garbage"));

        static const char convertStr[] = "Mon, 6 Nov 2006 20:30:15 +0100";
        BasicTime convert;
        CPPUNIT_ASSERT_EQUAL(0, convert.parseRfc822(convertStr));
        buffer = convert.formatRfc822();
        CPPUNIT_ASSERT(!strcmp(buffer, convertStr));
        delete [] buffer; buffer = NULL;
    }

private:
    BasicTime millenium;
    char *buffer;
};

FUNAMBOL_TEST_SUITE_REGISTRATION(BasicTimeTest);

#endif
