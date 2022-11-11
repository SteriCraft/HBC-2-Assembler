#include "parser.hpp"

Keyword::Keyword(std::string w, unsigned int minArgs)
{
	m_w = w;
	m_minArgs = minArgs;
	m_maxArgs = (unsigned int)~0; // Inverts all bits to get highest possible value
}

Keyword::Keyword(std::string w, unsigned int minArgs, unsigned int maxArgs)
{
	m_w = w;
	m_minArgs = minArgs;
	m_maxArgs = maxArgs;
}

std::string Keyword::getStr()
{
	return m_w;
}

unsigned int Keyword::getMinArgs()
{
	return m_minArgs;
}

unsigned int Keyword::getMaxArgs()
{
	return m_maxArgs;
}

Parser::Parser(std::vector<std::string>* sourceFileStr)
{
	m_sourceFileStr = sourceFileStr;

	m_keywordsMap.push_back(Keyword(".define", 2, 2)); // Only 2 arguments : variable name and data
	m_keywordsMap.push_back(Keyword(".data", 2)); // Minimum 2 arguments : variable name and one data
	m_keywordsMap.push_back(Keyword(":", 0, 1)); // With of without an address

	// Instructions
	m_keywordsMap.push_back(Keyword("nop", 0, 0));
	m_keywordsMap.push_back(Keyword("adc", 2, 2));
	m_keywordsMap.push_back(Keyword("add", 2, 2));
	m_keywordsMap.push_back(Keyword("and", 2, 2));
	m_keywordsMap.push_back(Keyword("cal", 1, 1));
	m_keywordsMap.push_back(Keyword("clc", 0, 0));
	m_keywordsMap.push_back(Keyword("cle", 0, 0));
	m_keywordsMap.push_back(Keyword("cli", 0, 0));
	m_keywordsMap.push_back(Keyword("cln", 0, 0));
	m_keywordsMap.push_back(Keyword("cls", 0, 0));
	m_keywordsMap.push_back(Keyword("clz", 0, 0));
	m_keywordsMap.push_back(Keyword("clf", 0, 0));
	m_keywordsMap.push_back(Keyword("cmp", 2, 2));
	m_keywordsMap.push_back(Keyword("dec", 1, 1));
	m_keywordsMap.push_back(Keyword("hlt", 0, 0));
	m_keywordsMap.push_back(Keyword("in",  2, 2));
	m_keywordsMap.push_back(Keyword("out", 2, 2));
	m_keywordsMap.push_back(Keyword("inc", 1, 1));
	m_keywordsMap.push_back(Keyword("int", 1, 1));
	m_keywordsMap.push_back(Keyword("irt", 0, 0));
	m_keywordsMap.push_back(Keyword("jmc", 1, 1));
	m_keywordsMap.push_back(Keyword("jme", 1, 1));
	m_keywordsMap.push_back(Keyword("jmf", 1, 1));
	m_keywordsMap.push_back(Keyword("jmk", 1, 1));
	m_keywordsMap.push_back(Keyword("jmp", 1, 1));
	m_keywordsMap.push_back(Keyword("jms", 1, 1));
	m_keywordsMap.push_back(Keyword("jmz", 1, 1));
	m_keywordsMap.push_back(Keyword("jmn", 1, 1));
	m_keywordsMap.push_back(Keyword("str", 2, 2));
	m_keywordsMap.push_back(Keyword("lod", 2, 2));
	m_keywordsMap.push_back(Keyword("mov", 2, 2));
	m_keywordsMap.push_back(Keyword("not", 1, 1));
	m_keywordsMap.push_back(Keyword("or",  2, 2));
	m_keywordsMap.push_back(Keyword("pop", 1, 1));
	m_keywordsMap.push_back(Keyword("psh", 1, 1));
	m_keywordsMap.push_back(Keyword("ret", 0, 0));
	m_keywordsMap.push_back(Keyword("shl", 1, 1));
	m_keywordsMap.push_back(Keyword("asr", 1, 1));
	m_keywordsMap.push_back(Keyword("shr", 1, 1));
	m_keywordsMap.push_back(Keyword("stc", 0, 0));
	m_keywordsMap.push_back(Keyword("sti", 0, 0));
	m_keywordsMap.push_back(Keyword("stn", 0, 0));
	m_keywordsMap.push_back(Keyword("stf", 0, 0));
	m_keywordsMap.push_back(Keyword("sts", 0, 0));
	m_keywordsMap.push_back(Keyword("ste", 0, 0));
	m_keywordsMap.push_back(Keyword("stz", 0, 0));
	m_keywordsMap.push_back(Keyword("sub", 2, 2));
	m_keywordsMap.push_back(Keyword("xor", 2, 2));
}

