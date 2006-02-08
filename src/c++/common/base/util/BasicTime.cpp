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
int BasicTime::parseRfc822(const wchar_t *date)
{
	const wchar_t *days[] = {
        TEXT("Sun"), TEXT("Mon"), TEXT("Tue"), TEXT("Wed"),
        TEXT("Thu"), TEXT("Fri"), TEXT("Sat")
    };
	const wchar_t *months[] = {
        TEXT("Jan"), TEXT("Feb"), TEXT("Mar"), TEXT("Apr"),
        TEXT("May"), TEXT("Jun"), TEXT("Jul"), TEXT("Aug"),
        TEXT("Sep"), TEXT("Oct"), TEXT("Nov"), TEXT("Dec")
    };
	wchar_t dayOfWeek[6] = TEXT("---,");
	wchar_t mon[4] = TEXT("---");
	wchar_t time[10] = TEXT("00:00:00");
	wchar_t timeZone[20] = TEXT("GMT");

    int ret;
    
    // Wed Feb 01 14:40:45 Europe/Amsterdam 2006
	// do we have day of week?
    wchar_t *pdate = wcsstr( date, TEXT(",") );
	if ( pdate == 0 ) {
		ret=swscanf(date, TEXT("%d %ls %d %ls %ls"),
            &day, month, &year, time, timeZone);
    }
	else {
		ret=swscanf(date, TEXT("%ls %d %ls %d %ls %ls"),
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
		if ( wcscmp(months[i], mon) == 0 ) {
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
	if (wcslen(time) > 6 && time[5] == ':')
		swscanf(time, TEXT("%d:%d:%d"), &hour, &min, &sec);
	else
		swscanf(time, TEXT("%d:%d"), &hour, &min);

	// Timezone ---------------------------------
    if ( wcscmp(timeZone, TEXT("GMT")) ) {
		// is this explicit time?
		if ( timeZone[0] == '+' || timeZone[0]== '-' ) {
			wchar_t wcH[3] = TEXT("00");
			wchar_t wcM[3] = TEXT("00");

			// get hour
			if ( wcslen(timeZone) > 3) {
				wcH[0] = timeZone[1];
				wcH[1] = timeZone[2];
				wcH[2] = '\0';
			}
			// get min
			if ( wcslen(timeZone) >= 5)	{
				wcM[0] = timeZone[3];
				wcM[1] = timeZone[4];
				wcM[2] = '\0';
			}
			tzHour = _wtoi(wcH);
			tzMin = _wtoi(wcM);
		}
		// otherwise it could be one string with the time
		else if ( wcscmp(timeZone, TEXT("EDT")) )
			tzHour = 4;
		else if ( wcscmp(timeZone, TEXT("EST"))
                  ||  wcscmp(timeZone, TEXT("CDT")) )
			tzHour = 5;
		else if ( wcscmp(timeZone, TEXT("CST"))
                  ||  wcscmp(timeZone, TEXT("MDT")) )
			tzHour = 6;
		else if ( wcscmp(timeZone, TEXT("MST"))
                  ||  wcscmp(timeZone, TEXT("PDT")) )
			tzHour = 7;
		else if ( wcscmp(timeZone, TEXT("PST")) )
			tzHour = 8;
	}

	// clean up
	return 0;
}

// Date: Fri, 01 Aug 2003 14:04:55 +0800
wchar_t *BasicTime::formatRfc822() const {
	const wchar_t *days[] = {
        TEXT("Sun"), TEXT("Mon"), TEXT("Tue"), TEXT("Wed"),
        TEXT("Thu"), TEXT("Fri"), TEXT("Sat"), TEXT("Sun")
    };
	const wchar_t *months[] = {
        TEXT("Jan"), TEXT("Feb"), TEXT("Mar"), TEXT("Apr"),
        TEXT("May"), TEXT("Jun"), TEXT("Jul"), TEXT("Aug"),
        TEXT("Sep"), TEXT("Oct"), TEXT("Nov"), TEXT("Dec")
    };
    wchar_t *ret = new wchar_t[30];

    wsprintf(ret, TEXT("%ls, %d %ls %d %02d:%02d:%02d %+03d%02d"),
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

