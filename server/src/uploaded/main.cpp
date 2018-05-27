#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>
#include <fstream>
#include <iostream>

#include "../common/daemon.h"
#include "UploadPassRequest.h"

void error_log(const char* msg)
{
   using namespace std;
   using namespace boost;
   static ofstream error;
   if(!error.is_open())
   {
      error.open("/tmp/errlog", ios_base::out | ios_base::app);
      error.imbue(locale(error.getloc(), new posix_time::time_facet()));
   }
   error << '[' << posix_time::second_clock::local_time() << "] " << msg << endl;
}

int main(int argc, char *argv[])
{
    using namespace std;

    if (argc > 1)
    {
        bool daemon = false;
        int operation;
        
        //通过while循环获取  
        while((operation = getopt(argc, argv, "d")) != -1)  
        {  
            switch(operation)  
            {
            case 'd': 
                {
                    daemon = true;
                }
            }
        }

        if(daemon)
        {
            init_daemon();
            signal(SIGTERM, sigterm_handler);
            signal(SIGCHLD, SIG_IGN);
        }
    }

    struct sockaddr_in sockaddr;
    memset(&sockaddr,0,sizeof(sockaddr));
    
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_port = htons(5000);

    int listenfd = socket(AF_INET,SOCK_STREAM,0);

    // 设置套接字选项避免地址使用错误
	int on = 1;
	if((setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
	{
        //cerr << "setsockopt failed" << endl;
        return 0;
	}

    if(bind(listenfd,(struct sockaddr*) &sockaddr,sizeof(sockaddr)) != 0)
    {
        //cerr << "errno = " << errno << ","<< strerror(errno) << endl;
        return 0;
    }

    FILE* fp = NULL;
    if((fp = fopen("uploaded.pid","w")) != NULL)
    {
        char buf[32] = {0};
        sprintf(buf,"%d\n",getpid());
        fwrite((void*)buf,strlen(buf),1,fp);
        fclose(fp);
    }

    listen(listenfd,5);

    try
    {
        Fastcgipp::Manager<CUploadPassRequest> fcgi(listenfd);
        fcgi.handler();
    }
    catch(std::exception& e)
    {
        error_log(e.what());
    }
}
