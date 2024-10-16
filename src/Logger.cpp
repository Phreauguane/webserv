#include "Logger.h"
#include "Utils.h"

namespace Logger
{
	std::fstream log_file;
	log_level min_lvl = TEXT;
	
	void log(const std::string& message, const log_level level)
	{
		// "●◆◈◉"
		if (level < min_lvl)
			return ;
		std::string bold_clr, clr, lvl;
		std::string msg = message;
		switch (level)
		{
		case TEXT:
			bold_clr = BOLD_WHITE;
			clr = WHITE;
			lvl = "DEBUG";
			msg = "\n" + message;
			break;
		case DEBUG:
			bold_clr = BOLD_BLUE;
			clr = BLUE;
			lvl = "DEBUG";
			break;
		case INFO:
			bold_clr = BOLD_CYAN;
			clr = CYAN;
			lvl = "INFO";
			break;
		case SUCCESS:
			bold_clr = BOLD_GREEN;
			clr = GREEN;
			lvl = "INFO";
			break;
		case WARNING:
			bold_clr = BOLD_YELLOW;
			clr = YELLOW;
			lvl = "WARNING";
			break;
		case ERROR:
			bold_clr = BOLD_RED;
			clr = RED;
			lvl = "ERROR";
			break;
		default:
			break;
		}
		std::stringstream consoleOutput, logFileOutput;
		std::string timestr = Utils::getCurrentTime();
		
		consoleOutput << DEF << "[" << timestr << "] " << bold_clr << " [" << lvl << "]" << clr << " - " << msg << DEF << std::endl;
		logFileOutput << "[" << timestr << "]  [" << lvl << "]" << " - " << msg << std::endl;
		std::cout << consoleOutput.str();
		if (log_file.is_open())
			log_file.write(logFileOutput.str().c_str(), logFileOutput.str().size());
	}
	
	void setLogFile(const std::string& filename)
	{
		log_file.open(filename.c_str(), std::ios::out | std::ios::app);
	}
	
	void saveLog()
	{
		log_file.close();
	}
	
	void setMinLogLevel(log_level lvl)
	{
		min_lvl = lvl;
	}
}
