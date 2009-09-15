#!/bin/bash

# Tool um die Patches in NetHack-De einzufügen und zu entfernen

PATCHES="
nethack-applied-cream-pie-C343-218.diff
nethack-long-worm-fix-C343-324.diff
nh343-astral-call-fix.v1.0.patch
nh343-randomized-sokoban-prize.v1.0.patch
xlog-v3.patch 
nh343-acid-C343-179.diff
nh343-simple_mail.diff
qwertz-343.diff
nethack.unfoog.de-livelog.patch
nh343-menucolor-v1.5.diff
nethack-autoconf-20081120.patch
nethack-3.4.1-pickup_thrown.1.diff
"

NH_DIR=/tmp/patches_nethack$$

# reverse patching?
[[ $1 == "-R" ]] && {
	shift
	REVERSE="-R"
	PATCHES=`echo $PATCHES | tr -s ' ' '\n'| tac`
}

function die() { echo -e "\033[1;31mError:\033[0;31m $@\033[0m\n"; exit 1; }
function trenner() { echo -e "\033[0;33m================================================== \033[1;34m${1}\033[0m"; }

apply_patch() {
	trenner
	echo $1
	trenner

	echo "patch -d $2 $REVERSE --fuzz=0 -p1 --dry-run < $1"
	patch -d $2 $REVERSE --fuzz=0 -p1 --dry-run < $1
	if [ $? != 0 ]
	then
		die $1 "failed"
	fi

	patch -d $2 $REVERSE --fuzz=0 -p1 --quiet < $1
	echo 
}

create_tmp_directory() {
	cp -r .. $NH_DIR
}

create_tmp_directory

cd $NH_DIR/patches

for i in $PATCHES
do
	apply_patch $i `pwd`/../
done
