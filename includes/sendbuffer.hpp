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
        SendBuffer(void);
        ~SendBuffer(void);
        
        void    clear();
        unsigned int get_offset(void) const;
        void    set_offset(unsigned int offset);
};

#endif  /* SENDBUFFER_HPP */
