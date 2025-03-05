#include "HttpParser.hpp"

void HttpParser::parseRequest(std::string strRequest)
{
	//	symbol table
	//	analysis
	//
	//	2. lexical analyzer
	//		2.2. context-free grammar
	//			2.2.1 Definition of Grammars
	//				1. terminal symbols (tokens)
	//					elementary symbols of language defined by grammar
	//				2. nonterminal (syntactic variables)
	//					set of string of terminals
	//				3. productions
	//					nonterminal head / left side with arrow and 
	//					sequence of terminals/nonterminals called the body/right side.
	//					stmt -> if (expr) stmt else stmt
	//				4. designation of nonterminal as start symbol
	//					grammar is specified by listing productions with start symbol first.
	//					italicized names in notation are assumed to be nonterminals, here [nonterminal] is used, everything else is terminal
	//					with "|" symbol being used in notation to group different bodies of statements
	//					e.g.
	//						(9-5+2, 3-1, 7) = "list of digits separated by + or - signs
	//					grammar to describe these expressions, with the productions:
	//						1.	[list]-> [list] + [digit]
	//						2.	[list] -> [list] - [digit]
	//						3.	[list] -> [digit]
	//						4.	[digit] -> 0 | 1 | 2 | 3 | 4 | etc.
	//					the productions with nonterminal [list] can be grouped as:
	//						[list] -> [list] + [digit]	| [list] - [digit] | [digit]
	//					terminals of grammar are then:
	//						+ - 0 1 2 3 4 5 6 etc.
	//					a production is FOR a non terminal if the nonterminal is the head of the production
	//					a string of terminals is a sequence of zero or more terminals
	//					a string of zero terminal is written as ~e, which is called an empty string
	//			2.2.2 derivations
	//				Using the productions, one can define a desired language, productions 1 - 4 can be used as followed:
	//					we can deduce "9-5+2" is a list because:
	//						a. 9 is a list by production 3, since 9 is a digit
	//						b. 9 - 5 is a list by production 2, since 9 is a list and 5 is a digit.
	//						c. 9-5+2 is a list by production 1, since 9-5 is a list and 2 is a digit.
	//				Parsing is the problem of taking a string of terminals and trying to derive the start symbol of the grammar
	//				and reporting syntax errors if it cannot be derived
	//				In general, a source program has multicharacter lexemes that are grouped by lexical analyser into tokens,
	//				whoes first components are terminals processed by the parser
	//			2.2.3 Parse Trees
	//				a parse tree shows how the start symbol of a gramma derives a string in the languages
	//				Nonterminal A which has production A -> XY Z, then it has A as an interior node A with three children X, Y, Z
	//					A
	//				  / | \
	//				X	Y  Z
	//				The order from left to right matter for the nodes in the tree. 
	//		token stream
	//			divide into lexemes
	//				lexem = {tokenName, attributeValue}
	//				token names are often terminals
	//				attribute value often a pointer to symbol table
	//			map to symbol table
	//		pass to syntax analyzer
	//
	//	syntax analyzer
	//		create syntax-tree of token stream
	//
	//	semantic analyzer
	//		use syntax tree and symbol table
	//		type checking
	//
	//	synthesis / code generation
	//		map into request
	

	// std::stringstream streamStrRequest(strRequest);
	// std::string method, target, version;
	//
	// streamStrRequest >> method >> target >> version;
	//
	// std::map<std::string, std::string> headers;
	// std::string header;
	// std::getline(streamStrRequest, header);
	// while (getline(streamStrRequest, header))
	// {
	// 	if (header == "\r" || header.empty())
	// 	{
	// 		break;
	// 	}
	//
	// 	size_t colonPos = header.find(':');
	// 	if (colonPos != std::string::npos)
	// 	{
	// 		std::string headerName = header.substr(0, colonPos);
	// 		std::string headerValue = std::string("lol");//ParsingHelper::trim(header.substr(colonPos + 1));
	// 		headers[headerName] = headerValue;
	// 	}
	// }

	// Request request(method, target, version, headers);

	// return Request();
}

