data_structure:libcounter liblist libhash

libcounter:lock.h lock.c counter.h counter.c spinlock.h spinlock.c mutex.h mutex.c twophase.h twophase.c
	gcc spinlock.c mutex.c twophase.c lock.c counter.c -fPIC -shared -o libcounter.so
liblist:lock.h lock.c list.h list.c spinlock.h spinlock.c mutex.h mutex.c twophase.h twophase.c
	gcc spinlock.c mutex.c twophase.c lock.c list.c -fPIC -shared -o liblist.so
libhash:lock.h lock.c list.c list.h hash.h hash.c spinlock.h spinlock.c mutex.h mutex.c twophase.h twophase.c
	gcc  spinlock.c mutex.c twophase.c lock.c list.c hash.c -fPIC -shared -o libhash.so


