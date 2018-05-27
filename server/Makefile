
default : upgmake

upgmake :
	$(MAKE) -f objs/upgmake/Makefile

uploaded :
	$(MAKE) -f objs/uploaded/Makefile

uservice :
	$(MAKE) -f objs/uservice/Makefile

install : upgmake
	test -d '/usr/local/upgrade/sbin/' || mkdir -p '/usr/local/upgrade/sbin/'
	cp bin/upgmake /usr/local/upgrade/sbin/upgmake
   
clean :
	rm -f objs/*/*.o
	rm -f bin/upgmake
	rm -f bin/uploaded
    
 
