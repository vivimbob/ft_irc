#ifndef SENDBUFFER_HPP
#define SENDBUFFER_HPP

#include <string>

class SendBuffer : public std::string
{
    private:
        unsigned int m_offset;
        
        SendBuffer(const SendBuffer& copy);
        SendBuffer &operator=(const SendBuffer& copy);
    public:
        SendBuffer(void)
            : m_offset(0)
        {
        }

        ~SendBuffer(void)
        {
        }
        
        void    clear()
        {
            this->std::string::clear();
            m_offset = 0;
        }

        unsigned int get_offset(void) const
        {
            return m_offset;
        }

        void    set_offset(unsigned int offset)
        {
            m_offset = offset;
        }
};

#endif  /* SENDBUFFER_HPP */