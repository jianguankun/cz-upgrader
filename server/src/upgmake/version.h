#ifndef _VERSION_H_
#define _VERSION_H_

#include <vector>
#include <string>

//获取指定目录中最大的版本号
int get_max_version(char* version,unsigned int countch,const char* dir);

//产生一个新版本号
int generic_version(char* maxver,unsigned int countch,const char* dir);

//版本比较
int version_cmp(const char* v1,const char* v2);

//检查版本是否合法
bool check_version(const char* v);

//分析版本
std::vector<std::string> ver_split(const char* v);

//合成版本号
std::string combine_ver(std::vector<std::string>& vt);

#endif
