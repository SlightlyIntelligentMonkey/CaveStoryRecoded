#pragma once

#include <string>

//Functions
[[noreturn]] void doError();
[[noreturn]] void doCustomError(const std::string& msg);
