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
vpath <pattern><directories>
vpath<pattern>
vpath
```

