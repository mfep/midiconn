#pragma once
#include <cstdlib>
#include <vector>

namespace mc::midi
{

class OutputObserver
{
public:
    virtual ~OutputObserver()                                                             = default;
    virtual void message_sent(size_t id, const std::vector<unsigned char>& message_bytes) = 0;
};

class OutputObservable
{
public:
    virtual ~OutputObservable() = default;
    void add_observer(OutputObserver* observer);
    void remove_observer(OutputObserver* observer);

protected:
    void raise_message_sent(size_t id, const std::vector<unsigned char>& message_bytes) const;

private:
    std::vector<OutputObserver*> m_observers;
};

} // namespace mc::midi
