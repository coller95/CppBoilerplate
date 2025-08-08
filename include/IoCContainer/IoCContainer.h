#pragma once

#include <typeindex>
#include <unordered_map>
#include <functional>
#include <memory>

/**
 * IoCContainer provides dependency registration and resolution for modular components.
 * Supports registration of factory functions for any type.
 */
class IoCContainer {
public:
    using Factory = std::function<std::shared_ptr<void>()>;

    IoCContainer() = default;
    ~IoCContainer() = default;

    /**
     * Registers a factory function for a given type T
     * @tparam T The type to register
     * @param factory The factory function returning a shared_ptr<T>
     */
    template<typename T>
    void registerType(Factory factory);

    /**
     * Resolves an instance of type T
     * @tparam T The type to resolve
     * @return shared_ptr<T> instance
     */
    template<typename T>
    std::shared_ptr<T> resolve();

    /**
     * Registers a factory function for a given type T globally
     * @tparam T The type to register
     * @param factory The factory function returning a shared_ptr<T>
     */
    template<typename T>
    static void registerGlobal(Factory factory);

    /**
     * Imports all globally registered factories into this container
     */
    inline void importGlobals() {
        for (const auto& pair : _globalFactories()) {
            _factories[pair.first] = pair.second;
        }
    }

private:
    std::unordered_map<std::type_index, Factory> _factories;

    static std::unordered_map<std::type_index, Factory>& _globalFactories() {
        static std::unordered_map<std::type_index, Factory> instance;
        return instance;
    }
};

#include "IoCContainer.inl"
