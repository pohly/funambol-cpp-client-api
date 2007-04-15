#! /usr/bin/perl -w
#
# Copyright (C) 2006 Funambol
# Author: Patrick Ohly
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

use strict;
use encoding 'utf8';
use Algorithm::Diff;

# ignore differences caused by specific servers?
my $server = $ENV{CLIENT_TEST_SERVER} || "funambol";
my $scheduleworld = $server =~ /scheduleworld/;
my $synthesis = $server =~ /synthesis/;
my $egroupware = $server =~ /egroupware/;
my $funambol = $server =~ /funambol/;

sub Usage {
  print "$0 <vcards.vcf\n";
  print "   normalizes one file (stdin or single argument), prints to stdout\n";
  print "$0 vcards1.vcf vcards2.vcf\n";
  print "   compares the two files\n";
  print "Also works for iCalendar files.\n";
}

# parameters: file handle with input, width to use for reformatted lines
# returns list of lines without line breaks
sub Normalize {
  my $in = shift;
  my $width = shift;

  $_ = join( "", <$in> );
  s/\r//g;

  my @items = ();

  foreach $_ ( split( /(?:(?<=\nEND:VCARD)|(?<=\nEND:VCALENDAR))\n*/ ) ) {
    # undo line continuation
    s/\n\s//gs;
    # ignore charset specifications, assume UTF-8
    s/;CHARSET="?UTF-8"?//g;

    # UID may differ, but only in vCards and journal entries:
    # in calendar events the UID needs to be preserved to handle
    # meeting invitations/replies correctly
    s/((VCARD|VJOURNAL).*)^UID:[^\n]*\n/$1/msg;

    # expand <foo> shortcuts to TYPE=<foo>
    while (s/^(ADR|EMAIL|TEL)([^:\n]*);(HOME|OTHER|WORK|PARCEL|INTERNET|CAR|VOICE|CELL|PAGER)/$1;TYPE=$3/mg) {}

    # the distinction between an empty and a missing property
    # is vague and handled differently, so ignore empty properties
    s/^[^:\n]*:;*\n//mg;

    # use separate TYPE= fields
    while( s/^(\w*)([^:\n]*);TYPE=(\w*),(\w*)/$1$2;TYPE=$3;TYPE=$4/mg ) {}

    # replace parameters with a sorted parameter list
    s!^([^;:\n]*);(.*?):!$1 . ";" . join(';',sort(split(/;/, $2))) . ":"!meg;

    # Map non-standard ADR;TYPE=OTHER to PARCEL, just like SyncEvolution does
    s/^ADR;TYPE=OTHER/ADR;TYPE=PARCEL/mg;
    # Ignore remaining "other" email, address and telephone type - this is
    # an Evolution specific extension which might not be preserved.
    s/^(ADR|EMAIL|TEL)([^:\n]*);TYPE=OTHER/$1$2/mg;
    # TYPE=PREF on the other hand is not used by Evolution, but
    # might be sent back.
    s/^(ADR|EMAIL)([^:\n]*);TYPE=PREF/$1$2/mg;
    # Evolution does not need TYPE=INTERNET for email
    s/^(EMAIL)([^:\n]*);TYPE=INTERNET/$1$2/mg;
    # ignore TYPE=PREF in address, does not matter in Evolution
    s/^((ADR|LABEL)[^:\n]*);TYPE=PREF/$1/mg;
    # ignore extra separators in multi-value fields
    s/^((ORG|N|(ADR[^:\n]*?)):.*?);*$/$1/mg;
    # the type of certain fields is ignore by Evolution
    s/^X-(AIM|GROUPWISE|ICQ|YAHOO);TYPE=HOME/X-$1/gm;
    # Evolution ignores an additional pager type
    s/^TEL;TYPE=PAGER;TYPE=WORK/TEL;TYPE=PAGER/gm;
    # PAGER property is sent by Evolution, but otherwise ignored
    s/^LABEL[;:].*\n//mg;
    # TYPE=VOICE is the default in Evolution and may or may not appear in the vcard;
    # this simplification is a bit too agressive and hides the problematic
    # TYPE=PREF,VOICE combination which Evolution does not handle :-/
    s/^TEL([^:\n]*);TYPE=VOICE,([^:\n]*):/TEL$1;TYPE=$2:/mg;
    s/^TEL([^:\n]*);TYPE=([^;:\n]*),VOICE([^:\n]*):/TEL$1;TYPE=$2$3:/mg;
    s/^TEL([^:\n]*);TYPE=VOICE([^:\n]*):/TEL$1$2:/mg;
    # don't care about the TYPE property of PHOTOs
    s/^PHOTO;(.*)TYPE=[A-Z]*/PHOTO;$1/mg;
    # encoding is not case sensitive, skip white space in the middle of binary data
    if (s/^PHOTO;.*?ENCODING=(b|B|BASE64).*?:\s*/PHOTO;ENCODING=B: /mgi) {
      while (s/^PHOTO(.*?): (\S+)[\t ]+(\S+)/PHOTO$1: $2$3/mg) {}
    }
    # ignore extra day factor in front of weekday
    s/^RRULE:(.*)BYDAY=\+?1(\D)/RRULE:$1BYDAY=$2/mg;
    # remove default VALUE=DATE-TIME
    s/^(DTSTART|DTEND)([^:\n]*);VALUE=DATE-TIME/$1$2/mg;

    # remove fields which may differ
    s/^(PRODID|CREATED|DTSTAMP|LAST-MODIFIED|REV):.*\r?\n?//gm;
    # remove optional fields
    s/^(METHOD|X-WSS-COMPONENT|X-WSS-LUID):.*\r?\n?//gm;

    if ($scheduleworld || $egroupware || $synthesis) {
      # does not preserve X-EVOLUTION-UI-SLOT=
      s/^(\w+)([^:\n]*);X-EVOLUTION-UI-SLOT=\d+/$1$2/mg;
    }

    if ($scheduleworld) {
      # cannot distinguish EMAIL types
      s/^EMAIL;TYPE=\w*/EMAIL/mg;
      # replaces certain TZIDs with more up-to-date ones
      s;TZID(=|:)/(scheduleworld.com|softwarestudio.org)/Olson_\d+_\d+/;TZID$1/foo.com/Olson_20000101_1/;mg;
    }

    if ($scheduleworld || $synthesis) {
      # only preserves ORG "Company", but loses "Department" and "Office"
      s/^ORG:([^;:\n]+)(;[^\n]*)/ORG:$1/mg;
    }

    if ($funambol) {
      # only preserves ORG "Company";"Department", but loses "Office"
      s/^ORG:([^;:\n]+)(;[^;:\n]*)(;[^\n]*)/ORG:$1$2/mg;
    }

    if ($synthesis) {
      # does not preserve certain properties
      s/^(FN|BDAY|X-MOZILLA-HTML|X-EVOLUTION-FILE-AS|X-AIM|NICKNAME|PHOTO|CALURI)(;[^:;\n]*)*:.*\r?\n?//gm;
      # default ADR is HOME
      s/^ADR;TYPE=HOME/ADR/gm;
      # only some parts of N are preserved
      s/^N\:(.*)/@_ = split(\/(?<!\\);\/, $1); "N:$_[0];" . ($_[1] || "") . ";;" . ($_[3] || "")/gme;
      # this vcard contains too many ADR and PHONE entries - ignore it
      if (/This is a test case which uses almost all Evolution fields/) {
        next;
      }
    }

    if ($egroupware) {
      # CLASS:PUBLIC is added if none exists (as in our test cases),
      # several properties not preserved
      s/^(BDAY|CATEGORIES|FBURL|PHOTO|FN|X-[A-Z-]*|CALURI|CLASS|NICKNAME|UID|TRANSP|PRIORITY|SEQUENCE)(;[^:;\n]*)*:.*\r?\n?//gm;
      # org gets truncated
      s/^ORG:([^;:\n]*);.*/ORG:$1/gm;
    }

    if ($funambol) {
      # several properties are not preserved
      s/^(FN|X-MOZILLA-HTML|PHOTO)(;[^:;\n]*)*:.*\r?\n?//gm;

      # quoted-printable line breaks are =0D=0A, not just single =0A
      s/(?<!=0D)=0A/=0D=0A/g;
    }

    if ($funambol || $egroupware) {
      # NOTE may be truncated due to length resistrictions
      s/^(NOTE(;[^:;\n]*)*:.{0,160}).*(\r?\n?)/$1$3/gm;
    }

    my @formatted = ();

    # Modify lines to cover not more than
    # $width characters by folding lines (as done for the N or SUMMARY above),
    # but also indent each inner BEGIN/END block by 2 spaces
    # and finally sort the lines.
    # We need to keep a stack of open blocks in @formatted:
    # - BEGIN creates another open block
    # - END closes it, sorts it, and adds as single string to the parent block
    push @formatted, [];
    foreach $_ (split /\n/, $_) {
      if (/^BEGIN:/) {
        # start a new block
        push @formatted, [];
      }

      my $spaces = "  " x ($#formatted - 1);
      my $thiswidth = $width -1 - length($spaces);
      $thiswidth = 1 if $thiswidth <= 0;
      s/(.{$thiswidth})(?!$)/$1\n /g;
      s/^(.*)$/$spaces$1/mg;
      push @{$formatted[$#formatted]}, $_;

      if (/^\s*END:/) {
        my $block = pop @formatted;
        my $begin = shift @{$block};
        my $end = pop @{$block};

        # Keep begin/end as first/last line,
        # inbetween sort, but so that N or SUMMARY are
        # at the top. This ensures that the order of items
        # is the same, even if individual properties differ.
        # Also put indented blocks at the end, not the top.
        sub numspaces {
          my $str = shift;
          $str =~ /^(\s*)/;
          return length($1);
        }
        $_ = join("\n",
                  $begin,
                  sort( { $a =~ /^\s*(N|SUMMARY):/ ? -1 :
                          $b =~ /^\s*(N|SUMMARY):/ ? 1 :
                          ($a =~ /^\s/ && $b =~ /^\S/) ? 1 :
                          numspaces($a) == numspaces($b) ? $a cmp $b :
                          numspaces($a) - numspaces($b) }
                        @{$block} ),
                  $end);
        push @{$formatted[$#formatted]}, $_;
      }
    }

    push @items, ${$formatted[0]}[0];
  }

  return split( /\n/, join( "\n\n", sort @items ));
}

# number of columns available for output:
# try tput without printing the shells error if not found,
# default to 80
my $columns = `which tput >/dev/null 2>/dev/null && tput 2>/dev/null && tput cols`;
if ($? || !$columns) {
  $columns = 80;
}

if($#ARGV > 1) {
  # error
  Usage();
  exit 1;
} elsif($#ARGV == 1) {
  # comparison

  my ($file1, $file2) = ($ARGV[0], $ARGV[1]);

  open(IN1, "<:utf8", $file1) || die "$file1: $!";
  open(IN2, "<:utf8", $file2) || die "$file2: $!";
  my $singlewidth = int(($columns - 3) / 2);
  $columns = $singlewidth * 2 + 3;
  my @normal1 = Normalize(*IN1{IO}, $singlewidth);
  my @normal2 = Normalize(*IN2{IO}, $singlewidth);
  close(IN1);
  close(IN2);

  # Produce output where each line is marked as old (aka remove) with o,
  # as new (aka added) with n, and as unchanged with u at the beginning.
  # This allows simpler processing below.
  my $res = 0;
  if (0) {
    # $_ = `diff "--old-line-format=o %L" "--new-line-format=n %L" "--unchanged-line-format=u %L" "$normal1" "$normal2"`;
    # $res = $?;
  } else {
    # convert into same format as diff above - this allows reusing the
    # existing output formatting code
    my $diffs_ref = Algorithm::Diff::sdiff(\@normal1, \@normal2);
    @_ = ();
    my $hunk;
    foreach $hunk ( @{$diffs_ref} ) {
      my ($type, $left, $right) = @{$hunk};
      if ($type eq "-") {
        push @_, "o $left";
        $res = 1;
      } elsif ($type eq "+") {
        push @_, "n $right";
        $res = 1;
      } elsif ($type eq "c") {
        push @_, "o $left";
        push @_, "n $right";
        $res = 1;
      } else {
        push @_, "u $left";
      }
    }

    $_ = join("\n", @_);
  }

  if ($res) {
    printf "%*s | %s\n", $singlewidth, "before sync", "after sync";
    printf "%*s <\n", $singlewidth, "removed during sync";
    printf "%*s > %s\n", $singlewidth, "", "added during sync";
    print "-" x $columns, "\n";

    # fix confusing output like:
    # BEGIN:VCARD                             BEGIN:VCARD
    #                                      >  N:new;entry
    #                                      >  FN:new
    #                                      >  END:VCARD
    #                                      >
    #                                      >  BEGIN:VCARD
    # and replace it with:
    #                                      >  BEGIN:VCARD
    #                                      >  N:new;entry
    #                                      >  FN:new
    #                                      >  END:VCARD
    #
    # BEGIN:VCARD                             BEGIN:VCARD
    #
    # With the o/n/u markup this presents itself as:
    # u BEGIN:VCARD
    # n N:new;entry
    # n FN:new
    # n END:VCARD
    # n
    # n BEGIN:VCARD
    #
    # The alternative case is also possible:
    # o END:VCARD
    # o 
    # o BEGIN:VCARD
    # o N:old;entry
    # u END:VCARD

    # case one above
    while( s/^u BEGIN:(VCARD|VCALENDAR)\n((?:^n .*\n)+?)^n BEGIN:/n BEGIN:$1\n$2u BEGIN:/m) {}
    # same for the other direction
    while( s/^u BEGIN:(VCARD|VCALENDAR)\n((?:^o .*\n)+?)^o BEGIN:/o BEGIN:$1\n$2u BEGIN:/m) {}

    # case two
    while( s/^o END:(VCARD|VCALENDAR)\n((?:^o .*\n)+?)^u END:/u END:$1\n$2o END:/m) {}
    while( s/^n END:(VCARD|VCALENDAR)\n((?:^n .*\n)+?)^u END:/u END:$1\n$2n END:/m) {}

    # split at end of each record
    my $spaces = " " x $singlewidth;
    foreach $_ (split /(?:(?<=. END:VCARD\n)|(?<=. END:VCALENDAR\n))(?:^. \n)*/m, $_) {
      # ignore unchanged records
      if (!length($_) || /^((u [^\n]*\n)*(u [^\n]*?))$/s) {
        next;
      }

      # make all lines equally long in terms of printable characters
      s/^(.*)$/$1 . (" " x ($singlewidth + 2 - length($1)))/gme;

      # convert into side-by-side output
      my @buffer = ();
      foreach $_ (split /\n/, $_) {
        if (/^u (.*)/) {
          print join(" <\n", @buffer), " <\n" if $#buffer >= 0;
          @buffer = ();
          print $1, "   ", $1, "\n";
        } elsif (/^o (.*)/) {
          # preserve in buffer for potential merging with "n "
          push @buffer, $1;
        } else {
          /^n (.*)/;
          # have line to be merged with?
          if ($#buffer >= 0) {
            print shift @buffer, " | ", $1, "\n";
          } else {
            print join(" <\n", @buffer), " <\n" if $#buffer >= 0;
            print $spaces, " > ", $1, "\n";
          }
        }
      }
      print join(" <\n", @buffer), " <\n" if $#buffer >= 0;
      @buffer = ();

      print "-" x $columns, "\n";
    }
  }

  # unlink($normal1);
  # unlink($normal2);
  exit($res ? 1 : 0);
} else {
  # normalize
  my $in;
  if( $#ARGV >= 0 ) {
    open(IN, "<$ARGV[0]") || die "$ARGV[0]: $!";
    $in = *IN{IO};
  } else {
    $in = *STDIN{IO};
  }

  print STDOUT join("\n", Normalize($in, $columns)), "\n";
}
