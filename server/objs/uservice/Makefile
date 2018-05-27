complier = g++
make = $(complier) -g -Wall

SC = src/common/
SU = src/uservice/
OC = objs/common/
OU = objs/uservice/

objs = 	$(OU)main.o \
		$(OC)daemon.o 

uservice : $(objs)
	$(make) -o bin/uservice $(objs) -llog4cpp -lpthread

$(OU)main.o : $(SU)main.cpp
	$(make) -c $(SU)main.cpp -o $(OU)main.o
    
$(OC)daemon.o : $(SC)daemon.cpp $(SC)daemon.h
	$(make) -c $(SC)daemon.cpp -o $(OC)daemon.o
    
 
