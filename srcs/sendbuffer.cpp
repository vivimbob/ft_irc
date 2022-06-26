#include "../includes/sendbuffer.hpp"

/* sendbuffer class member function begin */

SendBuffer::SendBuffer() : m_offset(0)
{
}

SendBuffer::~SendBuffer()
{
}

void
    SendBuffer::clear()
{
    this->std::string::clear();
    m_offset = 0;
}

unsigned int
    SendBuffer::get_offset() const
{
    return m_offset;
}

void
    SendBuffer::set_offset(unsigned int offset)
{
    m_offset = offset;
}

/* sendbuffer class member function end */