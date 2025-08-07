#pragma once

#include <typeindex>
#include <memory>
#include <functional>
#include <stdexcept>

// Inline implementations for IoCContainer

template<typename T>
void IoCContainer::registerType(std::function<std::shared_ptr<T>()> factory) {
    _factories[std::type_index(typeid(T))] = [factory]() {
        return std::static_pointer_cast<void>(factory());
    };
}

template<typename T>
std::shared_ptr<T> IoCContainer::resolve() {
    auto it = _factories.find(std::type_index(typeid(T)));
    if (it == _factories.end()) {
        throw std::runtime_error("Type not registered in IoCContainer");
    }
    return std::static_pointer_cast<T>(it->second());
}
