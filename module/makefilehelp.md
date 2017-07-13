# Makefile
## internet
http://blog.csdn.net/ruglcc/article/details/7814546/
## rules of makefile
target...: prerequisites ...
		command
		...
		...

target is the target file, which can be Object File, binary file even a lable.
prerequisites is the request file, which can be used to generate the target file.
command is what "make" should do(a lot of shell commands).
## an example
If there are 3 headfiles and 8 sourcefiles, our Makefile will be like that.
```makefile
edit: main.o kbd.o command.o display.0 /
	insert.o search.o files.o
	cc -o edit main.o kdb.o command.o display.o/
		insert.o search.o files.o utils.o
main.o: main.c defs.h
	cc -c main.c
kbd.o: kbd.c defs.h command.h
	cc -c kbd.c
command.o: command.c defs.h command.h
	cc -c command.c
display.o: display.c defs.h buffer.h
	cc -c display.c
insert.o: insert.c defs.h buffer.h
	cc -c insert.c
search.o: search.c defs.h buffer.h 
	cc-c search.c
files.o: files.c defs.h buffer.h command.h
	cc -c files.c
utils.o: utils.c defs.h
	cc -c utils.c
clean:
	rm edit main.o kdb.o command.o display.o/
	insert.o search.o files.o utils.o
```	

## variable
We can use
```makefile
objects = main.o ldb.o command.o display.o insert.o search.o files.o utils.o
```
to identify a variable.

We can use it via
```makefile
edit: $(objects)
	cc -o edit $(objects)
```

## no need for too many details
"Make" can know our c files via o files and get the right cc-c.Therefore 
```makefile
main.o: defs.h
```
equals to
```makefile
main.o: main.c defs.h
	cc -c main.c
```
## .PHONY
It means "clean" is a fake target file.
```makefile
.PHONY: clean
clean:
	rm edit $(objects)
```
Besides, "make clean" is the only way to run "clean".
.PHONY also can be the first file.
```makefile
all:pro1 pro2 pro3
.PHONY:all

pro1:...
	...
pro2:...
	...
pro3:...
	...

```
If we "make", we will get pro1, pro2 and pro3, not only pro1 or something else.
## "-"
```makefile
.PHONY: clean
clean:
	-rm edit $(objects)
```
"-" means that there may be something wrong while removing files, but leave it alone, go to the next step.
## include
```makefile
include<filename>
```
where filename may be a shell file in this system//include path,etc
## MAKEFILES
Please don't use this environment variable, because it may cause some weird things.
## Search file
### VPATH
```makefile
VPATH=src:../headers
```
If "make" can't find headers, it will search it from dirs in VPATH.

They are src and ../headers here, which is seprated via ':'.
### vpath
```makefile
vpath <pattern> <directories> //Search <directories> for files matching <pattern> 
vpath<pattern> // clean search directories for files matching <pattern>
vpath // clean all directories
```
example
```makefile
vpath %.c foo:blish
```
It means search c files in foo and blish in order.
## multiple targets "$@"
```makefile
bigoutput littleoutput:text.g
	generate text.g -$(subst output ,,$@) > $@
```
equals to
```make file
bigoutput:text.g
	generate text.g -big > bigoutput
littleoutput:text.g
	generate text.g -little > littleoutput
```
where "$" in "-$(subst output,,$@)" means run a Makefile function named "subst" the following is parameters.(subst equals subString in Java)

"$@" stands for a set of targets and pick them out one by one.




