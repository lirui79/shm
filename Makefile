
all:ishm_read ishm_write libishm.so demo-read demo-write

ishm_read:ishm.c ipcshm.c ishm_read.c
	gcc -g ishm.c ipcshm.c ishm_read.c -I./ -o ishm_read

ishm_write:ishm.c ipcshm.c ishm_write.c
	gcc -g ishm.c ipcshm.c ishm_write.c -I./ -o ishm_write

libishm.so:ishm.c ipcshm.c
	gcc -g ishm.c ipcshm.c -I./ -fPIC -shared  -o libishm.so

demo-read:ishm_read.c
	gcc -g ishm_read.c -L. -lishm -I./ -o demo-read

demo-write:ishm_write.c
	gcc -g ishm_write.c -L. -lishm  -I./ -o demo-write


clean:
	rm *_write *_read demo-* *.so -rf