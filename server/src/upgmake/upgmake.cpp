#include <stdio.h>
#include <iomanip>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <vector>

#include "../common/tinyxml.h"
#include "../common/tinystr.h"
#include "../common/sha1.h"
#include "version.h"

using namespace std;

#define MAX_PATH 512


//遍历一个目录(升级文件)
void traval_upgrade(const char* currdir,const char* innerdir,TiXmlElement* pXmlElement);

//遍历一个目录(需要删除的文件)
void traval_delete(const char* currdir,const char* innerdir,TiXmlElement* pXmlElement);

//获取文件的sha1码
bool get_file_sha1(unsigned int buflen,char* sha1buf,const char* szFile);


void show_help()
{
    cout << "    -g -- guide      The guide file name of this upgrade, also for the guide file name." << endl;
    cout << "    -h               Show help." << endl;
    cout << "    -s -- silent     Silent mode. Ignore all queries and deal with them in default." << endl;
    cout << "                     You can use the value 'auto' to gerenate a new guider file name automatic." << endl;
    cout << "    -v -- verbose    Verbosely list files processed." << endl;
    cout << "    -w -- news       Specify an information source file(what's new) and insert an update message into the file. " << endl;
    cout << endl;
}

bool verbose = false;
bool silent = false;

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        show_help();
        return 0;
    }

    char version[64] = {0}; 

    int operation;
    //通过while循环获取  
    while((operation = getopt(argc, argv, "g:hsv")) != -1)  
    {  
        switch(operation)  
        {
        case 'g': 
            {
                if (strlen((const char*)optarg) >= sizeof(version))
                {
                    cerr << "Version string is too long!" << endl;
                    return 0;
                }
                if (strlen((const char*)optarg) <= 3)
                {
                    cerr << "Version string is too short!" << endl;
                    return 0;
                }
                strcpy(version,(const char*)optarg);
                break;
            }
		case 'h': 
			{
				show_help();
				return 0;
			}
		case 'v': 
			{
				verbose = true;
				break;
			}
		case 's': 
			{
				silent = true;
				break;
			}
        case '?': //如果设置了未定义的参数，则返回这边的错误信息  
            {
                opterr = 0;
                break;
            }  
        case ':': //缺少选项  
            {
                opterr = 0;
                break;  
            }
        }  
    }

    char targetpath[MAX_PATH] = {0};
    if (optind == argc)
    {
        getcwd(targetpath, MAX_PATH);
    }
    else 
    {
        strcpy(targetpath,argv[optind]);
    }

    if((access(targetpath,F_OK)) == -1)   
    {   
        cerr << "Directory \"" << targetpath << "\" is not exist!" << endl;
        return 0;
    }

    //如果version的值被指定为auto，则检查本目录中所有的guider文件，通过xml文件名计算下一个版本号，
    //新的版本号被赋回version
    if(strcmp(version,"auto") == 0)
    {
        if(generic_version(version,sizeof(version),targetpath) != 1)
        {
            return 0;
        }
    }

    if(!check_version(version))
    {
        cerr << "Illegal version \"" << version << "\"" << endl;
        return 0;
    }

	//上一个版本的xml文件
	char guidefile_name_last[MAX_PATH] = {0};
	get_max_version(guidefile_name_last,MAX_PATH,targetpath);
	char guidefile_path_last[MAX_PATH] = {0};
	if(strlen(guidefile_name_last) > 0)
	{
		strcat(guidefile_name_last,".xml");
		sprintf(guidefile_path_last,"%s/%s",targetpath,guidefile_name_last);
	}

	//本次版本的xml文件
	char guidefile_name[MAX_PATH] = {0};
	sprintf(guidefile_name,"%s.xml",version);

	char guidefile_path[MAX_PATH] = {0};
	sprintf(guidefile_path,"%s/%s",targetpath,guidefile_name);

    struct stat fs;
    if (stat(guidefile_path, &fs) != -1)
    {
        cerr << "The guide file [" << guidefile_name << "] existed in this directoy, please specify other version." << endl;
        return 0; 
    }

    //XML文档对象
    TiXmlDocument* pXMLDoc = new TiXmlDocument();

    TiXmlDeclaration* pdecl = new TiXmlDeclaration("1.0", "UTF-8", "yes"); 
    pXMLDoc->LinkEndChild(pdecl); // <?xml version="1.0" encoding="UTF-8"?> 

    char szPath[MAX_PATH] = {0};

    TiXmlElement xElement0("IDTask");
    //xElement0.SetAttribute("version",version);

    TiXmlElement xElement1("WhatsNew");
    TiXmlText XmlText("");
    xElement1.InsertEndChild(XmlText);
    xElement0.InsertEndChild(xElement1);

    sprintf(szPath,"%s/improve",targetpath);
    if((access(szPath,F_OK)) == -1)   
    {   
        cerr << "Directory \"improve\" is not exist!" << endl;
        return 0;
    }
    TiXmlElement xElement2("UpgradeItems");
    traval_upgrade(szPath,"",&xElement2);
    xElement0.InsertEndChild(xElement2);

	sprintf(szPath,"%s/delete",targetpath);
	if((access(szPath,F_OK)) != -1)   
	{
		TiXmlElement xElement3("DeleteItems");
		traval_delete(szPath,"",&xElement3);
		xElement0.InsertEndChild(xElement3);
	}

    pXMLDoc->InsertEndChild(xElement0);

    pXMLDoc->SaveFile(guidefile_path);

    delete pXMLDoc;

	bool bShowResult = true;

	if (strlen(guidefile_path_last) > 0)
	{
		char sz_sha_old[64] = {0};
		char sz_sha_new[64] = {0};
		get_file_sha1(sizeof(sz_sha_old),sz_sha_old,guidefile_path_last);
		get_file_sha1(sizeof(sz_sha_new),sz_sha_new,guidefile_path);
		if(strcmp(sz_sha_old,sz_sha_new) == 0)
		{
			if (!silent)
			{
				cout << "Warning! File " << guidefile_name_last << "(last version) and " << guidefile_name << "(new version) are same, "<< endl;
				cout << "Do you want to giveup version file? [y/n]";
				char cQuery = cin.get();
				if (cQuery == 'Y' || cQuery == 'y')
				{
					remove(guidefile_path);
					bShowResult = false;
				}
			}
			else
			{
				cout << "There was no sign of directory \"improve\" updating." << endl;
				remove(guidefile_path);
				bShowResult = false;
			}
		}
	}

	if (bShowResult)
	{
		cout << "Make upgrade successfully!" << endl;
		cout << "Guide File : " << guidefile_name << endl;
// 		cout << "┌───────────────────────────────────────────────────────┐" << endl;
// 		cout << "│  guide file : " << setiosflags(ios::left) << setw(40) << guidefile_name << "│"<< endl;
// 		cout << "└───────────────────────────────────────────────────────┘" << endl;
	}

    return 0;
}


