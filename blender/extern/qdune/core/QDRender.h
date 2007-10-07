#ifndef _QDRENDER_H
#define _QDRENDER_H

#define __BEGIN_QDRENDER namespace QDRender {
#define __END_QDRENDER }

#include <exception>
#include <cstdio>

//-------------------------------------------------------------------------------
// exception class for error reports
class RiException_t : public std::exception
{
private:
	RiException_t& operator=(const RiException_t&);
public:
	RiException_t(const char* _msg) : std::exception(), msg(_msg) {}
	RiException_t(const RiException_t& rie) : std::exception(), msg(rie.msg) {}
	virtual ~RiException_t() throw() {}
	virtual const char* what() const throw() { return msg; }
protected:
	const char* msg;
};

#endif // _QDRENDER_H
