#!/bin/bash
##########################################
#
#
#
##########################################



filename="user_settings.txt"

if [ -e "$filename" ]
then
    # call the script
    echo "user_settings are already generated"
    echo "starting framework main driver"
    cd out/build/
    make
    cd ../../
    

else
    # create user_settings from console input
    python3 generate_user_settings.py $filename
    if [ -e "$filename" ]
    then
        clear
        echo "user_settings are generated"
        echo "starting framework main driver"
        cd out/build/
        make
        ./Crypto 
    fi
fi

