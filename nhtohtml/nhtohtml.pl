#!/usr/bin/perl
# nhtohtml.pl: A script to generate the nethack bestiary for NetHack-De.
# Copyright (c) 2008-2010 Patric Mueller (bhaak@gmx.net)
# Copyright (c) 2004 Robert Sim (rob@simra.net)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation.
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

#print STDERR <<EOF;
# nhtohtml.pl version $version, Copyright (C) 2004 Robert Sim 
#This program comes with ABSOLUTELY NO WARRANTY. This is free software,
#and you are welcome to redistribute it under certain conditions.
#EOF

my $nethome = "../";


# Parse monsym.h, which defines the tty characters for the various
# monsters.
my %symbols;
open MONSYM, "<${nethome}/include/monsym.h" or die $!;
while ($l=<MONSYM>) {
  if ($l=~/DEF_(.*?)\s+'(.)'/) {
    my ($mon,$sym)=($1,$2);
    $sym='@' if $sym =~/\s/; # Ghosts and shades will be colored black.
    $sym="&amp;" if $sym eq '&'; # generate valid html
    $sym="'" if $mon eq "GOLEM"; # regexp-bug-fix
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
   AT_CLAW => "Greifer",
   AT_BITE => "Biss",
   AT_KICK => "Tritt",
   AT_BUTT => "Head butt", # TODO
   AT_TUCH => "Berührung",
   AT_STNG => "Stich",
   AT_HUGS => "Umarmung",
   AT_SPIT => "Speit",
   AT_ENGL => "Verschlingen",
   AT_BREA => "Odem", # AT_BREA => "Breath",
   AT_EXPL => "Explodiert",
   AT_BOOM => "Explodiert (als Todesfolge)",
   AT_GAZE => "Blick",
   AT_TENT => "Tentakel", # AT_TENT => "Tentacles",
   AT_WEAP => "Waffe", # AT_WEAP => "Weapon",
   AT_MAGC => "Magie" # AT_MAGC => "Magic"
  );

# TODO: Again, parse the damage comments in the header and just fix
# whatever seems broken.
my %damage =
  (
 AD_PHYS=>	" gewöhnlicher Schaden ",
 AD_MAGM=>	" magische Geschosse ",
 AD_FIRE=>	" Feuer  ",
 AD_COLD=>	" Kälte ",
 AD_SLEE=>	" Schlafstrahl ",
 AD_DISN=>	" Desintegration (Todesstrahl) ",
 AD_ELEC=>	" Stromschlag ",
 AD_DRST=>	" schwächt Stärke (Gift) ",
 AD_ACID=>	" Säure ",
 AD_SPC1=>	" buzz ", # TODO
 AD_SPC2=>	" buzz ", # TODO
 AD_BLND=>	" blendend ",
 AD_STUN=>	" betäubt ",
 AD_SLOW=>	" verlangsamt ",
 AD_PLYS=>	" paralysierend ",
 AD_DRLI=>	" drain life levels (Vampire) ", # TODO
 AD_DREN=>	" drain magic energy ", # TODO
 AD_LEGS=>	" Beine (Xan) ",
 AD_STON=>	" versteinert Gegner",
 AD_STCK=>	" sticks to you (mimic) ", # TODO
 AD_SGLD=>	" stiehlt Gold ",
 AD_SITM=>	" stiehlt Gegenstand ",
 AD_SEDU=>	" verführt und stiehlt mehrere Gegenstände ",
 AD_TLPT=>	" teleportiert Spieler ",
 AD_RUST=>	" verrostet Rüstung ",
 AD_CONF=>	" verwirrt ",
 AD_DGST=>	" verdaut Gegner ",
 AD_HEAL=>	" heilt gegnerische Wunde ",
 AD_WRAP=>	" special \"stick\" for eel ", # TODO
 AD_WERE=>	" überträgt Lykanthropie ",
 AD_DRDX=>	" drains dexterity (quasit) ", # TODO
 AD_DRCO=>	" drains constitution ", # TODO
 AD_DRIN=>	" drains intelligence ", # TODO
 AD_DISE=>	" überträgt Krankheiten ",
 AD_DCAY=>	" decays organics (brown Pudding) ", # TODO
 AD_SSEX=>	" Succubus seduction (extended) ",
 AD_HALU=>	" verursacht Halluzinationen ",
 AD_DETH=>	" nur für Tod ",
 AD_PEST=>	" nur für Pestilenz ",
 AD_FAMN=>	" nur für Hunger ",
 AD_SLIM=>	" turns you into green slime ", # TODO
 AD_ENCH=>	" remove enchantment (disenchanter) ", # TODO
 AD_CORR=>	" corrode armor (black pudding) ", # TODO

 AD_CLRC=>	" zufälliger clerical spell ",
 AD_SPEL=>	" zufälliger magischer Zauberspruch ",
 AD_RBRE=>	" zufällige Odemwaffe ",

 AD_SAMU=>	" schlägt, kann Amulet stehlen (Zauberer) ",
 AD_CURS=>	" zufälliger Fluch (z.B. Gremlin) ",
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
  $col= "NO_COLOR" if ($name eq "NAM_GHOST" || $name eq "NAM_SHADE");
  # workaround for inconsistency in naming
  $name= "NAM_ARCHAEOLOGE" if ($name eq "NAM_ARCHEOLOGIST");
  $name= "NAM_BARBAR" if ($name eq "NAM_BARBARIAN");
  $name= "NAM_HEILER" if ($name eq "NAM_HEALER");
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
#  print STDERR "$mon_struct->{NAME} ($symbols{$mon_struct->{SYMBOL}}): $mon_struct->{LEVEL}->{LVL}\n";
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
     ALN => ($aln==0)?"neutral":($aln<0)?"chaotisch":"rechtschaffen"
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
    $l=~s/&/&amp;/g;
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
<meta name="keywords" content="nethack,games,rpg"/>
<title>NetHacks Bestiarium</title>
</head>
<body>
<h1>NetHacks Bestiarium</h1>
<table class="monster_table">
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
#  print STDERR "HTML: htmlname $htmlname\n";
  $print_name=german($m->{NAME});
#  print STDERR "HTML: print_name: $print_name\n";

  # The index entry.
  print INDEX qq(<tr><td class="symbol" align="center"><span style="color:#$colors{$m->{COLOR}};">$symbols{$m->{SYMBOL}}</span></td><td class="name"><a href="$htmlname">$print_name</a></td></tr>\n);

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
<title>Nethacks Bestiarium: $print_name</title>
</head>
<body>
$nextprev
<div class="symbol" style="color:#$colors{$m->{COLOR}};">$symbols{$m->{SYMBOL}}</div>
<h1>$print_name</h1>
<hr class="separator"/>
<h2>Level $m->{LEVEL}->{LVL}</h2> 
<table> 
<tr><td class="attribute">Rüstungsklasse: </td><td align="right">$m->{LEVEL}->{AC}</td></tr>
<tr><td class="attribute">Geschwindigkeit:</td><td align="right">$m->{LEVEL}->{MOV}</td></tr>
<tr><td class="attribute">Magieresistenz: </td><td align="right">$m->{LEVEL}->{MR}</td></tr>
<tr><td class="attribute">Gesinnung:      </td><td align="right">$m->{LEVEL}->{ALN}</td></tr>
</table>
EOF
  # If the monster has any attacks, produce an attack section.
  if (scalar(@{$m->{ATK}})) {
    print HTML <<EOF;
<div class="attack">
<h3>Angriffe:</h3>
<ul>
EOF
    for $a (@{$m->{ATK}}) {
      print HTML "<li>$attacks{$a->{AT}}: $a->{N}d$a->{D} $damage{$a->{AD}}</li> \n"
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
<h3>Eigenschaftsübertragung:</h3>
<ul>
EOF
    for $mr (split /\|/, $m->{MR2}) {
      my $r=$flags{$mr};
      $r=~s/ (.*)resistent /\1/;
      $r.="resistenz";
      $r=ucfirst($r);
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
<h3>Resistenzen:</h3>
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
<h3>Andere Eigenschaften:</h3>
<ul>
EOF
    for $mr (split /\|/, $m->{FLGS}) {
      print HTML "<li>$flags{$mr}</li> " if ($mr && !($mr eq "0L"));
#      print "$mr: $flags{$mr}\n";
    }
    print HTML "</ul></div>\n";
  }

  # The help file entry, and the footer.
  my $entry=lookup_entry("NOUN_".substr($m->{NAME},4));
print HTML <<EOF;
<hr class="separator"/>
<div class="entry">$entry</div>
<hr class="separator"/>
<div>$nextprev<br/>
<p>Erzeugt mit nhtohtml $version. Author: <a href="http://www.cs.ubc.ca/~simra/">Robert Sim</a><br/>
Angepasst für <a href="http://nethack-de.sourceforge.net/">NetHack-De</a> von <a href="http://bhaak.dyndns.org/">Patric Mueller</a>.
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
</table>
<p>Erzeugt mit nhtohtml $version. Author: <a href="http://www.cs.ubc.ca/~simra/">Robert Sim</a><br/>
Angepasst für <a href="http://nethack-de.sourceforge.net/">NetHack-De</a> von <a href="http://bhaak.dyndns.org/">Patric Mueller</a>.
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
  width: 34em;
  position: relative;
  top: 20%;
  padding-left: 1em;
}

table {
  border-collapse: collapse;
  margin-left: 1em;
}

.monster_table td.name {
  padding-left: 0.5em;
}

a {
  color: #ffffff;
}

h1 {
}

.attribute {
  font-weight: bold;
}

.separator {
  clear: both;
  height: 0.1em;
  color: #444;
  background-color: #444;
  border: 0;
}

.entry {
  padding: .75em;
}

.symbol {
  font-family:monospace;
  background:#111111;
  padding: 0.1em;
}

div.symbol {
  font-size:300%;
  float:right;
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
  my $htmlname=lc(substr("$m->{NAME}.html",4));
  $htmlname=~s/....\s/_/g;
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
# print STDERR "Suche Datenbank-Eintrag fuer $name\n";
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
 print STDERR "No database entry found for $name\n";
 return "kein Datenbank-Eintrag\n";
}

# 
sub german {
  my $name=substr(shift,4);
#  print STDERR "$name\n";

  open GERMAN, "<${nethome}/src/german_vocabulary.c" or die $!;

  while ($l=<GERMAN>) {
    if ($l=~/\{"(.*)".*"NOUN_${name}".*nominativ.*n_singular.*ohne/) { 
      # print STDERR "$1\n";
      return $1;
    }
  }
  return "NAM_$name";
}
