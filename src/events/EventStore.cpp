//
// Created by Kacper Skelnik on 15.03.2025.
//

#include "EventStore.h"

EventStore::EventStore(): events(std::queue<std::shared_ptr<Event>>()) {}

void EventStore::push(const std::shared_ptr<Event>& event) {
    events.push(event);
}

std::optional<std::shared_ptr<Event>> EventStore::front() const {
    if (events.empty()) {
        return std::nullopt;
    }
    std::shared_ptr<Event> event = this->events.front();
    return event;
}

void EventStore::pop() {
    if (!events.empty()) {
        events.pop();
    }
}

unsigned long EventStore::size() const {
    return events.size();
}