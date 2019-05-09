#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
VNAME="archie"
MOOS_PORT="9002"
UDP_LISTEN_PORT="9202"
SHORE_LISTER="9200"
COOL_FAC=50
COOL_STEPS=1000
CONCURRENT="true"
ADAPTIVE="false"


for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
	printf "  --vname=VNAME      \n" 
	printf "  --help, -h         \n"
	printf "  --warp=WARP_VALUE      \n"
	printf "  --adaptive, -a         \n"
	printf "  --unconcurrent, -uc       \n"
	printf "  --angle=DEGREE_VALUE   \n"
	printf "  --cool=COOL_FAC        \n"
	exit 0;
    elif [ "${ARGI:0:8}" = "--vname=" ] ; then
        VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	  JUST_MAKE="yes"
    elif [ "${ARGI:0:6}" = "--warp" ] ; then
        WARP="${ARGI#--warp=*}"
        UNDEFINED_ARG=""
    elif [ "${ARGI:0:6}" = "--cool" ] ; then
        COOL_FAC="${ARGI#--cool=*}"
        UNDEFINED_ARG=""
    elif [ "${ARGI:0:7}" = "--angle" ] ; then
        DEGREES1="${ARGI#--angle=*}"
        UNDEFINED_ARG=""
    elif [ "${ARGI}" = "--unconcurrent" -o "${ARGI}" = "-uc" ] ; then
        CONCURRENT="false"
        UNDEFINED_ARG=""
    elif [ "${ARGI}" = "--adaptive" -o "${ARGI}" = "-a" ] ; then
        ADAPTIVE="true"
        UNDEFINED_ARG=""
    elif [ "${ARGI:0:8}" = "--shore=" ] ; then
        SHOREIP="${ARGI#--shore=*}"
    elif [ "${ARGI:0:8}" = "--mport=" ] ; then
        MOOS_PORT="${ARGI#--shore=*}"
    elif [ "${ARGI:0:8}" = "--lport=" ] ; then
        UDP_LISTEN_PORT="${ARGI#--shore=*}"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------

VNAME1="archie"
VNAME2="betty"

START_POS="0,0"

#start first vehicle:                                                                                                                                                                                                                         
nsplug meta_vehicle.moos targ_$VNAME.moos -f WARP=$TIME_WARP  \
   VNAME=$VNAME      START_POS=$START_POS                    \
   VPORT=$MOOS_PORT       SHARE_LISTEN=$UDP_LISTEN_PORT                   \
   SHOREIP=$SHOREIP SHORE_LISTEN=$SHORE_LISTEN                 \
   VTYPE=KAYAK          COOL_FAC=$COOL_FAC  COOL_STEPS=$COOL_STEPS\
   CONCURRENT=$CONCURRENT  ADAPTIVE=$ADAPTIVE  SHORE=$SHORE     \



nsplug meta_vehicle.bhv targ_$VNAME.bhv -f VNAME=$VNAME      \
    START_POS=$START_POS VNAME1=$VNAME1 VNAME2=$VNAME2       



if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME.moos >& /dev/null &

uMAC targ_$VNAME.moos

kill %1 
printf "Done.   \n"


