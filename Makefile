
all:ishm_read ishm_write libishm.so demo-read demo-write

ishm_read:ishm_read.c ishm.c
	gcc -g ishm.c ishm_read.c -I./ -o ishm_read

ishm_write:ishm_write.c ishm.c
	gcc -g ishm.c ishm_write.c -I./ -o ishm_write

libishm.so:ishm.c
	gcc -g ishm.c -I./ -fPIC -shared  -o libishm.so

demo-read:ishm_read.c
	gcc -g ishm_read.c -L. -lishm -I./ -o demo-read

demo-write:ishm_write.c
	gcc -g ishm_write.c -L. -lishm  -I./ -o demo-write


clean:
	rm ishm_* demo-* *.so -rf