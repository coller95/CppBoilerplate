#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

/**
 * IoCContainer provides dependency registration and resolution for modular components.
 * Supports registration of factory functions for any type.
 */
class IoCContainer {
public:
    IoCContainer() = default;
    ~IoCContainer();

    /**
     * Registers a factory function for a given type T
     * @tparam T The type to register
     * @param factory The factory function returning a shared_ptr<T>
     */
    template<typename T>
    void registerType(std::function<std::shared_ptr<T>()> factory);

    /**
     * Resolves an instance of type T
     * @tparam T The type to resolve
     * @return shared_ptr<T> instance
     */
    template<typename T>
    std::shared_ptr<T> resolve();

private:
    std::unordered_map<std::type_index, std::function<std::shared_ptr<void>()>> _factories;
};

#include "IoCContainer.inl"
