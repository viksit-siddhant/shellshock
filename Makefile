shellshock: binaries
	gcc -pthread -o shellshock shellshock.c

dev: binaries_dev
	gcc -pthread -o shellshock shellshock.c

binaries_dev: bin
	gcc -o bin/ls src/ls.c
	gcc -o bin/cat src/cat.c
	gcc -o bin/rm src/rm.c
	gcc -o bin/mkdir src/mkdir.c
	gcc -o bin/date src/date.c

binaries: binaries_dev
	touch binaries
	echo "DUMMY FILE" > binaries

bin:
	mkdir bin