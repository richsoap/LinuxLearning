#! /bin/bash

val=`expr 2 + 2`
echo "The sum is $val"
# ` is at the left and top corner

val=`expr 2 \* 3`
echo "The result is $val"
# "\*"

if [ 4 == 6 ]
then
echo "eq"
else
	echo "neq"
fi

