#include "webserv.h"
#include "Request.h"
#include "Response.h"

namespace Cgi
{
	std::string executePHP(const std::string& scriptPath, const std::string& query);
	bool executeCGI(const Request&, Response&);
}