void Parser::removeComments()
{
	size_t commaPos;

	for (unsigned int i(0); i < m_sourceFileStr->size(); i++)
	{
		commaPos = m_sourceFileStr->at(i).find(';');

		if (commaPos != std::string::npos) // Comment found
		{
			m_sourceFileStr->at(i) = m_sourceFileStr->at(i).substr(0, commaPos);

			while (m_sourceFileStr->at(i).back() == ' ') // Deleting spaces before the comment symbol
			{
				m_sourceFileStr->at(i) = m_sourceFileStr->at(i).substr(0, m_sourceFileStr->at(i).size() - 1);

				if (m_sourceFileStr->at(i).size() == 0)
					break;
			}
		}
	}
}

void Parser::removeEmptyLines()
{
	unsigned int sourceFileLineIndex(0);

	for (unsigned int i(0); i < m_sourceFileStr->size(); i++)
	{
		if (m_sourceFileStr->at(i).size() == 0)
		{
			m_sourceFileStr->erase(m_sourceFileStr->begin() + i);

			i--;
		}
		else
		{
			m_lineCorrespondance[i] = sourceFileLineIndex;
		}

		sourceFileLineIndex++;
	}
}

void Parser::fixIndentation()
{
	size_t commaPos;

	for (unsigned int i(0); i < m_sourceFileStr->size(); i++) // Spaces after commas
	{
		commaPos = m_sourceFileStr->at(i).find(',');

		while (commaPos != std::string::npos)
		{
			while (m_sourceFileStr->at(i)[commaPos + 1] == ' ')
			{
				m_sourceFileStr->at(i) = m_sourceFileStr->at(i).substr(0, commaPos + 1) + m_sourceFileStr->at(i).substr(commaPos + 2);
			}

			commaPos = m_sourceFileStr->at(i).find(',', commaPos + 1);
		}
	}
}

ErrorMsg Parser::parseFile()
{
	size_t pos;
	std::string firstWord;
	std::vector<std::string> argsStr;
	bool argsExists(false);

	// Default error message (none)
	ErrorMsg err;
	err.code = ErrorCode::NONE;
	err.line = 0;

	for (unsigned int i(0); i < m_sourceFileStr->size(); i++)
	{
		argsStr.clear();
		pos = m_sourceFileStr->at(i).find(" ");

		// Extracting the first word
		if (m_sourceFileStr->at(i)[0] == ':') // Label found
		{
			firstWord = ":";

			if (pos != std::string::npos) // Argument(s) found
				argsExists = true;
			else
				argsExists = false;
		}
		else if (pos != std::string::npos) // Argument(s) found
		{
			firstWord = m_sourceFileStr->at(i).substr(0, pos);
			argsExists = true;
		}
		else // No argument
		{
			firstWord = m_sourceFileStr->at(i);
			argsExists = false;
		}

		if (checkFirstWord(firstWord)) // First word is valid
		{
			std::cout << m_sourceFileStr->at(i) << std::endl;
			if (argsExists) // Check arguments
			{
				argsStr = extractArguments(m_sourceFileStr->at(i));

				for (unsigned int j(0); j < argsStr.size(); j++)
				{
					std::cout << "|" << argsStr[j] << "| ";
				}
				std::cout << std::endl << std::endl;

				if (checkArgsNb(firstWord, argsStr)) // Check arguments nb
				{
					std::vector<Arg> args;
					for (unsigned int j(0); j < argsStr.size(); j++)
					{
						args.push_back(checkArgAbsValidity(argsStr[j])); // Checks arguments absolute validity

						if (args.back().validity != ErrorCode::NONE)
						{
							err.code = args.back().validity;
							err.line = m_lineCorrespondance[i] + 1;
							return err;
						}
					}

					// Getting here means no error was found in terms of arguments absolute validity
					
					// TODO
					// Check for keyword relative validity

					// DO BINARY STUFF FROM HERE (probably)
				}
				else
				{
					err.code = ErrorCode::ARGS_NB_INVALID;
					err.line = m_lineCorrespondance[i] + 1;
					return err;
				}
			}
			else
			{
				// DO BINARY STUFF FROM HERE (with no arguments)
			}
		}
		else
		{
			err.code = ErrorCode::KEYWORD_INVALID;
			err.line = m_lineCorrespondance[i] + 1;
			return err;
		}
	}
}

