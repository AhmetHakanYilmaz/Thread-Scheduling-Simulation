all: systemsim

systemsim: systemsim.c
	gcc -Wall -o systemsim systemsim.c -lpthread -lm
	
clean: 
	rm -fr *~ systemsim
