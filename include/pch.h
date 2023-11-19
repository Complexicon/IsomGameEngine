#pragma once

#include <string>
#include <thread>
#include <vector>
#include <list>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <array>
#include <stdint.h>
#include "engine/structs.h"

using std::string;
using std::thread;
using std::vector;
using std::array;
using std::list;
template <class K, class V> using map = std::unordered_map<K,V>;
using std::function;
using std::runtime_error;