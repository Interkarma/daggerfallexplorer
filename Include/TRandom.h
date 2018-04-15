#if !defined(AFX_TRANDOM_H__1DE3DAB8_C9EB_4A18_8628_525617ADECBA__INCLUDED_)
#define AFX_TRANDOM_H__1DE3DAB8_C9EB_4A18_8628_525617ADECBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class TRandom  
{
public:		// construction / destruction
	TRandom();
	TRandom( unsigned long seed );
	virtual ~TRandom();

public:		// methods
	void			Seed( unsigned long seed );
	unsigned long	Rnd();

private:	// data
	int mti;
	unsigned long *mt;
};


#endif // !defined(AFX_TRANDOM_H__1DE3DAB8_C9EB_4A18_8628_525617ADECBA__INCLUDED_)
