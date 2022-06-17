#ifndef SENDBUFFER_HPP
#define SENDBUFFER_HPP

#include <string>

class Buffer : public std::string
{
  private:
    unsigned int _offset;

    Buffer(const Buffer& copy);
    Buffer& operator=(const Buffer& copy);

  public:
    Buffer();
    ~Buffer();

    void         clear();
    unsigned int get_offset() const;
    void         set_offset(unsigned int offset);
};

#endif /* SENDBUFFER_HPP */
