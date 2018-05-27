#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include "version.h"

using namespace std;

#define MAX_PATH 512


bool check_version(const char* v)
{
	while(*v != 0)
	{
		if(!isdigit(*v) && *v != '.')
		{
			return false;
		}
		v++;
	}
	return true;
}

//获取指定目录中最大的版本号
int get_max_version(char* maxver,unsigned int countch,const char* dir)
{
	DIR* dfd;
	struct dirent* _dp;
	if ((dfd = opendir(dir)) == NULL)
	{
		return 0;
	}

	char maxver_temp[MAX_PATH] = {0};

	while ((_dp = readdir(dfd)) != NULL)
	{
		if (strncmp(_dp->d_name, ".", 1) == 0)
		{
			//跳过当前目录和上一层目录以及隐藏文件
			continue; 
		}

		char filepath[MAX_PATH] = {0};
		sprintf(filepath,"%s/%s", dir,_dp->d_name);

		struct stat fs;
		if (stat(filepath, &fs) != -1)
		{
			if ((fs.st_mode & S_IFMT) != S_IFDIR)
			{
				char name[MAX_PATH] = {0};
				strcpy(name,_dp->d_name);

				char* ext = strrchr(name,'.');
				if (ext == 0)
				{
					continue;
				}
				*(ext++) = 0;

				if(strcmp(ext,"xml") == 0 && check_version(name))
				{
					//cout << name << endl;
					if(version_cmp(maxver_temp,name) < 0)
					{
						strcpy(maxver_temp,name);
					}
				}
			}
		}
	}

	strcpy(maxver,maxver_temp);

	closedir(dfd);

	return 1;
}

int generic_version(char* version,unsigned int countch,const char* dir)
{
	char maxver[MAX_PATH] = {0};
	get_max_version(maxver,MAX_PATH,dir);

	if(strlen(maxver) > 0)
	{
		vector<string> vt = ver_split(maxver);
		const char* serial = vt[vt.size() - 1].c_str();

		char* newserial = new char[strlen(serial) + 2];
		sprintf(newserial,"%d",atoi((char*)serial) + 1);

		int diffcount = strlen(serial) - strlen(newserial);
		if ( diffcount > 0)
		{
			string temp = "";
			for (int i = 0; i < diffcount; ++i)
			{
				temp += "0";
			}
			temp += newserial;
			vt[vt.size() - 1] = temp;
		}
		else
		{
			vt[vt.size() - 1] = newserial;
		}

		delete[] newserial;
		newserial = NULL;

		string newversion = combine_ver(vt);
		if (newversion.length() >= countch)
		{
			cerr << "The new version string [" << newversion << "] is too long." << endl;
			return 0;
		}
		strcpy(version,newversion.c_str());
	}
	else
	{
		cerr << "nothing valid guider file in this current." << endl;
		return 0;
	}

	return 1;
}


int version_cmp(const char* v1,const char* v2)
{
	vector<string> vect1 = ver_split(v1);
	vector<string> vect2 = ver_split(v2);

	int maxlen = min(vect1.size(),vect2.size());

	int i = 0;
	for (; i < maxlen; i++)
	{
		if(atoi(vect1[i].c_str()) > atoi(vect2[i].c_str()))
		{
			return 1;
		}
		else if(atoi(vect1[i].c_str()) < atoi(vect2[i].c_str()))
		{
			return -1;
		}
	}

	if(vect1.size() > vect2.size())
	{
		return 1;
	}
	else if(vect1.size() < vect2.size())
	{
		return -1;
	}

	return 0;
}

std::vector<std::string> ver_split(const char* v)
{
	vector<string> vect;
	char* p = NULL;
	const char* delim = ".";

	char tempstr[32] = {0};
	strcpy(tempstr,v);

	p = strtok(tempstr,delim);
	while(p != NULL)
	{
		vect.push_back(p);
		p = strtok(NULL,delim);
	}

	return vect;
}


//合成版本号
std::string combine_ver(std::vector<std::string>& vt)
{
	if (vt.size() == 0)
	{
		return "";
	}

	string retval = "";
	unsigned int i = 0;
	for (; i < vt.size() - 1; ++i)
	{
		retval += vt[i] + ".";
	}

	retval += vt[i];
	return retval;
}