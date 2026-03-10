#pragma once
#include <Arduino.h>
#include <string>
#include <vector>
// #include <map>
using std::string;
using std::vector;
// using std::map;

#include "app/configs/BleConfig.hpp"
#include "app/configs/QueuesConfig.hpp"

#include "app/configs/HardwareMapConf.hpp"

#include "app/service/RTOS/Tasks/TaskService.hpp"
#include "app/service/RTOS/Queues/QueueService.hpp"

#include "app/service/bluetooth/ble/BleService.hpp"