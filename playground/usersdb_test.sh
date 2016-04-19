gcc -std=c99 -Wall -O2 -DDMR_USERDB_NOT_IN_FLASH -I ../applet/src/ ../applet/src/usersdb.c usersdb_test.c -o usersdb_test && ./usersdb_test