bool get_file_sha1(unsigned int buflen,char* sha1buf,const char* szFile)
{
    if (NULL == sha1buf || buflen < 64)
    {
        return false;
    }

    sha1buf[0] = '\0';

    unsigned int szMDTemp[5] = {0};

    const unsigned int READ_BLOCK_SIZE = 2048;
    unsigned char fileBuf[READ_BLOCK_SIZE];

    FILE* fp = fopen(szFile,"rb");
    if(NULL == fp)
    {
        return false;
    }

    size_t nReadlen = 0;

    SHA1 sha;
    sha.Reset();
    do
    {
        nReadlen = fread(fileBuf,1,READ_BLOCK_SIZE,fp);
        if(nReadlen != 0)
        {
            sha.Input(fileBuf,(unsigned int)nReadlen);
        }
    } 
    while (nReadlen == READ_BLOCK_SIZE);

    sha.Result(szMDTemp);

    char* p = sha1buf;
    for (int i = 0; i < 5; ++i)
    {
        sprintf(p,"%.8x",szMDTemp[i]);
        p += 8;
    }

    fclose(fp);

    return true;
}

void traval_upgrade(const char* currdir,const char* innerdir,TiXmlElement* pXmlElement)//遍历一个目录
{
    DIR* dfd;
    char item[MAX_PATH];
    struct dirent* _dp;
    if ((dfd = opendir(currdir)) == NULL)
    {
        cerr << "can't open " << currdir << endl;
        cerr << strerror(errno) << endl;
        return;
    }
    else
    {
        while ((_dp = readdir(dfd)) != NULL)
        {
            if (strncmp(_dp->d_name, ".", 1) == 0)
            {
                //跳过当前目录和上一层目录以及隐藏文件
                continue; 
            }
            if (strlen(currdir) + strlen(_dp->d_name) + 1 >= MAX_PATH)
            {
                cerr << "name " << currdir << _dp->d_name << "too long" << endl;
                continue; 
            }

            memset(item,0,sizeof(item));
            sprintf(item,"%s/%s", currdir,_dp->d_name);

            struct stat fs;
            if (stat(item, &fs) == -1)
            {
                cerr << "cannot access the file : " << item;
                continue; 
            }

            if ((fs.st_mode & S_IFMT) == S_IFDIR)
            {
                char dir_temp[MAX_PATH];
                sprintf(dir_temp,"%s%s/", innerdir,_dp->d_name);
                TiXmlElement xPos("Directory");
                xPos.SetAttribute("path",dir_temp);
                pXmlElement->InsertEndChild(xPos);
                traval_upgrade(item,dir_temp,pXmlElement);
            }
            else
            {
                char szSHA1[64] = {0};
                get_file_sha1(sizeof(szSHA1),szSHA1,item);
                if (verbose)
                {
                    cout<<"[" << szSHA1 << "] "<<innerdir<<_dp->d_name<<endl;
                }

                char dir_temp[MAX_PATH] = {0};
                sprintf(dir_temp,"%s%s", innerdir,_dp->d_name);

                TiXmlElement xPos("File");

                xPos.SetAttribute("checksum",szSHA1);
                xPos.SetAttribute("path",dir_temp);

                xPos.SetAttribute("size",fs.st_size);

                pXmlElement->InsertEndChild(xPos);
            }
        }
    }
    closedir(dfd);
}