// PRIVATE
bool Parser::checkFirstWord(std::string w) // Returns true if first word is a keyword
{
	for (unsigned int i(0); i < m_keywordsMap.size(); i++)
	{
		if (m_keywordsMap.at(i).getStr() == w) // Keyword exist
		{
			return true;
		}
	}

	return false;
}

std::vector<std::string> Parser::extractArguments(std::string line)
{
	size_t spacePos(line.find(" ")), commaPos, nextPos; // nextPos = closest symbol between comma and space
	std::vector<std::string> args;

	// Remove keyword
	line = line.substr(spacePos + 1);
	spacePos = line.find(" ");
	commaPos = line.find(",");
	nextPos = (spacePos > commaPos) ? (commaPos) : (spacePos);

	while (nextPos != std::string::npos)
	{
		if (line[0] == '\"')
		{
			args.push_back(line.substr(0, line.find("\"", 1) + 1));

			if (line.size() > args.back().size())
				line = line.substr(line.find("\"", 1) + 2);
			else
				line = ""; // Safety
		}
		else
		{
			args.push_back(line.substr(0, nextPos));
			line = line.substr(nextPos + 1);
		}

		spacePos = line.find(" ");
		commaPos = line.find(",");
		nextPos = ((spacePos > commaPos) ? commaPos : spacePos); // Using the closest one
	}

	if (line != "") // Safety
		args.push_back(line);

	return args;
}

bool Parser::checkArgsNb(std::string w, std::vector<std::string> args)
{
	for (unsigned int i(0); i < m_keywordsMap.size(); i++)
	{
		if (w == m_keywordsMap[i].getStr())
		{
			if (args.size() >= m_keywordsMap[i].getMinArgs() && args.size() <= m_keywordsMap[i].getMaxArgs())
			{
				return true;
			}
		}

	}

	return false;
}

