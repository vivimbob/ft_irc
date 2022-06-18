class ICommand
{
    virtual void empty()        = 0;
    virtual void pass()         = 0;
    virtual void nick()         = 0;
    virtual void user()         = 0;
    virtual void quit()         = 0;
    virtual void join()         = 0;
    virtual void part()         = 0;
    virtual void topic()        = 0;
    virtual void names()        = 0;
    virtual void list()         = 0;
    virtual void invite()       = 0;
    virtual void kick()         = 0;
    virtual void mode()         = 0;
    virtual void privmsg()      = 0;
    virtual void notice()       = 0;
    virtual void unknown()      = 0;
    virtual void unregistered() = 0;
};
