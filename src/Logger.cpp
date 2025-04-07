#include "Logger.h"
#include "Utils.h"
#include <vector>


namespace Logger
{
	std::fstream log_file;
	log_level min_lvl = TEXT;
	std::vector<std::string> errors;
	
	void log(const std::vector<char>& message, const log_level level) {
		std::string str = "";

		for (size_t i = 0; i < message.size(); i++) {
			if (message[i] == '\0')
				str += "\\0";
			else 
				str += message[i];
		}
		
		Logger::log(str, level);
	}

	void log(const std::string& message, const log_level level)
	{
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
			errors.push_back(message);
			bold_clr = BOLD_RED;
			clr = RED;
			lvl = "ERROR";
			break;
		default:
			break;
		}
		std::stringstream consoleOutput, logFileOutput;
		std::string timestr = Utils::getCurrentTime();
		
		consoleOutput << DEF << "🮤" << timestr << "🮥 " << bold_clr << " [" << lvl << "]" << clr << " 🭺 " << msg << DEF << std::endl;
		logFileOutput << "🮤" << timestr << "🮥  [" << lvl << "]" << " 🭺 " << msg << std::endl;
		std::cout << (consoleOutput.str());
		if (log_file.is_open())
			log_file.write((logFileOutput.str()).c_str(), (logFileOutput.str()).size());
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

	void logErrors() {
		if (errors.size() == 0) return ;
		std::string bold_clr = BOLD_RED, clr = RED, lvl = "ERROR";
		std::stringstream consoleOutput, logFileOutput;
		std::string timestr = Utils::getCurrentTime();

		consoleOutput << DEF << "🮤" << timestr << "🮥 " << BOLD_RED << "    [ ERROR SUMMARY ]    " << DEF << std::endl;
		logFileOutput << "🮤" << timestr << "🮥    [ ERROR SUMMARY ]    " << std::endl;

		for (size_t i = 0; i < errors.size(); ++i)
		{
			std::string msg = errors[i];
			timestr = Utils::getCurrentTime();

			consoleOutput << DEF << "🮤" << timestr << "🮥 " << bold_clr << " [" << lvl << "]" << clr << " 🭺 " << msg << DEF << std::endl;
			logFileOutput << "🮤" << timestr << "🮥  [" << lvl << "]" << " 🭺 " << msg << std::endl;
		}

		std::cout << (consoleOutput.str());
		if (log_file.is_open())
			log_file.write((logFileOutput.str()).c_str(), (logFileOutput.str()).size());
	}

	void synthaxError(const std::vector<std::string>& strs, size_t word, size_t id, const std::string& filename, const std::string& message) {
		std::stringstream ss;
		std::string waves = "\t    ";

		std::string lineStr = strs[0];
		for (size_t i = 1; i < strs.size(); i++) {
			lineStr += " " + strs[i];
			if (i <= word)
				waves += " ";
		}

		ss << filename << ": error: " << message;
		Logger::log(ss.str(), ERROR);
		Logger::log("\t--> " + lineStr, ERROR);

		for (size_t i = 0; i < std::min(word, strs.size()); i++) {
			for (size_t j = 0; j < strs[i].size(); j++) {
				waves += " ";
			}
		}
		if (word > strs.size())
			waves += " ";
		for (size_t i = 0; i < ((word >= strs.size()) ? 5 : std::max(strs[word].size(), id + 1)); i++) {
			if (i == id)
				waves += "^";
			else
				waves += "~";
		}
		Logger::log(waves, ERROR);
	}
}
