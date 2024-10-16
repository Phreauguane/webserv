#include "webserv.h"
#include <sys/wait.h>

namespace Cgi
{
	std::string executePHP(const std::string& scriptPath, const std::string& query);
}
