#!/usr/bin/perl
# nhtohtml.pl: A script to generate the nethack bestiary.
# Copyright (C) 2004 Robert Sim (rob@simra.net)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

my $rev = '$Revision: 1.8 $ ';
my ($version) = $rev=~ /Revision:\s+(.*?)\s?\$/;

print STDERR <<EOF;
nhtohtml.pl version $version, Copyright (C) 2004 Robert Sim 
This program comes with ABSOLUTELY NO WARRANTY. This is free software,
and you are welcome to redistribute it under certain conditions.
EOF

my $nethome = "$ENV{HOME}/src/nethack-3.4.3";
my $nethome = "/home/surf/nethack/translation/nethack-de";
# my $nethome = "/home/surf/nethack/versions/nethack-3.4.2";


# Parse monsym.h, which defines the tty characters for the various
# monsters.
my %symbols;
open MONSYM, "<${nethome}/include/monsym.h" or die $!;
while ($l=<MONSYM>) {
  if ($l=~/DEF_(.*?)\s+'(.)'/) {
    my ($mon,$sym)=($1,$2);
    $sym='@' if $sym =~/\s/; # Ghosts and shades will be colored black.
    $symbols{$mon}=$sym;
#    print "$1: $2\n";
  }
}
close MONSYM;

# Parse monflag.h, which defines all the miscellaneous monster flags.
# The devteam has been kind enough to comment each flag in a nice way,
# so we don't need to do much fixing up.
my %flags;
open MONFLAG, "<${nethome}/include/monflag.h" or die $!;
while ($l=<MONFLAG>) {
  if ($l=~/^\#define (.*?)\s+\S+\s+\/\*(.*?)\*\//) {
    $flags{$1}=$2;
#    print "$1: $2\n";
  }
}
close MONFLAG;

# Fixes to strange flag comments.
$flags{M1_OMNIVORE}=" Allesfresser ";


# We define the colors by hand. They're all rough guesses.
my %colors=
  (
   CLR_BLACK =>"404040",
   CLR_RED => "880000",
   CLR_GREEN => "008800",
   CLR_BROWN => "888800", # Low-intensity yellow
   CLR_BLUE => "000088",
   CLR_MAGENTA	=> "880088",
   CLR_CYAN	=> "008888",
   CLR_GRAY	=> "888888",
   NO_COLOR	=> "000000",
   CLR_ORANGE	=> "ffaa00",
   CLR_BRIGHT_GREEN => "00FF00",
   CLR_YELLOW => "ffff00",
   CLR_BRIGHT_BLUE  => "0000FF",
   CLR_BRIGHT_MAGENTA => "ff00ff",
   CLR_BRIGHT_CYAN	=> "00ffff",
   CLR_WHITE	=> "FFFFFF"
);


# TODO: Parse the attacks from the comments in the appropriate header.
my %attacks = 
  (
   AT_CLAW => "Claw",
   AT_BITE => "Bite",
   AT_KICK => "Kick",
   AT_BUTT => "Head butt",
   AT_TUCH => "Touch",
   AT_STNG => "Sting",
   AT_HUGS => "Hug",
   AT_SPIT => "Spit",
   AT_ENGL => "Engulf",
   AT_BREA => "Breath",
   AT_EXPL => "Explode",
   AT_BOOM => "Explode (when killed)",
   AT_GAZE => "Gaze",
   AT_TENT => "Tentacles",
   AT_WEAP => "Weapon",
   AT_MAGC => "Magic"
  );

