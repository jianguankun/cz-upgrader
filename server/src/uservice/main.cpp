#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <iostream>
#include <errno.h> 

#include "log4cpp/Category.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
#include "log4cpp/PatternLayout.hh"
#include "log4cpp/RollingFileAppender.hh"

log4cpp::Category* g_Logger = NULL;

using namespace std;

#include "../common/daemon.h"

#define MAX_EVENTS 500

//设置socket连接为非阻塞模式
bool setnonblocking(int sockfd) 
{
    int opts;
    opts = fcntl(sockfd, F_GETFL);
    if(opts < 0) 
    {
        g_Logger->error("fcntl(F_GETFL)");
        return false;
    }
    opts = (opts | O_NONBLOCK);
    if(fcntl(sockfd, F_SETFL, opts) < 0)
    {
        g_Logger->error("fcntl(F_SETFL)");
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    using namespace std;

    log4cpp::RollingFileAppender* fileAppender = new log4cpp::RollingFileAppender("Logger","/var/log/uservice/uservice",5 * 1024 * 1024,1);
    log4cpp::PatternLayout* pLayout = new log4cpp::PatternLayout();
    pLayout->setConversionPattern("%d{%m/%d %H:%M:%S.%l} %p : %m%n");
    fileAppender->setLayout(pLayout);
    g_Logger = &log4cpp::Category::getRoot().getInstance("Logger");
    g_Logger->addAppender(fileAppender);
    g_Logger->setPriority(log4cpp::Priority::DEBUG);

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
    sockaddr.sin_port = htons(6600);

    int listenfd = socket(AF_INET,SOCK_STREAM,0);

    if(!setnonblocking(listenfd))
    {
        return 0;
    }

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
    if((fp = fopen("uservice.pid","w")) != NULL)
    {
        char buf[32] = {0};
        sprintf(buf,"%d\n",getpid());
        fwrite((void*)buf,strlen(buf),1,fp);
        fclose(fp);
    }

    listen(listenfd,5);

    int epollfd = epoll_create(1);
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);

    epoll_event events[MAX_EVENTS] = {0};

    while(true)
    {
        int nRet = epoll_wait(epollfd,events,MAX_EVENTS,-1);
        if(nRet == -1)
        {
            g_Logger->error("epoll_wait failed.");
            break;
        }

        for(int i = 0; i < nRet; ++i)
        {
            int socketfd = events[i].data.fd;
            if(socketfd == listenfd)
            {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(struct sockaddr_in);
                memset(&client_addr,0,(size_t)client_len);
                int clientfd = accept(listenfd,(struct sockaddr*)&client_addr,&client_len);
                setnonblocking(clientfd);
                g_Logger->info("accept from %s:%d. sockfd = %d",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port),clientfd);
                g_Logger->info("EAGAIN = %d",EAGAIN);
                ev.events = EPOLLIN;
                ev.data.fd = clientfd;
                epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&ev);
            }
            else
            {
                if(events[i].events & EPOLLIN)
                {
                    char buf[1024];
                    ssize_t n = 0;
                    ssize_t nread = 0;
			        while((nread = read(socketfd,buf + n,sizeof(buf) - 1)) > 0)
                    {
                        n += nread;
                    }

                    g_Logger->info("client:%d read:%d lastread:%d errno:%d",socketfd,n,nread,errno);

                    if(nread == 0) //client:close
                    {
                        g_Logger->info("client:%d close",socketfd);
                        epoll_ctl(epollfd,EPOLL_CTL_DEL,socketfd,NULL);
                        close(socketfd);
                    }
                    else
                    {
                        epoll_event e;
                        e.events = EPOLLIN | EPOLLOUT;
                        e.data.fd = socketfd;
                        epoll_ctl(epollfd,EPOLL_CTL_MOD,socketfd,&e);
                    }
                }
                else if(events[i].events & EPOLLOUT)
                {
                    //g_Logger->info("EPOLLOUT.sockfd = %d",socketfd);
                    write(socketfd,"Hello",5);
                }
            }
        }
    }

}
