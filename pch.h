#pragma once

#include <iostream>
#include <memory>

// Allows us to create a "Scoped" pointer that will automatically destroy when out of scope
template<typename T>
using Scope = std::unique_ptr<T>; // Allows us to create a unique_ptr by calling "Scope<T>"
template<typename T, typename ... Args> // Allows us to create a unique_ptr of type "T" with arguments of type "Args"
constexpr Scope<T> CreateScope(Args&& ... args) // Takes any number of arguments of type "Args". Arguments must be rvalues (&&). This allows is to properly "move" the arguments to the make_unique constructor 
{
	return std::make_unique<T>(std::forward<Args>(args)...); // Forwards our arguments to make_unique while retaining rvalueness (AKA: "moves" the arguments as opposed to copying)
}

// Allow us to create a "Reference" pointer that will effectivley be the same as above but it is not unique
template<typename T>
using Ref = std::shared_ptr<T>; // Allows us to create a shared_ptr by calling "Ref<T>"
template<typename T, typename ... Args> // Allows us to create a shared_ptr of type "T" with arguments of type "Args"
constexpr Ref<T> CreateRef(Args&& ... args) // Takes any number of arguments of type "Args". Arguments must be rvalues (&&). This allows is to properly "move" the arguments to the make_shared constructor 
{
	return std::make_shared<T>(std::forward<Args>(args)...); // Forwards our arguments to make_shared while retaining rvalueness (AKA: "moves" the arguments as opposed to copying)
}