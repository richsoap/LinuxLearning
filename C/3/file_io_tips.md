##显示表示标准流
/dev/stdin=/dev/fd/0
/dev/stdout=/dev/fd/1
/dev/stderr=/dev/fd/2
###eg
filter file2 | cat file1 /dev/fd/0 file3 



