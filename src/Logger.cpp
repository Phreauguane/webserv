/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 18:36:07 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/19 19:35:41 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.h"
#include "Utils.h"

namespace Logger
{
	std::fstream log_file;
	log_level min_lvl = DEBUG;
	
	void log(const std::string& message, const log_level level)
	{
		// "●◆◈◉"
		if (level < min_lvl)
			return ;
		std::string bold_clr, clr, lvl;
		switch (level)
		{
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
			lvl = "SUCCESS";
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
		std::stringstream consoleOutput;
		std::string timestr = Utils::getCurrentTime();
		
		consoleOutput << DEF << timestr << bold_clr << " [" << lvl << "]" << clr << " - " << message << DEF << std::endl;
		std::cout << consoleOutput.str();
		if (log_file.is_open())
			log_file.write(consoleOutput.str().c_str(), consoleOutput.str().size());
	}
	
	void setLogFile(const std::string& filename)
	{
		log_file.open(filename.c_str());
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