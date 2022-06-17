#include "../includes/sendbuffer.hpp"

/* sendbuffer class member function begin */

Buffer::Buffer() : _offset(0)
{
}

Buffer::~Buffer()
{
}

void
    Buffer::clear()
{
    this->std::string::clear();
    _offset = 0;
}

unsigned int
    Buffer::get_offset() const
{
    return _offset;
}

void
    Buffer::set_offset(unsigned int offset)
{
    _offset = offset;
}

/* sendbuffer class member function end */
