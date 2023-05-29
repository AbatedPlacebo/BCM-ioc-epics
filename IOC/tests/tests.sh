#!/bin/bash

# assert.sh
 
assert ()                 #  If condition false,
{                         #+ exit from script with error message.
  E_PARAM_ERR=98
  E_ASSERT_FAILED=99
 
 
  if [ -z "$2" ]          # Not enough parameters passed.
  then
    return $E_PARAM_ERR   # No damage done.
  fi
 
  lineno=$2
 
  if [ ! \( $1 \) ]
  then
    echo "Assertion failed:  \"$1\""
    echo "File \"$0\", line $lineno"
    exit $E_ASSERT_FAILED
  # else
  #   return
  #   and continue executing script.
  fi
}
 
 
#a=5
#b=4
#condition="$a -lt $b"     # Error message and exit from script.
                          #  Try setting "condition" to something else,
                          #+ and see what happens.
 
#assert "$condition" $LINENO

EPICS_ENV="export EPICS_CA_AUTO_ADDR_LIST=NO"
EPICS_ENV="$EPICS_ENV;export EPICS_CA_ADDR_LIST=127.255.255.255"
eval $EPICS_ENV

if [ -x /usr/lib/epics/support/autosave-5-0/bin/$EPICS_HOST_ARCH/asApp ]; then
	AUTOSAVE=/usr/lib/epics/support/autosave-5-0
elif [ -x /usr/lib/epics/bin/$EPICS_HOST_ARCH/asApp ]; then
	AUTOSAVE=/usr/lib/epics
fi

assert "-x $AUTOSAVE/bin/$EPICS_HOST_ARCH/asApp" $LINENO
assert "-e $AUTOSAVE/dbd/as.dbd" $LINENO

export AUTOSAVE

if ! /sbin/pidof caRepeater > /dev/null; then
	echo "caRepeater &"
	caRepeater &
fi

IOCDIR=iocBoot/iocBCM
[ -d $IOCDIR ] || IOCDIR=../$IOCDIR

assert "-d $IOCDIR" $LINENO

(cd $IOCDIR; (echo "dbnr"; while sleep 10; do echo "casr 1"; done) | ../../bin/$EPICS_HOST_ARCH/BCM BCM.cmd ; echo finish ) &

sleep 10

#COUNT=`caget -t V:BCM_ro-I`
#assert "$? -eq 0" $LINENO
#assert "$COUNT -gt 4 -a $COUNT -lt 16" $LINENO
#FF=`caget -t V:BCM-SP`
#assert "$? -eq 0" $LINENO
#assert "$FF -eq -3 -o $FF -eq 3" $LINENO

sleep 1

killall BCM

exit 0

