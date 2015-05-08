CC = gcc
CFLAGS = -g -Wall -Wno-unused-label -Wno-parentheses -I include/
LIBS = 
ONAME = sim

OBJS = shell.o \
	   proc_t.o \
	   of_t.o \
	   mino_t.o \
	   mnt_t.o \
	   k_t.o \
	   vsh_t.o \
	   string_funcs.o \
	   func_addresses.o \
	   test_func.o \
   	   show_help.o \
	   print_minos.o \
	   print_vfs_mounts.o \
	   show_imap.o \
	   show_bmap.o \
	   print_ino.o \
	   print_procs.o \
	   switch.o \
 	   ls.o \
	   cd.o \
	   creat.o \
	   mkdir.o \
	   pwd.o \
	   link.o \
	   unlink.o \
	   rmdir.o\
	   symblink.o\
	   stat.o \
	   touch.o \
	   chmod.o \
	   chgrp.o \
	   chown.o \
	   open.o \
	   close.o \
	   lseek.o \
	   pfd.o \
	   read.o \
	   cat.o \
	   mv.o \
	   write.o \
	   cp.o


shell: ${OBJS}
	${CC} -o ${ONAME} ${CFLAGS} ${OBJS} ${LIBS}

clean:
	rm -f *.o
	rm -f l0/*.o
	rm -f l1/*.o
	rm -f l2/*.o
	rm -f ${ONAME}
	
#-------------------------------------------------

shell.o: shell.c shell.h
	${CC} ${CFLAGS} -c shell.c

proc_t.o: proc_t.c proc_t.h
	${CC} ${CFLAGS} -c proc_t.c

of_t.o: of_t.c of_t.h
	${CC} ${CFLAGS} -c of_t.c

mino_t.o: mino_t.c mino_t.h
	${CC} ${CFLAGS} -c mino_t.c

mnt_t.o: mnt_t.c mnt_t.h
	${CC} ${CFLAGS} -c mnt_t.c

k_t.o: k_t.c k_t.h
	${CC} ${CFLAGS} -c k_t.c

vsh_t.o: vsh_t.c vsh_t.h
	${CC} ${CFLAGS} -c vsh_t.c

string_funcs.o: string_funcs.c string_funcs.h
	${CC} ${CFLAGS} -c string_funcs.c

func_addresses.o: func_addresses.c func_addresses.h
	${CC} ${CFLAGS} -c func_addresses.c

show_help.o: l0/show_help.c l0/show_help.h
	${CC} ${CFLAGS} -c l0/show_help.c

print_minos.o: l0/print_minos.c l0/print_minos.h
	${CC} ${CFLAGS} -c l0/print_minos.c

print_vfs_mounts.o: l0/print_vfs_mounts.c l0/print_vfs_mounts.h
	${CC} ${CFLAGS} -c l0/print_vfs_mounts.c

test_func.o: l0/test_func.c l0/test_func.h
	${CC} ${CFLAGS} -c l0/test_func.c

show_imap.o: l0/show_imap.c l0/show_imap.h
	${CC} ${CFLAGS} -c l0/show_imap.c

show_bmap.o: l0/show_bmap.c l0/show_bmap.h
	${CC} ${CFLAGS} -c l0/show_bmap.c

print_ino.o: l0/print_ino.c l0/print_ino.h
	${CC} ${CFLAGS} -c l0/print_ino.c

print_procs.o: l0/print_procs.c l0/print_procs.h
	${CC} ${CFLAGS} -c l0/print_procs.c

switch.o: l0/switch.c l0/switch.h
	${CC} ${CFLAGS} -c l0/switch.c

ls.o: l1/ls.c l1/ls.h
	${CC} ${CFLAGS} -c l1/ls.c

cd.o: l1/cd.c l1/cd.h
	${CC} ${CFLAGS} -c l1/cd.c

mkdir.o: l1/mkdir.c l1/mkdir.h
	${CC} ${CFLAGS} -c l1/mkdir.c

creat.o: l1/creat.c l1/creat.h
	${CC} ${CFLAGS} -c l1/creat.c

pwd.o: l1/pwd.c l1/pwd.h
	${CC} ${CFLAGS} -c l1/pwd.c

link.o: l1/link.c l1/link.h
	${CC} ${CFLAGS} -c l1/link.c

unlink.o: l1/unlink.c l1/unlink.h
	${CC} ${CFLAGS} -c l1/unlink.c

rmdir.o: l1/rmdir.c l1/rmdir.h
	${CC} ${CFLAGS} -c l1/rmdir.c

symblink.o: l1/symblink.c l1/symblink.h
	${CC} ${CFLAGS} -c l1/symblink.c

stat.o: l1/stat.c l1/stat.h
	${CC} ${CFLAGS} -c l1/stat.c

touch.o: l1/touch.c l1/touch.h
	${CC} ${CFLAGS} -c l1/touch.c

chmod.o: l1/chmod.c l1/chmod.h	
	${CC} ${CFLAGS} -c l1/chmod.c

chgrp.o: l1/chgrp.c l1/chgrp.h
	${CC} ${CFLAGS} -c l1/chgrp.c

chown.o: l1/chown.c l1/chown.h
	${CC} ${CFLAGS} -c l1/chown.c

open.o: l2/open.c l2/open.h
	${CC} ${CFLAGS} -c l2/open.c

close.o: l2/close.c l2/close.h
	${CC} ${CFLAGS} -c l2/close.c

lseek.o: l2/lseek.c l2/lseek.h
	${CC} ${CFLAGS} -c l2/lseek.c

pfd.o: l2/pfd.c l2/pfd.h
	${CC} ${CFLAGS} -c l2/pfd.c

read.o: l2/read.c l2/read.h
	${CC} ${CFLAGS} -c l2/read.c

cat.o: l2/cat.c l2/cat.h
	${CC} ${CFLAGS} -c l2/cat.c

mv.o: l2/mv.c l2/mv.h
	${CC} ${CFLAGS} -c l2/mv.c

write.o: l2/write.c l2/write.h
	${CC} ${CFLAGS} -c l2/write.c

cp.o: l2/cp.c l2/cp.h
	${CC} ${CFLAGS} -c l2/cp.c