Arg Parser::checkArgAbsValidity(std::string argStr) // Analyses and categorizes arguments, then checks for validity
{
	Arg arg;
	arg.validity = ErrorCode::NONE;

	if (argStr.size() == 1) // Register
	{
		arg.category = ArgCategory::REGISTER;

		// Analyze
		if (argStr == "a")
		{
			arg.reg = Register::A;
		}
		else if (argStr == "b")
		{
			arg.reg = Register::B;
		}
		else if (argStr == "c")
		{
			arg.reg = Register::C;
		}
		else if (argStr == "d")
		{
			arg.reg = Register::D;
		}
		else if (argStr == "i")
		{
			arg.reg = Register::I;
		}
		else if (argStr == "j")
		{
			arg.reg = Register::J;
		}
		else if (argStr == "x")
		{
			arg.reg = Register::X;
		}
		else if (argStr == "y")
		{
			arg.reg = Register::Y;
		}
		else
		{
			arg.validity = ErrorCode::REG_INVALID;
		}

		return arg;
	}
	else if (is_digits(argStr)) // Decimal value
	{
		arg.category = ArgCategory::DEC_VALUE;
		int value = std::stoi(argStr, nullptr, 10);

		if (value < 256)
		{
			arg.value = value;
		}
		else
		{
			arg.validity = ErrorCode::DEC_VALUE_TOO_HIGH;
		}

		return arg;
	}
	else if (argStr.substr(0, 2) == "0x") // Hexadecimal value
	{
		arg.category = ArgCategory::HEX_VALUE;

		if (is_hexDigits(argStr.substr(2)))
		{
			int value = std::stoi(argStr, nullptr, 16);
			
			if (value < 256)
			{
				arg.value = value;
			}
			else
			{
				arg.validity = ErrorCode::HEX_VALUE_TOO_HIGH;
			}
		}
		else
		{
			arg.validity = ErrorCode::INVALID_HEX_VALUE;
		}

		return arg;
	}
	else if (argStr.substr(0, 3) == "$0x") // Address
	{
		arg.category = ArgCategory::HEX_ADDRESS;

		if (is_hexDigits(argStr.substr(3)))
		{
			int value = std::stoi(argStr.substr(3), 0, 16);

			if (value <= 0xFFFFFF)
			{
				arg.address = value;
			}
			else
			{
				arg.validity = ErrorCode::HEX_ADDRESS_TOO_HIGH;
			}
		}
		else
		{
			arg.validity = ErrorCode::INVALID_HEX_ADDRESS;
		}

		return arg;
	}
	else if (argStr[0] == '[') // Concatenated registers
	{
		arg.category = ArgCategory::REG_CONCAT;

		if (argStr.back() != ']')
		{
			arg.validity = ErrorCode::REG_CONCAT_MISSING_BRACKET;
		}
		else if (argStr.size() < 5)
		{
			arg.validity = ErrorCode::REG_CONCAT_TOO_FEW;
		}
		else if (argStr.size() > 5)
		{
			arg.validity = ErrorCode::REG_CONCAT_TOO_MANY;
		}
		else if (!is_register(argStr.substr(1, 1)) || !is_register(argStr.substr(2, 1)) || !is_register(argStr.substr(3, 1)))
		{
			arg.validity = ErrorCode::REG_CONCAT_INVALID;
		}
		else
		{
			arg.contactReg[0] = getReg(argStr.substr(1, 1));
			arg.contactReg[1] = getReg(argStr.substr(2, 1));
			arg.contactReg[2] = getReg(argStr.substr(3, 1));
		}

		return arg;
	}
	else if (argStr[0] == '\"')
	{
		arg.category = ArgCategory::STRING;

		if (argStr.back() == '\"')
		{
			arg.str = argStr.substr(1, argStr.size() - 2);
		}
		else
		{
			arg.validity = ErrorCode::MISSING_QUOTE;
		}

		return arg;
	}
	else if (argStr.find(".") != std::string::npos) // Variable specific byte
	{
		arg.category = ArgCategory::VARIABLE_SPECIFIC_BYTE;

		arg.labelVariableName = argStr.substr(0, argStr.find("."));

		if (argStr.substr(argStr.find(".") + 1) == "msb")
		{
			arg.byte = SpecificByte::MSB;
		}
		else if (argStr.substr(argStr.find(".") + 1) == "mb")
		{
			arg.byte = SpecificByte::MB;
		}
		else if (argStr.substr(argStr.find(".") + 1) == "lsb")
		{
			arg.byte = SpecificByte::LSB;
		}
		else
		{
			arg.validity = ErrorCode::BYTE_SPEC_INVALID;
		}

		return arg;
	}
	else // Variable name or label name
	{
		arg.category = ArgCategory::LABEL_VARIABLE;
		arg.labelVariableName = argStr;

		return arg;
	}
}

bool Parser::is_digits(const std::string& str)
{
	return str.find_first_not_of("0123456789") == std::string::npos;
}

bool Parser::is_hexDigits(const std::string& str)
{
	return str.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos;
}

bool Parser::is_register(const std::string& str)
{
	return str.find_first_not_of("abcdijxy") == std::string::npos;
}

Register Parser::getReg(const std::string& str) // WARNING : No check
{
	if (str == "a")
	{
		return Register::A;
	}
	else if (str == "b")
	{
		return Register::B;
	}
	else if (str == "c")
	{
		return Register::C;
	}
	else if (str == "d")
	{
		return Register::D;
	}
	else if (str == "i")
	{
		return Register::I;
	}
	else if (str == "j")
	{
		return Register::J;
	}
	else if (str == "x")
	{
		return Register::X;
	}

	return Register::Y;
}
