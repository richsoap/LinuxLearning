#! /bin/bash

var="http://www.google.com/123/456.com"

#0 "#" function
echo ${var#*//}
#Delete all things before the first "//"
#Output is "www.google.com/123/456.com"

#1 "##" function
echo ${var##*/}
#Delete all thigs before the last "/"
#Output is "456.com"

#2 "%" function
echo ${var%/*}
#Delete all things after the last "/"
#Output is "http://www.google.com/123"

#3 "%%" function
echo ${var%%/*}
#Delete all things after the first "/"
#Output is "http:"

#4 ":a:b" means a substring from a and its length is b
echo ${var:1:5}
#Output is "ttp://"

#5 ":a" means a substring from a to the end of the original String
echo ${var:15}
#Output is "le.com/123/456.com"

#6 ":0-a:b" means a substring from a(right) and its length is b
echo ${var:0-5:3}
#Output is "6.3"

#7 ":0-a" means a substring from a(right) to the end of the original String
echo ${var:0-5}
#Output is "6.com"
