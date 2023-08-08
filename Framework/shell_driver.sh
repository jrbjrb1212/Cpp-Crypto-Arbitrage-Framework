#!/bin/bash
#################################################
#                                               #
# Shell script to run the arbitrage framework   #
# - A single execution of this will facilite    #
#   user setting creation and execute           #
#   iterations of the frame work                #
#                                               #
#################################################



filename="user_settings.txt"
fileCopyLoc="out/build/user_settings.txt"

if [ -e "$filename" ]
then
    if diff "$filename" "$fileCopyLoc" > /dev/null
    then
        # build the framework
        echo "user_settings are already generated"
        echo "building framework"
        echo ""
        cmake -S . -B out/build/
    else
        bash create_user_settings.sh
    fi
    
else
    # create user_settings from console input
    bash create_user_settings.sh
fi

clear
cd out/build/
# compile the frame work
make
echo ""
echo "finished framework build"  
./Crypto