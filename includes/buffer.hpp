#ifndef BUFFER_HPP
#define BUFFER_HPP

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

    unsigned int get_offset() const;
    void         set_offset(unsigned int offset);
    void         clear();
};

#endif /* BUFFER_HPP */
