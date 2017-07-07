#! /bin/bash

array_name=(
zqp
zh
bth
)

echo -e  "I have ${#array_name[@]} classmates\nThey are"
for name in ${array_name[@]}
do
	echo "$name"
done
