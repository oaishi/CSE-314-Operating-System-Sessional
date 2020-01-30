#!/bin/bash
echo "Zip File : $1"
echo "CSV File : $2"

#roll number extraction
cut -d , -f 1 "$2" | cut -d "\"" -f 2 | sed 's/^[ \t]*//;s/[ \t]*$//' > Roster.txt

#submissionfolder
mkdir AllSubmission
cp $1 ./AllSubmission
pushd ./AllSubmission

unzip $1
rm $1
ls > files.txt
sed -e "/files/g" -i files.txt
sed -e "/^\s*$/d" -i files.txt
ls | cut -d _ -f 5 | cut -d . -f 1 > temp.txt
sed -e "/files/g" -i temp.txt
sed -e "/^\s*$/d" -i temp.txt
sed -e "/temp/g" -i temp.txt
sed -e "/^\s*$/d" -i temp.txt
find temp.txt -maxdepth 1 -exec mv {} .. \;  #move file to parent dir
popd
cp $2 ./AllSubmission
cp Roster.txt ./AllSubmission

filename="Roster.txt"
while read -r line; do
    name="$line"
    echo "Name read from file - $name"
    flag=`grep "$name" temp.txt|wc -l`
    if [ $flag -ne "0" ]; then
    echo "found"
    else
	echo "$name" >> Absentee_list.txt 
    fi
done < "$filename"

mv Absentee_list.txt ./AllSubmission
pushd ./AllSubmission
#filename="Roster.txt"


#absentee list creation
mkdir Others
a=0;
filename="files.txt"
while read -r line; do
    name="$line"
    echo "unzipping file - $name"
    mkdir tempdir
    #mv "$line" ./tempdir 
    #pushd ./tempdir
    unzip "$line" -d tempdir
    rm "$line"
    #popd  #back to AllSubmission folder
    a=`expr $a + 1`
    filecount=$(ls tempdir|wc -l)
    if [ $filecount -eq 1 ]; then
	folder=$(ls tempdir)
    	c=$(echo $line|cut -d _ -f 5|cut -d . -f 1)
	if [[ $folder = $c ]]; then   #fully matched
	mark="$c	10"
	pushd ./tempdir
	find "$c" -maxdepth 1 -exec mv {} .. \;  #move file to parent dir
	popd
	#mv tempdir/"$c" AllSubmission
	echo "voila "
	elif [[ $folder =~ $c ]];then	#substring matched
	mark="$c	5"
	pushd ./tempdir
	mv "$folder" "$c"
	find "$c" -maxdepth 1 -exec mv {} .. \;  #move file to parent dir
	popd
	#mv tempdir/"$folder" tempdir/"$c"
	#mv tempdir/"$c" AllSubmission
	echo "HALF voila | $folder | $c "
	else	#nothing matched
	name="$(cut -d '_' -f1 <<< "$line")"
	row=`grep -i "$name" "$2"`
	count=`grep -i "$name" "$2"|wc -l`
	roll=$(cut -d ',' -f1 <<< "$row" | cut -d '"' -f2 | sed 's/^[ \t]*//;s/[ \t]*$//') #if sid found by name
	check=`grep -i "$c" Roster.txt` #if sid found

	echo "folder : $folder |roll $roll |check $check "
	if [[ $check != "" ]];then
	roll="$check"
	name="$check"
	sed -e "/$roll/g" -i Absentee_list.txt
	sed -e "/^\s*$/d" -i Absentee_list.txt
	pushd ./tempdir
	mv "$folder" "$name"
	find "$name" -maxdepth 1 -exec mv {} .. \;  #move file to parent dir
	popd
	elif [[ $roll != "" ]];then
	name="$roll"
	sed -e "/$roll/g" -i Absentee_list.txt
	sed -e "/^\s*$/d" -i Absentee_list.txt
	pushd ./tempdir
	mv "$folder" "$name"
	find "$name" -maxdepth 1 -exec mv {} .. \;  #move file to parent dir
	popd
	else
	roll="$unidentified"
	pushd ./tempdir
	mv "$folder" "$name"
	find "$name" -maxdepth 1 -exec mv {} .. \;  #move file to parent dir
	popd
	mv "$name" Others
	fi
	echo "$roll"
	mark="$roll	0"
	fi
    else   #multiple directory
	name="$(cut -d '_' -f1 <<< "$line")"
	row=`grep -i "$name" "$2"`
	count=`grep -i "$name" "$2"|wc -l`
	roll=$(cut -d ',' -f1 <<< "$row" | cut -d '"' -f2 | sed 's/^[ \t]*//;s/[ \t]*$//') #if sid found by name
	check=`grep -i "$c" Roster.txt` #if sid found

	echo "folder : $folder |roll $roll |check $check "
	if [[ $check != "" ]];then
	roll="$check"
	name="$check"
	sed -e "/$roll/g" -i Absentee_list.txt
	sed -e "/^\s*$/d" -i Absentee_list.txt
	cp -r tempdir "$name"
	elif [[ $roll != "" ]];then
	name="$roll"
	sed -e "/$roll/g" -i Absentee_list.txt
	sed -e "/^\s*$/d" -i Absentee_list.txt
	cp -r tempdir "$name"
	else
	roll="$unidentified"
	cp -r tempdir Others/"$name"
	fi
	echo "$roll"
	mark="$roll	0"
	#pushd ./tempdir
	#cp tempdir "$name"
	#find "$name" -maxdepth 1 -exec mv {} .. \;  #move file to parent dir
	#popd
	#mv "$name" Others
    fi
    #popd  #back to AllSubmission folder
    echo "$mark" >> Marks.txt
    rm -rf tempdir

done < "$filename"
echo $a #total entry
rm $2
#sub folder creation and unzipping
#find Marks.txt -maxdepth 1 -exec mv {} .. \;  #move file to parent dir

filename="Absentee_list.txt"
while read -r line; do
    name="$line"
    echo "$name	0" >> Marks.txt 
done < "$filename"
sort -o Absentee_list.txt Absentee_list.txt
sort -o Marks.txt Marks.txt
popd
#cp Marks.txt ./AllSubmission
mv AllSubmission Output
