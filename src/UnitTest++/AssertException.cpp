#include "AssertException.h"
#include <cstring>

namespace UnitTest {

AssertException::AssertException(char const* description, char const* filename, int lineNumber)
    : m_lineNumber(lineNumber)
{
	using namespace std;

	strcpy_s(m_filename, filenameSize, filename);
    strcpy_s(m_description, descriptionSize, description);
}

AssertException::~AssertException() throw()
{
}

char const* AssertException::what() const throw()
{
    return m_description;
}

char const* AssertException::Filename() const
{
    return m_filename;
}

int AssertException::LineNumber() const
{
    return m_lineNumber;
}

}
