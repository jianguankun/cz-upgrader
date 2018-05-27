complier = g++
make = $(complier) -g -Wall

SC = src/common/
SU = src/uploaded/
OC = objs/common/
OU = objs/uploaded/

objs = 	$(OU)main.o \
		$(OU)UploadPassRequest.o \
		$(OC)daemon.o 

uploaded : $(objs)
	$(make) -o bin/uploaded $(objs) \
	-lfastcgipp -lboost_system-mt -lboost_thread-mt

$(OU)main.o : $(SU)main.cpp
	$(make) -c $(SU)main.cpp -o $(OU)main.o
    
$(OU)UploadPassRequest.o : $(SU)UploadPassRequest.cpp $(SU)UploadPassRequest.h
	$(make) -c $(SU)UploadPassRequest.cpp -o $(OU)UploadPassRequest.o
    
$(OC)daemon.o : $(SC)daemon.cpp $(SC)daemon.h
	$(make) -c $(SC)daemon.cpp -o $(OC)daemon.o
    
 
