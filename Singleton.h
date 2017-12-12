#ifndef _SINGLETON_H_
#define _SINGLETON_H_


template <class T_DERIVED>
class CSingleton
{
public:
	static T_DERIVED& GetInstance()
	{
		static T_DERIVED oInstance;
		return oInstance;
	}

protected:
	CSingleton() { }

private:
	CSingleton( const CSingleton& );
	CSingleton& operator=( const CSingleton& ) {	return *this;	}
};


#endif