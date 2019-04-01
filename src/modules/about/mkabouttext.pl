#!/usr/bin/perl

#============================================================================
#
#   File : mkabouttext.pl
#   Creation date : Sat 03 May 2000 01:40:44 by Szymon Stefanek
#
#   This file is part of the KVIrc IRC Client distribution
#   Copyright (C) 2000 Szymon Stefanek <pragma at kvirc dot net>
#   Copyright (C) 2008 Elvio Basello <hell at hellvis69 dot netsons dot org>
#
#   This program is FREE software. You can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your option) any later version.
#
#   This program is distributed in the HOPE that it will be USEFUL,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#   See the GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, write to the Free Software Foundation,
#   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
#============================================================================

open(TEMPLATE,"ENTRIES") or die "Can't open the ENTRIES file";

$cnt = -1;

while(<TEMPLATE>)
{
	$_ =~ s/\n//g;
	$_ =~ s/</&lt;/g;
	$_ =~ s/>/&gt;/g;

	if(/^SECTION:[ 	]*/)
	{
		$_ =~ s/^SECTION:[ 	]*//;
		$cnt++;
		$g_sections[$cnt]=$_;
	} elsif(/^NAME:[ 	]*/)
	{
		$_ =~ s/^NAME:[ 	]*//;
		$cnt++;
		$g_names[$cnt]=$_;
	} elsif(/^MAIL:[ 	]*/)
	{
		$_ =~ s/^MAIL:[ 	]*//;
		$tmp = $g_mails[$cnt];
		if($tmp ne "")
		{
			$tmp = "$tmp<!!!>";
		}
		$tmp = "$tmp$_";
		$g_mails[$cnt] = $tmp;
	} elsif(/^ROLE:[ 	]*/)
	{
		$_ =~ s/^ROLE:[ 	]*//;
		$tmp = $g_roles[$cnt];
		if($tmp ne "")
		{
			$tmp = "$tmp<!!!>";
		}
		$tmp = "$tmp$_";
		$g_roles[$cnt] = $tmp;
	} elsif(/^TEXT:[ 	]*/)
	{
		$_ =~ s/^TEXT:[ 	]*//;
		$tmp = $g_texts[$cnt];
		if($tmp ne "")
		{
			$tmp = "$tmp<!!!>";
		}
		$tmp = "$tmp$_";
		$g_texts[$cnt] = $tmp;
	} elsif(/^NICK:[ 	]*/)
	{
		$_ =~ s/^NICK:[ 	]*//;
		$tmp = $g_nicks[$cnt];
		if($tmp ne "")
		{
			$tmp = "$tmp<!!!>";
		}
		$tmp = "$tmp$_";
		$g_nicks[$cnt] = $tmp;
	}
	#elsif(/^FROM:[ 	]*/)
	#{
	#	$_ =~ s/^FROM:[ 	]*//;
	#	$tmp = $g_contributors[$cnt];
	#	if($tmp ne "")
	#	{
	#		$tmp = "$tmp<!!!>";
	#	}
	#	$tmp = "$tmp$_";
	#	$g_contributors[$cnt] = $tmp;
	#}
}

close(TEMPLATE);

sub p
{
	$t = $_[0];
	$t =~ s/\"/\\\"/g;
	$t =~ s/^[ 	]*//g;
	print OUT "\"$t\" \\\n";
}

open(OUT,">abouttext.inc") or die "Can't open abouttext.inc";

print OUT "static const char * g_szAboutText = \"\" \\\n";

p "<html>";
p " <head>";
p "  <title>Honor and glory</title>";
p " </head>";
p " <body bgcolor=\"#FFFFFF\" text=\"#000000\" link=\"#FF0000\"><center>";


p " <h4><font color=\"#000000\">";
p "This is a partial list of the people that have ";
p "contributed in some way to the KVIrc project.<br><br>";
p "Honor and glory to:<br>";
p " </font></h4>";

$i = 0;
$cnt++;
while($i < $cnt)
{
	if($g_sections[$i] ne "")
	{
		p "<br><table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">";
		p "<tr><td align=\"center\">";
		p "<font color=\"#000000\" size=\"+3\"><b>$g_sections[$i]</b></font>";
		p "</td></tr>";
		p "</table><br><br>";
		$i++;
	}

	p "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">";
	p "<tr><td align=\"center\">";
	p "<font color=\"#000000\" size=\"+2\"><b>$g_names[$i]</b></font>";
	p "</td></tr>";

	if($g_nicks[$i] ne "")
	{

		p "<tr><td align=\"center\"><font color=\"#800020\" size=\"+2\"> (";
		@n = split(/<!!!>/,$g_nicks[$i]);
		$j = 0;
		while($n[$j] ne "")
		{
			if($j > 0){ p ", "; }
			p $n[$j];
			$j++;
		}
		p ")</font></td></tr>";
	}

	if($g_mails[$i] ne "")
	{
		p "<tr><td align=\"center\"><font color=\"#0000FF\" size=\"+1\">";
		@n = split(/<!!!>/,$g_mails[$i]);
		$j = 0;
		while($n[$j] ne "")
		{
			if($j > 0){ p "<br>"; }
			p "[";
			p $n[$j];
			p "]";
			$j++;
		}
		p "</font></td></tr>";
	}

	if($g_roles[$i] ne "")
	{
		p "<tr><td align=\"center\"><font color=\"#303030\">";
		@n = split(/<!!!>/,$g_roles[$i]);
		$j = 0;
		while($n[$j] ne "")
		{
			if($j > 0){ p "<br>"; }
			p $n[$j];
			$j++;
		}
		p "</font></td></tr>";
	}

	if($g_texts[$i] ne "")
	{
		p "<tr><td align=\"center\"><font color=\"#606060\"><i>";
		@n = split(/<!!!>/,$g_texts[$i]);
		$j = 0;
		while($n[$j] ne "")
		{
			if($j > 0){ p "<br>" ; }
			p "\"$n[$j]\"";
			$j++;
		}
		p "</i></font></td></tr>";
	}

	if($g_contributors[$i] ne "")
	{
		p "<tr><td align=\"center\"><font color=\"#FF0000\">";
		@n = split(/<!!!>/,$g_contributors[$i]);
		$j = 0;
		while($n[$j] ne "")
		{
			if($j > 0){ p "<br>" ; }
			p $n[$j];
			$j++;
		}
		p "</font></td></tr>";
	}

	p "</table><br><br>";
	$i++;
}

p "<br>";
p "This list is far from being complete.<br>";
p "A lot of more or less \"anonymous\" people have wasted their time, ";
p "eyes, bandwidth and brain resources on this project.<br>";
p "If you're one of those, then well, this dialog is also for you :)<br>";

p " </center><body>";
p "</html>";

print OUT "\"\";\n";

close(OUT);
