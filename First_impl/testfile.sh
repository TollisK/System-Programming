#!/bin/bash

#Dexetai ta orismata 
virusfile=$1
countriesFile=$2
numLines=$3
duplicatesAllowed=$4

#An den exei 4 akribws orismata tote exit
if [ "$#" -ne 4 ]
then
    echo "Error with arguments!"
    exit 1
fi
#An den yparxei to arxeio gia tous ioys h xwres tote exit
if [ ! -f $virusfile ]; then
    echo "virus file not found!"
    exit 2
fi
if [ ! -f $countriesFile ]; then
    echo "country file not found!"
    exit 3
fi
#Katharizei to arxeio inputFile
>inputFile
#Ftiaxnei 3 arrays
declare -a arrayvirus;
declare -a arraycountry;
declare -a arrayid;

#Topothetei ta dedomena apo ta arxeia stous pinakes gia thn grhgorh eyresh stoixeiwn
while IFS= read -r line; do arrayvirus+=("$line"); done < $virusfile
while IFS= read -r line; do arraycountry+=("$line"); done < $countriesFile
for ((i=0;i<$numLines;i++))
do
    citizenid="$(($RANDOM%9999))"
    #An den afhnoyme diplotypa
    if [ $duplicatesAllowed -eq 0 ];
    then
        #An to id yparxei hdh ston pinaka tote 3anatrexei ton broxo
        if [[ " ${arrayid[@]} " =~ " ${citizenid} " ]]; then
            i=$i-1
            continue;
        fi
    else
        #Alliws yparxei h pithanothta 1/5 na 3anakanei diplotypo
        random=$(($RANDOM%5))
        if [ $random -eq 1 ] && [ $i -ne 0 ]
        then
            num=${#arrayid[@]}
            r_num=$(($RANDOM%$num))
            citizenid=${arrayid[$r_num]}
        fi
    fi
    #Vazei to id ston pinaka
    arrayid+=("$citizenid")
    #Vriskei ta dedomena
    random=$(($RANDOM%9 + 3))
    name=$(cat /dev/urandom | tr -dc 'a-zA-Z' | fold -w $random | head -n 1)
    random=$(($RANDOM%9 + 3))
    lastname=$(cat /dev/urandom | tr -dc 'a-zA-Z' | fold -w $random | head -n 1)
    num=${#arraycountry[@]}
    r_num=$(($RANDOM%$num))
    country=${arraycountry[$r_num]}
    age=$(($RANDOM%120 + 1))
    num=${#arrayvirus[@]}
    r_num=$(($RANDOM%$num))
    virus=${arrayvirus[$r_num]}
    r_num=$(($RANDOM%2))
    #An einai 0 tote tha thewrhtai ws nai
    if [ "$r_num" -eq "0" ]
    then
        date1=$(($RANDOM%30 + 1))
        date2=$(($RANDOM%12 + 1))
        date3=$(($RANDOM%100 + 2000))
        printf "$citizenid $name $lastname $country $age $virus YES $date1-$date2-$date3\n">>inputFile
    else
        printf "$citizenid $name $lastname $country $age $virus NO\n">>inputFile
    fi
done
exit 0