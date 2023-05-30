#!/bin/bash

usage() {
	
	echo "Usage: $0 [-h|--help] [-app appname] [-var type,name,min,max,event,default]"
	exit 1
}

#usage

#TREPO=`mktemp -u template.XXXX`
#echo TREPO=${TREPO}
TOPPATH=$(readlink -f "$0")
TOPPATH=`dirname ${TOPPATH}`
echo TOPPATH=${TOPPATH}

APP=BCM


wget -N https://star.inp.nsk.su/~bekhte/genlib-example.git/template.tar

for f in `tar --list -f template.tar`; do
	DEST=`echo $f|sed "s/_TEMPLATE_/$APP/g"`
	if [ -e "$DEST" ]; then
		echo "file exist: $f  ... skip"
	else
		case "$DEST" in
		*/)
			echo mkdir -p $DEST
			mkdir -p $DEST
			;;
		*.sh)
			echo "$f -> $DEST"
			tar xf template.tar  --to-stdout $f | sed "s/_TEMPLATE_/$APP/g" > $DEST
			chmod +x $DEST
			;;
		*)
			echo "$f -> $DEST"
			tar xf template.tar  --to-stdout $f | sed "s/_TEMPLATE_/$APP/g" > $DEST
			;;
		esac
	fi
done

if [ ! -d genlib ]; then
	git clone https://star.inp.nsk.su/~bekhte/genlib.git
fi

