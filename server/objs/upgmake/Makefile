complier = g++
make = $(complier) -g -Wall

SC = src/common/
SU = src/upgmake/
OC = objs/common/
OU = objs/upgmake/

objs = 	$(OU)upgmake.o \
		$(OU)version.o \
		$(OC)sha1.o 

upgmake : $(objs) libs/libtinyxml.a
	$(make) -o bin/upgmake $(objs) libs/libtinyxml.a

$(OU)upgmake.o : $(SU)upgmake.cpp $(SU)version.h $(SC)sha1.h $(SC)tinyxml.h $(SC)tinystr.h 
	$(make) -c $(SU)upgmake.cpp -o $(OU)upgmake.o
    
$(OU)version.o : $(SU)version.cpp $(SU)version.h
	$(make) -c $(SU)version.cpp -o $(OU)version.o
    
$(OC)sha1.o : $(SC)sha1.cpp $(SC)sha1.h
	$(make) -c $(SC)sha1.cpp -o $(OC)sha1.o
