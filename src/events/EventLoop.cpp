//
// Created by Kacper Skelnik on 16.04.2025.
//

#include "EventLoop.h"

#include <utility>

void EventLoop::runSingle() const {
    if (const auto optionalEvent = eventQueue->front(); optionalEvent.has_value()) {
        const std::shared_ptr<Event>& event = optionalEvent.value();
        event->execute();
        eventQueue->pop();
    }
}

EventLoop::EventLoop(std::shared_ptr<EventQueue> eventQueue): eventQueue(std::move(eventQueue)) {}

void EventLoop::run() const {
    while (eventQueue->size() > 0) {
        runSingle();
    }
}