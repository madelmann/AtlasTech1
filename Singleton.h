
#pragma once

// Library includes

// Project includes


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

