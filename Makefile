# $Id: Makefile,v 1.1 2011-12-02 01:27:49-08 - - $
# Michael Baptist - mbaptist@ucsc.edu
# Written: May 8, 2014	

GCC       = gcc -g -O0 -Wall -Wextra -std=gnu99 -pthread
CSOURCE   = http_parser.c http_response.c utils.c proxy_server.c 
CHEADER   = http_parser.h http_response.h utils.h

all: proxy_server

proxy_server: proxy_server.o utils.o http_parser.o http_response.o log.o siteblock.o
	${GCC} -o proxy_server proxy_server.o utils.o http_parser.o http_response.o log.o siteblock.o

proxy_server.o: proxy_server.c
	${GCC} -c proxy_server.c

utils.o: utils.c
	${GCC} -c utils.c

http_parser.o: http_parser.c
	${GCC} -c http_parser.c

http_response.o: http_response.c
	${GCC} -c http_response.c

log.o: log.c
	${GCC} -c log.c

siteblock.o: siteblock.c
	${GCC} -c siteblock.c

clean:
	rm *.o

wipe: clean
	rm proxy_server
	rm proxy.log

#testcli:
#	./clitests.sh

#need Doxygen installed for this.
docs:
	./docgen.sh

#scpcats:
#	scp client mbaptist@unix.ic.ucsc.edu:~/private/CE156/lab1
#	scp client mbaptist@unix2.ic.ucsc.edu:~/private/CE156/lab1

