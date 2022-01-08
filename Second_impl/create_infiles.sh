#!/bin/bash

#Dexetai ta orismata 
inputfile=$1
inputdir=$2
numFiles=$3

#An den exei 4 akribws orismata tote exit
if [ "$#" -ne 3 ]
then
    echo "Error with arguments!"
    exit 1
fi
#An den yparxei to arxeio gia tous ioys h xwres tote exit
if [ ! -f $inputfile ]; then
    echo "Input file not found!"
    exit 2
fi
if [ -d $inputdir ]; then
    echo "The directory already exists!"
    exit 2
fi

#dhmiourgei to directory
mkdir $inputdir;

#Pinakas gia thn apothhkeysh twn xwrwn
declare -a arraycountry;
#Pinakas poy gia kathe xwra exei ena noymero gia to arxeio poy tha mpei h epomenh grammh
declare -a arraynum;
#Gia kathe grammh sto inputfile pairnoyme ta dedomena
while read -r citid name surname country age virus flag date; do
    #An h xwra yparxei hdh
    if [[ " ${arraycountry[@]} " =~ " ${country} " ]]; then
        #Briskei to index ths xwras ayths
        for i in "${!arraycountry[@]}"; do
            if [[ "${arraycountry[$i]}" = "${country}" ]]; then
                #Vazei ta dedomena sto swsto arxeio
                printf "$citid $name $surname $country $age $virus $flag $date \n">>$inputdir/$country/$country-${arraynum[$i]}.txt
                #Proxwraei sto epomeno arxeio gia thn ylopoihsh round robin
                arraynum[$i]=$((${arraynum[$i]}+1));
                if [ "${arraynum[$i]}" -eq $numFiles ]
                then
                    arraynum[$i]=$((0));
                fi
            fi
        done
    else
        #Topothetei thn xwra ston pinaka
        arraycountry+=("$country");
        #Arxikopoiei kai ena stoixeio toy pinaka arithmwn me 0
        arraynum+=(0);
        #Ftiaxnei toys fakeloys kai ta arxeia antistoixa
        mkdir $inputdir/$country;
        for ((i=0;i<$numFiles;i++))
        do
            touch $inputdir/$country/$country-$i.txt
        done
        for i in "${!arraycountry[@]}"; do
            if [[ "${arraycountry[$i]}" = "${country}" ]]; then
                #Vazei ta dedomena sto swsto arxeio
                printf "$citid $name $surname $country $age $virus $flag $date \n">>$inputdir/$country/$country-${arraynum[$i]}.txt
                #Proxwraei sto epomeno arxeio gia thn ylopoihsh round robin
                arraynum[$i]=$((${arraynum[$i]}+1));
                if [ "${arraynum[$i]}" -eq $numFiles ]
                then
                    arraynum[$i]=$((0));
                fi
            fi
        done

    fi
done < $inputfile
exit 0