#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>

enum class ErrorCode { NONE, KEYWORD_INVALID, ARGS_NB_INVALID, DEC_VALUE_TOO_HIGH, HEX_VALUE_TOO_HIGH, HEX_ADDRESS_TOO_HIGH, REG_CONCAT_INVALID, REG_INVALID,
					   BYTE_SPEC_INVALID, INVALID_HEX_VALUE, INVALID_HEX_ADDRESS, REG_CONCAT_MISSING_BRACKET, REG_CONCAT_TOO_FEW, REG_CONCAT_TOO_MANY, MISSING_QUOTE };
enum class ArgCategory { REGISTER, DEC_VALUE, HEX_VALUE, HEX_ADDRESS, REG_CONCAT, LABEL_VARIABLE, VARIABLE_SPECIFIC_BYTE, STRING };
enum class Register { A, B, C, D, I, J, X, Y };
enum class SpecificByte { MSB, MB, LSB };

typedef struct
{
	ErrorCode code;
	unsigned int line;
} ErrorMsg;

typedef struct
{
	ArgCategory category;
	ErrorCode validity;
	Register reg;
	uint8_t value;
	uint32_t address;
	Register contactReg[3];
	std::string labelVariableName;
	SpecificByte byte;
	std::string str;
} Arg;

class Keyword
{
	public:
		Keyword(std::string w, unsigned int minArgs); // m_maxArgs = 0 means that there is no limit to the number of arguments for this keyword
		Keyword(std::string w, unsigned int minArgs, unsigned int maxArgs);
	
		std::string getStr();
		unsigned int getMinArgs();
		unsigned int getMaxArgs();

	private:
		std::string m_w;
		unsigned int m_minArgs;
		unsigned int m_maxArgs;
};

class Parser
{
	public:
		Parser(std::vector<std::string>* sourceFileStr);

		void removeComments();
		void removeEmptyLines();
		void fixIndentation();
		ErrorMsg parseFile();

	private:
		bool checkFirstWord(std::string w);
		std::vector<std::string> extractArguments(std::string line);
		bool checkArgsNb(std::string w, std::vector<std::string> args);
		Arg checkArgAbsValidity(std::string argStr);
		bool is_digits(const std::string& str);
		bool is_hexDigits(const std::string& str);
		bool is_register(const std::string& str);
		Register getReg(const std::string& str);

		std::map<unsigned int, unsigned int> m_lineCorrespondance;
		std::vector<std::string>* m_sourceFileStr;
		std::vector<Keyword> m_keywordsMap;
};