# TODO: Again, parse the damage comments in the header and just fix
# whatever seems broken.
my %damage =
  (
 AD_PHYS=>	" gewöhnlicher Schaden ",
 AD_MAGM=>	" magic missile ",
 AD_FIRE=>	" fire  ",
 AD_COLD=>	" frost ",
 AD_SLEE=>	" sleep ray ",
 AD_DISN=>	" disintegration (death ray) ",
 AD_ELEC=>	" shock ",
 AD_DRST=>	" drains strength (poison) ",
 AD_ACID=>	" acid ",
 AD_SPC1=>	" buzz ",
 AD_SPC2=>	" buzz ",
 AD_BLND=>	" blinding (yellow light) ",
 AD_STUN=>	" stun ",
 AD_SLOW=>	" slow ",
 AD_PLYS=>	" paralysing ",
 AD_DRLI=>	" drain life levels (Vampire) ",
 AD_DREN=>	" drain magic energy ",
 AD_LEGS=>	" legs (xan) ",
 AD_STON=>	" petrifies (Medusa, cockatrice) ",
 AD_STCK=>	" sticks to you (mimic) ",
 AD_SGLD=>	" steals gold (leppie) ",
 AD_SITM=>	" steals item (nymphs) ",
 AD_SEDU=>	" seduces & steals multiple items ",
 AD_TLPT=>	" teleports you (Quantum Mech.) ",
 AD_RUST=>	" rusts armour (Rust Monster)",
 AD_CONF=>	" confuses (Umber Hulk) ",
 AD_DGST=>	" digests opponent (trapper, etc.) ",
 AD_HEAL=>	" heals opponent's wounds (nurse) ",
 AD_WRAP=>	" special \"stick\" for eel ",
 AD_WERE=>	" confers lycanthropy ",
 AD_DRDX=>	" drains dexterity (quasit) ",
 AD_DRCO=>	" drains constitution ",
 AD_DRIN=>	" drains intelligence (mind flayer) ",
 AD_DISE=>	" confers diseases ",
 AD_DCAY=>	" decays organics (brown Pudding) ",
 AD_SSEX=>	" Succubus seduction (extended) ",
 AD_HALU=>	" causes hallucination ",
 AD_DETH=>	" for Death only ",
 AD_PEST=>	" for Pestilence only ",
 AD_FAMN=>	" for Famine only ",
 AD_SLIM=>	" turns you into green slime ",
 AD_ENCH=>	" remove enchantment (disenchanter) ",
 AD_CORR=>	" corrode armor (black pudding) ",

 AD_CLRC=>	" random clerical spell ",
 AD_SPEL=>	" random magic spell ",
 AD_RBRE=>	" random breath weapon ",

 AD_SAMU=>	" hits, may steal Amulet (Wizard) ",
 AD_CURS=>	" random curse (ex. gremlin) ",
);

# Some monster names appear twice (were-creatures).  We use the
# mon_count hash to keep track of them and flag cases where we need to
# specify. 
my %mon_count;


# The main monster parser.  Takes a MON() construct from monst.c and
# breaks it down into its components.
my @monsters;
sub process_monster {
  my $the_mon=shift;
  ($name) = $the_mon=~/\s+MON\((NAM_.*?),/;
  $the_mon=~s/\s//g;
  $the_mon=~s/\/\*.*?\*\///g;
  ($sym,$lvl,$gen,$atk,$siz,$mr1,$mr2,$flg1,$flg2,$flg3,$col) = 
    $the_mon=~/MON\(NAM_.*,S_(.*?),LVL\((.*?)\),\(?(.*?)\)?,A\((.*)\),SIZ\((.*)\),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?)\),$/;
  $col= "NO_COLOR" if ($name eq "ghost" || $name eq "shade");
  my $mon_struct=
    {
     NAME => $name,
     SYMBOL => $sym,
     LEVEL => parse_level($lvl),
     GEN => $gen,
     ATK => parse_attack($atk),
     SIZE => parse_size($siz),
     MR1 => $mr1,
     MR2 => $mr2,
     FLGS => $flg1."|".$flg2."|".$flg3,
     COLOR => $col
    };
  
  # TODO: Automate this from the headers too.
  $mon_struct->{COLOR}=~s/HI_DOMESTIC/CLR_WHITE/;
  $mon_struct->{COLOR}=~s/HI_LORD/CLR_MAGENTA/;
  $mon_struct->{COLOR}=~s/HI_OBJ/CLR_MAGENTA/;
  $mon_struct->{COLOR}=~s/HI_METAL/CLR_CYAN/;
  $mon_struct->{COLOR}=~s/HI_COPPER/CLR_YELLOW/;
  $mon_struct->{COLOR}=~s/HI_SILVER/CLR_GRAY/;
  $mon_struct->{COLOR}=~s/HI_GOLD/CLR_YELLOW/;
  $mon_struct->{COLOR}=~s/HI_LEATHER/CLR_BROWN/;
  $mon_struct->{COLOR}=~s/HI_CLOTH/CLR_BROWN/;
  $mon_struct->{COLOR}=~s/HI_ORGANIC/CLR_BROWN/;
  $mon_struct->{COLOR}=~s/HI_WOOD/CLR_BROWN/;
  $mon_struct->{COLOR}=~s/HI_PAPER/CLR_WHITE/;
  $mon_struct->{COLOR}=~s/HI_GLASS/CLR_BRIGHT_CYAN/;
  $mon_struct->{COLOR}=~s/HI_MINERAL/CLR_GRAY/;
  $mon_struct->{COLOR}=~s/DRAGON_SILVER/CLR_BRIGHT_CYAN/;
  $mon_struct->{COLOR}=~s/HI_ZAP/CLR_BRIGHT_BLUE/;
  
  push @monsters, $mon_struct;
  print STDERR "$mon_struct->{NAME} ($symbols{$mon_struct->{SYMBOL}}): $mon_struct->{LEVEL}->{LVL}\n";
$mon_count{$name}++;
};


