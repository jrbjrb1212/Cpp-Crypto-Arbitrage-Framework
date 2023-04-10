#!/bin/bash
##########################################
#
#
#
##########################################

filename="user_settings.txt"

python3 generate_user_settings.py $filename
cp $filename out/build/
contents=$(cat "$filename")
coinReq=$(echo "$contents" | grep -oP '(?<=coinReq=)[0-9]+')
volReq=$(echo "$contents" | grep -oP '(?<=volReq=)[0-9]+(\.[0-9]+)?')
python3 Symbol_Data_Files/request_coingecko_api.py $coinReq $volReq
mv Viable_Trading_Pairs.txt Symbol_Data_Files/

if [ -e "$filename" ]
then
    clear
    echo "user_settings are generated"
    echo "building framework"
    cmake -S ../ -B out/build/
    cd out/build/
    make
    echo "finished framework build"
fi