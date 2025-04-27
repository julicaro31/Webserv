#ifndef PARSER_H
#define PARSER_H

#include "Token.hpp"
#include <vector>
#include "Logger.hpp"

class Parser {
public:
	Parser(std::vector<Token>);
	Parser(const Parser &);
	~Parser(void);
	bool httpMessage(void);
private:
	bool startLine(void);
	bool method(void);
	bool space(void);
	bool requestTarget(void);
	bool httpVersion(void);
	bool crlf(void);
	bool fieldLine(void);
	bool messageBody(void);
	bool eof(void);
	static void error(int, std::string, std::string);
	std::vector<Token> tokens;
	int current = 0;
};

// the RFC ( https://datatracker.ietf.org/doc/html/rfc9112 ) uses
// augmented Backus-Naur Form (https://en.wikipedia.org/wiki/Augmented_Backus%E2%80%93Naur_form)
//
// core rules:
// The core rules are defined in the ABNF standard.
// Rule 	Formal definition									Meaning
// ALPHA	| 	%x41–5A / %x61–7A								| 	Upper- and lower-case ASCII letters (A–Z, a–z)
// DIGIT	| 	%x30–39											| 	Decimal digits (0–9)
// HEXDIG	| 	DIGIT / "A" / "B" / "C" / "D" / "E" / "F"		| 	Hexadecimal digits (0–9, A–F, a–f)
// DQUOTE	| 	%x22											| 	Double quote
// SP		| 	%x20											| 	Space
// HTAB		| 	%x09											| 	Horizontal tab
// WSP		| 	SP / HTAB										| 	Space and horizontal tab
// LWSP		| 	*(WSP / CRLF WSP)								| 	Linear white space (past newline)
// VCHAR	| 	%x21–7E											| 	Visible (printing) characters
// CHAR		| 	%x01–7F											| 	Any ASCII character, excluding NUL
// OCTET	| 	%x00–FF											| 	8 bits of data
// CTL		| 	%x00–1F / %x7F									| 	Controls
// CR		| 	%x0D											| 	Carriage return
// LF		| 	%x0A											| 	Linefeed
// CRLF		| 	CR LF											| 	Internet-standard newline
// BIT		| 	"0" / "1"										| 	Binary digit

//

// HTTP-message = start-line CRLF *( field-line CRLF ) CRLF [ message-body ]
// start-line = request-line / status-line
// request-line = method SP request-target SP HTTP-version
// request-target = origin-form / absolute-form / authority-form / asterisk-form
// status-line = HTTP-version SP status-code SP [ reason-phrase ]
// status-code = 3DIGIT
// CRLF = CR LF
// field-line = field-name ":" OWS field-value OWS
// field-content = field-vchar [ 1*( SP / HTAB / field-vchar ) field-vchar ]
// field-name = token
// field-value = *field-content
// field-vchar = VCHAR / obs-text
// HTTP-version = HTTP-name "/" DIGIT "." DIGIT
// OWS = *( SP / HTAB )
// origin-form = absolute-path [ "?" query ]
// absolute-URI = <absolute-URI, see [URI], Section 4.3>
// absolute-form = absolute-URI
// absolute-path = 1*( "/" segment )
// asterisk-form = "*"
// message-body = *OCTET
// method = token
// obs-date = rfc850-date / asctime-date
// obs-fold = OWS CRLF RWS
// obs-text = %x80-FF
// port = <port, see [URI], Section 3.2.3>
// reason-phrase = 1*( HTAB / SP / VCHAR / obs-text )
// token = 1*tchar
// uri-host = <host, see [URI], Section 3.2.2>
//


#endif
