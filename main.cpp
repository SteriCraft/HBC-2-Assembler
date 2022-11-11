#include "parser.hpp"

#define RAM_SIZE 16777216

void fixSourceFileName(std::string* sourceFileName);

int main(int argc, char* argv)
{
	std::string sourceFileName("");
	std::vector<uint8_t> binaryData;
	std::vector<std::string> sourceFileStr;

	Parser* p = new Parser(&sourceFileStr);

	std::cout << "=== HBC-2 Assembler ===" << std::endl << std::endl;
	std::cout << "Source files must be stored in the \"roms\" directory next to this executable." << std::endl << std::endl;
	std::cout << "Please specify the source file name to assemble (*.has) : ";

	std::cin >> sourceFileName;

	fixSourceFileName(&sourceFileName);

	std::cout << "Trying to open source file \"" << sourceFileName << ".has\"..." << std::endl;

	// Opening source file
	std::ifstream sourceFile("roms\\" + sourceFileName + ".has", std::ios::in);
	if (sourceFile.is_open())
	{
		std::string line;
		unsigned int i(1);

		std::cout << std::endl << "-- SOURCE FILE CONTENT --" << std::endl;

		while (std::getline(sourceFile, line))
		{
			sourceFileStr.push_back(line);
			std::cout << i << " : " << line << std::endl;

			i++;
		}

		std::cout << "-- SOURCE FILE END --" << std::endl << std::endl;

		// TODO
		// Handle empty line with spaces of tabs

		p->removeComments();
		p->removeEmptyLines();
		p->fixIndentation();

		ErrorMsg err(p->parseFile());

		switch (err.code)
		{
			case ErrorCode::KEYWORD_INVALID:
				std::cout << "Error at line " << err.line << " : Invalid keyword" << std::endl;
				break;

			case ErrorCode::ARGS_NB_INVALID:
				std::cout << "Error at line " << err.line << " : Too few or too many arguments" << std::endl;
				break;

			case ErrorCode::BYTE_SPEC_INVALID:
				std::cout << "Error at line " << err.line << " : Invalid address byte specification" << std::endl;
				break;

			case ErrorCode::DEC_VALUE_TOO_HIGH:
				std::cout << "Error at line " << err.line << " : Decimal value too high (> 255)" << std::endl;
				break;

			case ErrorCode::HEX_ADDRESS_TOO_HIGH:
				std::cout << "Error at line " << err.line << " : Hexadecimal address too high (> 0xffffff)" << std::endl;
				break;

			case ErrorCode::HEX_VALUE_TOO_HIGH:
				std::cout << "Error at line " << err.line << " : Hexadecimal value too high (> 0xff)" << std::endl;
				break;

			case ErrorCode::INVALID_HEX_ADDRESS:
				std::cout << "Error at line " << err.line << " : Invalid hexadecimal address" << std::endl;
				break;

			case ErrorCode::INVALID_HEX_VALUE:
				std::cout << "Error at line " << err.line << " : Invalid hexadecimal value" << std::endl;
				break;

			case ErrorCode::REG_CONCAT_INVALID:
				std::cout << "Error at line " << err.line << " : Invalid register concatenation" << std::endl;
				break;

			case ErrorCode::REG_CONCAT_MISSING_BRACKET:
				std::cout << "Error at line " << err.line << " : Missing \"]\"" << std::endl;
				break;

			case ErrorCode::REG_CONCAT_TOO_FEW:
				std::cout << "Error at line " << err.line << " : Too few registers to concatenate" << std::endl;
				break;

			case ErrorCode::REG_CONCAT_TOO_MANY:
				std::cout << "Error at line " << err.line << " : Too many registers to concatenate" << std::endl;
				break;
				
			case ErrorCode::REG_INVALID:
				std::cout << "Error at line " << err.line << " : Invalid register" << std::endl;
				break;

			default:
				std::cout << "Binary file assembled successfully." << std::endl;
		}
	}
	else
	{
		std::cout << "Unable to open file " << sourceFileName << ".has\"." << std::endl;
	}

	return 0;
}

void fixSourceFileName(std::string* sourceFileName)
{
	if (sourceFileName->size() > 4) // The extension could have been written by user
	{
		if (sourceFileName->substr(sourceFileName->size() - 4) == ".has")
		{
			*sourceFileName = sourceFileName->substr(0, sourceFileName->size() - 4);
		}
	}
}
