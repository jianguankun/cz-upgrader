#ifndef _FASTCGIPP_ECHO_H_
#define _FASTCGIPP_ECHO_H_

#include <fastcgi++/request.hpp>

class CUploadPassRequest: public Fastcgipp::Request<wchar_t>
{
     bool response();
};

#endif //_FASTCGIPP_ECHO_H_