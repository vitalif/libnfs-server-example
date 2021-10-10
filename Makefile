nfs-server: nfs-server.c nfs-service.c nfs-service.h
	gcc -g -I/usr/include/nfsc nfs-server.c nfs-service.c -o nfs-server -lnfs -levent
