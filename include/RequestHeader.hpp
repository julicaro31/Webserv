#ifndef __REQUEST_HEADER_H__
#define __REQUEST_HEADER_H__

enum class RequestHeader
{
	CacheControl,
	Expect,
	Host,
	MaxForwards,
	Pragma,
	Range,
	TE,
	IfMatch,
	IfNoneMatch,
	IfModifiedSince,
	IfUnmodifiedSince,
	IfRange,
	Accept,
	AcceptCharset,
	AcceptEncoding,
	AcceptLanguage,
	Authorization,
	ProxyAuthorization,
	From,
	Referer,
	UserAgent
};

#endif