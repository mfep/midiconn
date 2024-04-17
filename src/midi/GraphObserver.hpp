#pragma once
#include <cstdlib>
#include <span>
#include <vector>

namespace mc::midi
{

class GraphObserver
{
public:
    virtual ~GraphObserver()                                                     = default;
    virtual void message_processed(std::span<const unsigned char> message_bytes);
    virtual void message_received(std::span<const unsigned char> message_bytes);
};

class GraphObservable
{
public:
    virtual ~GraphObservable() = default;
    void add_observer(GraphObserver* observer);
    void remove_observer(GraphObserver* observer);

protected:
    void raise_message_processed(std::span<const unsigned char> message_bytes) const;
    void raise_message_received(std::span<const unsigned char> message_bytes) const;

private:
    std::vector<GraphObserver*> m_observers;
};

} // namespace mc::midi
