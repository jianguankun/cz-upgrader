
#include "UploadPassRequest.h"

bool CUploadPassRequest::response()
{
	using namespace Fastcgipp;
	wchar_t cookieString[] = { '<', '"', 0x0440, 0x0443, 0x0441, 0x0441, 0x043a, 0x0438, 0x0439, '"', '>', ';', 0x0000 };

	out << "Set-Cookie: echoCookie=" << encoding(URL) << cookieString << encoding(NONE) << "; path=/\n";
	out << "Content-Type: text/html; charset=utf-8\r\n\r\n";

	out << "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8' />";
	out << "<title>fastcgi++: Echo in UTF-8</title></head><body>";

	out << "<h1>Environment Parameters</h1>";
	out << "<p><b>FastCGI Version:</b> " << Protocol::version << "<br />";
	out << "<b>fastcgi++ Version:</b> " << version << "<br />";
	out << "<b>Hostname:</b> " << encoding(HTML) << environment().host << encoding(NONE) << "<br />";
	out << "<b>User Agent:</b> " << encoding(HTML) << environment().userAgent << encoding(NONE) << "<br />";
	out << "<b>Accepted Content Types:</b> " << encoding(HTML) << environment().acceptContentTypes << encoding(NONE) << "<br />";
	out << "<b>Accepted Languages:</b> " << encoding(HTML) << environment().acceptLanguages << encoding(NONE) << "<br />";
	out << "<b>Accepted Characters Sets:</b> " << encoding(HTML) << environment().acceptCharsets << encoding(NONE) << "<br />";
	out << "<b>Referer:</b> " << encoding(HTML) << environment().referer << encoding(NONE) << "<br />";
	out << "<b>Content Type:</b> " << encoding(HTML) << environment().contentType << encoding(NONE) << "<br />";
	out << "<b>Root:</b> " << encoding(HTML) << environment().root << encoding(NONE) << "<br />";
	out << "<b>Script Name:</b> " << encoding(HTML) << environment().scriptName << encoding(NONE) << "<br />";
	out << "<b>Request URI:</b> " << encoding(HTML) << environment().requestUri << encoding(NONE) << "<br />";
	out << "<b>Request Method:</b> " << encoding(HTML) << environment().requestMethod << encoding(NONE) << "<br />";
	out << "<b>Content Length:</b> " << encoding(HTML) << environment().contentLength << encoding(NONE) << "<br />";
	out << "<b>Keep Alive Time:</b> " << encoding(HTML) << environment().keepAlive << encoding(NONE) << "<br />";
	out << "<b>Server Address:</b> " << encoding(HTML) << environment().serverAddress << encoding(NONE) << "<br />";
	out << "<b>Server Port:</b> " << encoding(HTML) << environment().serverPort << encoding(NONE) << "<br />";
	out << "<b>Client Address:</b> " << encoding(HTML) << environment().remoteAddress << encoding(NONE) << "<br />";
	out << "<b>Client Port:</b> " << encoding(HTML) << environment().remotePort << encoding(NONE) << "<br />";
	out << "<b>If Modified Since:</b> " << encoding(HTML) << environment().ifModifiedSince << encoding(NONE) << "</p>";

	out << "<h1>Path Data</h1>";
	if(environment().pathInfo.size())
	{
		std::wstring preTab;
		for(Http::Environment<wchar_t>::PathInfo::const_iterator it=environment().pathInfo.begin(); it!=environment().pathInfo.end(); ++it)
		{
			out << preTab << encoding(HTML) << *it << encoding(NONE) << "<br />";
			preTab += L"&nbsp;&nbsp;&nbsp;";
		}
	}
	else
		out << "<p>No Path Info</p>";

	out << "<h1>GET Data</h1>";
	if(environment().gets.size())
		for(Http::Environment<wchar_t>::Gets::const_iterator it=environment().gets.begin(); it!=environment().gets.end(); ++it)
			out << "<b>" << encoding(HTML) << it->first << encoding(NONE) << ":</b> " << encoding(HTML) << it->second << encoding(NONE) << "<br />";
	else
		out << "<p>No GET data</p>";

	out << "<h1>Cookie Data</h1>";
	if(environment().cookies.size())
		for(Http::Environment<wchar_t>::Cookies::const_iterator it=environment().cookies.begin(); it!=environment().cookies.end(); ++it)
			out << "<b>" << encoding(HTML) << it->first << encoding(NONE) << ":</b> " << encoding(HTML) << it->second << encoding(NONE) << "<br />";
	else
		out << "<p>No Cookie data</p>";

	out << "<h1>POST Data</h1>";
	if(environment().posts.size())
	{
		for(Http::Environment<wchar_t>::Posts::const_iterator it=environment().posts.begin(); it!=environment().posts.end(); ++it)
		{
			out << "<h2>" << encoding(HTML) << it->first << encoding(NONE) << "</h2>";
			if(it->second.type==Http::Post<wchar_t>::form)
			{
				out << "<p><b>Type:</b> form data<br />";
				out << "<b>Value:</b> " << encoding(HTML) << it->second.value << encoding(NONE) << "</p>";
			}

			else
			{
				out << "<p><b>Type:</b> file<br />";
				out << "<b>Filename:</b> " << encoding(HTML) << it->second.filename << encoding(NONE) << "<br />";
				out << "<b>Content Type:</b> " << encoding(HTML) << it->second.contentType << encoding(NONE) << "<br />";
				out << "<b>Size:</b> " << it->second.size() << "<br />";
				out << "<b>Data:</b></p><pre>";
				//out.dump(it->second.data.get(), it->second.size);
				out << "</pre>";
			}
		}
	}
	else
		out << "<p>No POST data</p>";

	out << "</body></html>";

	return true;

}