# Parse a LVL() construct.
sub parse_level {
  my $lvl=shift;
  my ($lv,$mov,$ac,$mr,$aln)=$lvl=~/(.*?),(.*?),(.*?),(.*?),(.*?)/;
  my $l=
    {
     LVL => $lv,
     MOV => $mov,
     AC => $ac,
     MR => $mr,
     ALN => ($aln==0)?"Neutral":($aln<0)?"Chaotisch":"Rechtschaffen"
    };
}

# Parse an A(ATTK(),...) construct
sub parse_attack {
  my $atk=shift;
  my $astr=[];
  while ($atk=~/ATTK\((.*?),(.*?),(.*?),(.*?)\)/g) {
    my $a=
      {
       AT => $1,
       AD => $2,
       N => $3,
       D => $4
      };
    push @$astr, $a;
  }
  return $astr;
}

# Parse a SIZ() construct
sub parse_size {
  my $siz=shift;
  my ($wt, $nut, $pxl, $snd, $sz)=$siz=~/(.*?),(.*?),(.*?),(.*?),(.*?)/;
  my $s=
    {
     WT => $wt,
     NUT => $nut,
     PXL => $pxl,
     SND => $snd,
     SIZ => $sz
    };
}

# Read the text descriptions from the help database.
open DBASE, "<${nethome}/dat/data.base" or die $!;
my @entries;
my $tags=[];
my $entry="";
while ($l=<DBASE>) {
  next if ($l=~/^#/); # Ignore comments
  # Lines beginning with non-whitespace are tags
  if ($l=~/^\S/) { 
    # If $entry is non-empty, then the last entry is done, push it.
    if ($entry) {
#      print STDERR "Entry:\n@{$tags}\n$entry\n";
      push @entries,
	{
	 TAGS => $tags,
	 ENTRY => $entry
	};
      # Reset for the next entry.
      $tags=[];
      $entry="";
    }
    chomp $l;
    # Set up the tag for future pattern matches.
    $l=~s/\*/.*/g;
    $l=~s/\~/\\~/;
    # There can be multiple tags per entry.
    push @$tags, $l;
  } else {
    $entry.=$l."<br/>";
  }
}

# The main monst.c parser.
open MONST, "<${nethome}/src/monst.c" or die $!;
my $sex_attack="";
my $having_sex=0;
my $curr_mon=0;
my $the_mon;
while ($l=<MONST>) {
  chomp $l;
  # Monsters are defined with MON() declarations
  if ($l=~/^\s+MON/) {
    $curr_mon=1;
    $the_mon="";
  }
  # Succubi need special handling for their attack strings.
  if ($l=~/^\# define SEDUCTION_ATTACKS/) {
    $having_sex=1;
    #    print STDERR "Setting sex attacks\n";
  } elsif ($having_sex) {
    $sex_attack.=$l;
    $sex_attack=~s/\\//;
    $having_sex=0 if (! ($l=~/\\/)); # If there's no backslash
                                     # continuation, then we're done.
    # print STDERR "Sex: $sex_attack/;
  } else {
    # Do the substitution if SED_ATK appears in $l.  We can count on
    # $sex_attack being properly defined.
    $l=~s/SEDUCTION_ATTACKS/$sex_attack/;
  }
  # Not re-setting r,l to 0 here seems to work better. Not sure why.
  if ($curr_mon) {
    $the_mon.=$l;
    $l_brack+= scalar(split /\(/, $l)-1;
    $r_brack+= scalar(split /\)/, $l)-1;
    # If left and right brackets balance, we're done reading a MON()
    # declaration. Process it.
    if (($l_brack-$r_brack)==0) {
      $curr_mon=0;
      process_monster($the_mon);
    }
  }
}

# Now get ready to spit out the results.
`mkdir html` unless -e html;

# The main index file. We use stylesheets. Sorry.
open INDEX, ">html/index.html" or die $!;
print INDEX <<EOF;
<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
 "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<link rel="stylesheet" href="nhstyle.css" type="text/css"/>
<meta <meta name="keywords" content="nethack,games,rpg"/>
<title>Ye Olde Nethack Bestiary</title>
</head>
<body>
<div style="text-align:center;margin:0 auto;">
<script type="text/javascript"><!--
google_ad_client = "pub-8689903183399668";
google_ad_width = 468;
google_ad_height = 60;
google_ad_format = "468x60_as";
google_ad_type = "text_image";
google_ad_channel ="";
google_color_border = "336699";
google_color_bg = "FFFFFF";
google_color_link = "0000FF";
google_color_url = "008000";
google_color_text = "000000";
//--></script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
</div>
<h1>Ye Olde Nethack Bestiary</h1>
<ul>
EOF

# For each monster, create the html.
my $last_html="";
while ($m=shift @monsters) {
  # Name generation is due to issues like were-creatures with multiple
  # entries.
  my ($htmlname,$print_name)=gen_names($m);
  # nextprev is just the "navigation bar".
  $nextprev="<div class=\"nextprev\">";
  $nextprev.=qq(<a href="$last_html">&lt;&lt;</a>) if $last_html;
  $nextprev.=qq( <a href="index.html">^^</a> );
  if ($monsters[0]) {
    ($nexthtml,$foo)=gen_names($monsters[0]);
    $nextprev.=qq(<a href="$nexthtml">&gt;&gt;</a>);
  }
  $nextprev.="</div>";
  print STDERR "HTML: $htmlname\n";

  # The index entry.
  print INDEX qq(<li><div style="display:inline;color:#$colors{$m->{COLOR}};">$symbols{$m->{SYMBOL}}</div> <a href="$htmlname">$print_name</a></li>\n);

  # The html file.  I think the symbol stuff is really pretty. :-)
  open HTML, ">html/$htmlname" or die $!;
  print HTML <<EOF;
<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
 "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<link rel="stylesheet" href="nhstyle.css" type="text/css"/>
<meta name="keywords" content="nethack,games,rpg"/>
<title>$print_name</title>
</head>
<body>
<div style="text-align:center;margin:0 auto;">
<script type="text/javascript"><!--
google_ad_client = "pub-8689903183399668";
google_ad_width = 468;
google_ad_height = 60;
google_ad_format = "468x60_as";
google_ad_type = "text_image";
google_ad_channel ="";
google_color_border = "336699";
google_color_bg = "FFFFFF";
google_color_link = "0000FF";
google_color_url = "008000";
google_color_text = "000000";
//--></script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
</div>
$nextprev
<div class="symbol" style="color:#$colors{$m->{COLOR}};">$symbols{$m->{SYMBOL}}</div>
<h1>$print_name</h1>
<hr class="separator1"/>
<h2>Level $m->{LEVEL}->{LVL}</h2> 
<div class="attribute">Rüstungsklasse: $m->{LEVEL}->{AC}</div>
<div class="attribute">Geschwindigkeit: $m->{LEVEL}->{MOV}</div>
<div class="attribute">Magieresistenz: $m->{LEVEL}->{MR}</div>
<div class="attribute">Gesinnung: $m->{LEVEL}->{ALN}</div>
EOF
  # If the monster has any attacks, produce an attack section.
  if (scalar(@{$m->{ATK}})) {
    print HTML <<EOF;
<div class="attack">
<h3>Angriffe:</h3>
<ul>
EOF
    for $a (@{$m->{ATK}}) {
      print HTML "<li>$attacks{$a->{AT}}: $a->{N}d$a->{D} $damage{$a->{AD}} damage</li> \n"
    }
    print HTML <<EOF;
</ul>
</div>
EOF
  }
print HTML <<EOF;
<div style="position:relative;">
EOF

  # If the monster has any conveyances from ingestion, produce a
  # conveyances section.
  if ($m->{MR2}) {
    print HTML <<EOF;
<div class="convey">
<h3>Conveyances:</h3>
<ul>
EOF
    for $mr (split /\|/, $m->{MR2}) {
      my $r=$flags{$mr};
      $r=~s/resists//;
      $r.=" resistance";
      print HTML "<li>$r</li> ";
#      print "$mr: $r\n";
    }
    print HTML "</ul></div>\n";
  }

  # Same for resistances.  We did conveyances first for mysterious css
  # reasons.
  if ($m->{MR1}) {
    print HTML <<EOF;
<div class="resist">
<h3>Resistances:</h3>
<ul>
EOF
    for $mr (split /\|/, $m->{MR1}) {
      print HTML "<li>$flags{$mr}</li> ";
#      print "$mr: $flags{$mr}\n";
    }
    print HTML "</ul></div>\n";
  }

print HTML <<EOF;
</div>
EOF

  # No output all the other flags of interest.
  if (!($m->{FLGS} eq "0|0|0")) {
    print HTML <<EOF;
<div class="flags">
<h3>Andere Attribute:</h3>
<ul>
EOF
    for $mr (split /\|/, $m->{FLGS}) {
      print HTML "<li>$flags{$mr}</li> " if ($mr && !($mr eq "0L"));
#      print "$mr: $flags{$mr}\n";
    }
    print HTML "</ul></div>\n";
  }

  # The help file entry, and the footer.
  my $entry=lookup_entry($m->{NAME});
print HTML <<EOF;
<hr class="separator1"/>
<div>$entry</div>
<hr class="separator1"/>
<div>$nextprev<br/>
<p>
Generated by nhtohtml $version. Author: <a href="http://www.cs.ubc.ca/~simra/">Robert Sim</a><br/>

<div style="text-align:center;margin:0 auto;">
<script type="text/javascript"><!--
google_ad_client = "pub-8689903183399668";
google_ad_width = 468;
google_ad_height = 60;
google_ad_format = "468x60_as";
google_ad_type = "text_image";
google_ad_channel ="";
google_color_border = "336699";
google_color_bg = "FFFFFF";
google_color_link = "0000FF";
google_color_url = "008000";
google_color_text = "000000";
//--></script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
</div>
</p>
</div>
</body>
</html>
EOF
  close HTML;
  $last_html=$htmlname;
}

# Polish off the index file.
print INDEX <<EOF;
</ul>
<p>Generated by nhtohtml $version. Author: <a href="http://www.cs.ubc.ca/~simra/">Robert Sim</a>

<div style="text-align:center;margin:0 auto;">
<script type="text/javascript"><!--
google_ad_client = "pub-8689903183399668";
google_ad_width = 468;
google_ad_height = 60;
google_ad_format = "468x60_as";
google_ad_type = "text_image";
google_ad_channel ="";
google_color_border = "336699";
google_color_bg = "FFFFFF";
google_color_link = "0000FF";
google_color_url = "008000";
google_color_text = "000000";
//--></script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
</div>
</p>
</body>
</html>
EOF
close INDEX;


# Produce the style sheet.
open STYLE, ">html/nhstyle.css" or die $!;
print STYLE <<EOF;

body {
  font-family: arial, sans-serif;
  background: #000000;
  color: #d0d0d0;
  width: 500px;
  position: relative;
  top: 20%;
}

a {
  color: #ffffff;
}

h1 {
}

.attribute {
  font-weight: bold;
}

.separator1 {
  clear: both;
  height: 4px;
  color: #ffffff;
  background-color: #ffffff;
  border: 0;
}

.symbol {
  font-family:courier;
  float:right;
  font-size:40pt;
  background:#000000;
}

.attack ul {
  margin-top: 0px;
  padding-top: 0px;
  margin-bottom: 0px;
  padding-bottom: 0px;
}
.attack li {
  font-style: normal;
}

.resist {
  float: left;
/*  position: absolute;
  top: 0px;
  left: 0px; */
}

.convey {
  position: absolute;
  top: 0px;
  left: 250px; 
}

.flags {
  clear: both;
}

EOF


# Handy subs follow...

# Generate html filenames, and the monster's name.
sub gen_names {
  my $m=shift;
  my $htmlname="$m->{NAME}.html";
  $htmlname=~s/\s/_/g;
  my $print_name=$m->{NAME};
  if ($mon_count{$m->{NAME}}>1) {
    my $symbol=$m->{SYMBOL};
    $symbol=~tr/A-Z/a-z/;
    $htmlname=~s/.html/_$symbol.html/;
    $print_name.=" ($symbol)";
  }
  ($htmlname,$print_name);
}

# Lookup a monster's entry in the help database.
sub lookup_entry {
  my $name=shift;
ENTRY_LOOP:  for $e (@entries) {
    for $pat (@{$e->{TAGS}}) {
#      print STDERR "Pattern: $pat\n";
      if ($name=~/^$pat$/i) {
	next ENTRY_LOOP if ($pat=~/^\\\~/); # Tags starting with ~ say
                                            # don't match this entry.
# print STDERR "Found entry for $name\n";
	return $e->{ENTRY};
      }
    }
  }
}


