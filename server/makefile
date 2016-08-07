
.PHONY:all
all:clean server client
server:
	g++ *.c -o server -lpthread
client:
	gcc ../client/client.c -o client
clean:
	rm -f server client
