#ifndef __CONTEXT_H__
#define __CONTEXT_H__

/// @brief Directive's contexts.
enum class Context
{
	HTTP,
	SERVER,
	LOCATION,
	LIMIT_EXCEPT,
	NONE,
};

#endif

//limit_except is actually a directive but the parsing is like a context because of the braces.