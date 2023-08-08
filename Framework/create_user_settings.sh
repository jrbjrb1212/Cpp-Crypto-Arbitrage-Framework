#!/bin/bash
#############################################
#                                           #
# Script to generate user setting via the   #
# generate_user_setting.py program.         #
# Result of this script is a txt with user  #
# defined settings and a generated txt      #
# of trading pairs.                         #
#                                           #
#############################################

filename="user_settings.txt"

python3 generate_user_settings.py $filename
cp $filename out/build/
contents=$(cat "$filename")
coinReq=$(echo "$contents" | awk -F '[= ]' '/coinReq=/{print $2}')
volReq=$(echo "$contents" | awk -F '[= ]' '/volReq=/{print $2}')
python3 Symbol_Data_Files/request_coingecko_api.py $coinReq $volReq
mv Viable_Trading_Pairs.txt Symbol_Data_Files/

if [ -e "$filename" ]
then
    # clear
    echo "user_settings are generated"
    echo "building framework"
    cmake -S . -B out/build/
fi

