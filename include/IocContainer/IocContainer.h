#pragma once

// Forward include to the interface - concrete implementation is private
#include "IocContainer/IIocContainer.h"

/**
 * This header provides access to the IoC Container functionality.
 * 
 * IMPORTANT: The concrete IocContainer class is intentionally private.
 * Users can ONLY access IoC functionality through the IIocContainer interface.
 * 
 * Usage:
 *   auto& container = ioccontainer::IIocContainer::getInstance();
 *   container.registerInstance<IMyService>(myService);
 *   auto service = container.resolve<IMyService>();
 * 
 * The concrete implementation is completely hidden to prevent
 * accidental usage of implementation details.
 */

namespace ioccontainer {
    // All IoC functionality is available through IIocContainer interface
    // No concrete class is exposed in public headers
}