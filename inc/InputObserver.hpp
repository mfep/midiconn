#pragma once
#include <cstdlib>
#include <vector>

namespace mc::midi
{

class InputObserver
{
public:
    virtual ~InputObserver() = default;
    virtual void message_received(size_t id, const std::vector<unsigned char>& message_bytes) = 0;
};

class InputObservable
{
public:
    virtual ~InputObservable() = default;
    void add_observer(InputObserver* observer);
    void remove_observer(InputObserver* observer);

protected:
    void raise_message_received(size_t id, const std::vector<unsigned char>& message_bytes) const;

private:
    std::vector<InputObserver*> m_observers;
};

}