//遍历一个目录(需要删除的文件)
void traval_delete(const char* currdir,const char* innerdir,TiXmlElement* pXmlElement)
{
    DIR* dfd;
    char item[MAX_PATH];
    struct dirent* _dp;
    if ((dfd = opendir(currdir)) == NULL)
    {
        cerr << "can't open " << currdir << endl;
        cerr << strerror(errno) << endl;
        return;
    }
    else
    {
        while ((_dp = readdir(dfd)) != NULL)
        {
            if (strncmp(_dp->d_name, ".", 1) == 0)
            {
                //跳过当前目录和上一层目录以及隐藏文件
                continue; 
            }
            if (strlen(currdir) + strlen(_dp->d_name) + 1 >= MAX_PATH)
            {
                cerr << "name " << currdir << _dp->d_name << "too long" << endl;
                continue; 
            }

            memset(item,0,sizeof(item));
            sprintf(item,"%s/%s", currdir,_dp->d_name);

            struct stat fs;
            if (stat(item, &fs) == -1)
            {
                cerr << "cannot access the file : " << item;
                continue; 
            }

            if ((fs.st_mode & S_IFMT) == S_IFDIR)
            {
                char dir_temp[MAX_PATH];
                sprintf(dir_temp,"%s%s/", innerdir,_dp->d_name);
                traval_delete(item,dir_temp,pXmlElement);
            }
            else
            {
                char dir_temp[MAX_PATH] = {0};
                sprintf(dir_temp,"%s%s", innerdir,_dp->d_name);

                TiXmlElement xPos("File");
                xPos.SetAttribute("path",dir_temp);

                pXmlElement->InsertEndChild(xPos);
            }
        }
    }
    closedir(dfd);
}