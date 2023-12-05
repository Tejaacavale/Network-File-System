nm: naming_server.c Tries.c
	gcc naming_server.c Caching.c path_conversion.c Tries.c LRU.c -o nm

ss: storage_server.c
	gcc storage_server.c path_conversion.c -o ss

cl: client.c
	gcc client.c path_conversion.c -o cl
all: nm ss cl