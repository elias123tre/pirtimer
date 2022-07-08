#pragma once

#include "GPIO.h"
#include "sendpacket.h"
#include "timer.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <time.h>
#include <cmath>

std::string configLocation = "/etc/pirtimer/";

/// Set onboard LED to a specific state
/// @param powerLevel The state to set the LED to
/// @return Error code: 0 on success
int pwrLed(bool powerLevel);

/// Make a packet for changing a lifx bulb's state
/// @param brightness The brightness to set the light to: Between 0 and UINT16_MAX
/// @param delay The time for the change to be executed in: Between 0 and UINT32_MAX
/// @param packet The packet variable to store the built packet in
/// @return Error code: 0 on success
int buildPacket(uint16_t brightness, uint32_t delay, uint8_t* packet);

/// Print to cout with prepended timestamp
/// @param input String to be logged/printed
/// @return Error code: 0 on success
int log(std::string input);

/// Get the current time
/// @return The current time in fractional hours
double hour();

typedef enum
{
	TIMEOUT,
	STARTTIME,
	STOPTIME
} ConfigKey;

/// Get the a file value corresponding to the key
/// @param key The config value to retrieve
/// @return The config value
double config(ConfigKey key);