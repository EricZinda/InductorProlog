#ifndef UNITTEST_ASSERTEXCEPTION_H
#define UNITTEST_ASSERTEXCEPTION_H

#include <exception>


namespace UnitTest {

class AssertException : public std::exception
{
public:
    AssertException(char const* description, char const* filename, int lineNumber);
    virtual ~AssertException() throw();

    virtual char const* what() const throw();

    char const* Filename() const;
    int LineNumber() const;

private:
	static const int descriptionSize = 512;
    char m_description[descriptionSize];
	static const int filenameSize = 256;
	char m_filename[filenameSize];
    int m_lineNumber;
};

}

#endif
