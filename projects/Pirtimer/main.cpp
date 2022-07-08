/**
 * PirTimer
 * pirtimer.cpp
 * Purpose: Is an interface between a motion sensor GPIO module and a lifx lightbulb with a timeout function
 * Dependencies: timer.hpp, GPIO.hpp, sendpacket.hpp
 *
 * @author Elias Floreteng
 * @version 1.2 30/03/2020
*/

#include "main.h"

int main(/* int argc, char **argv */)
{
	std::cout << "Program started" << std::endl;

	remove("log.txt");

	log("Log entry started");

	auto bulbIp = "192.168.1.99";
	auto stipIp = "192.168.1.45";

	log("TIMEOUT value: " + std::to_string(config(TIMEOUT)));
	log("STARTTIME value: " + std::to_string(config(STARTTIME)));
	log("STOPTIME value: " + std::to_string(config(STOPTIME)));

	bool sensor = true;

	uint8_t packet[42] = { 0 };
	auto pir = GPIO::openGPIO(17, GPIO_INPUT);

	Timer countdown(std::chrono::minutes((uint32_t)std::round(config(TIMEOUT))), [&] {
		if (!pir->read())
		{
			if ((hour() > config(STARTTIME) && hour() < config(STOPTIME)) || sensor)
			{
				log("Turning off");
				buildPacket(0, 0, packet);
				sendPacket(packet, bulbIp);
				sendPacket(packet, stipIp);
			}
		}
		});

	while (true)
	{
		switch (pir->waitForAnyEdge())
		{
		case GPIO_EDGE_RISING: // When motion is detected
			if (!countdown.isRunning())
			{
				if (hour() > config(STARTTIME) && hour() < config(STOPTIME))
				{
					sensor = true;
					log("Turning on");
					//pwrLed(true);
					buildPacket(UINT16_MAX, 0, packet);
					sendPacket(packet, bulbIp);
					sendPacket(packet, stipIp);
				}
				else if (sensor)
				{
					sensor = false;
					log("Turning on for last time");
					//pwrLed(false);
					buildPacket(UINT16_MAX, 0, packet);
					sendPacket(packet, bulbIp);
					sendPacket(packet, stipIp);
				}
			}
			// log("Resetting timer");
			countdown.stop();
			break;

		case GPIO_EDGE_FALLING: // Start timer
		  // log("Starting timer");
			countdown.start(std::chrono::minutes((uint32_t)std::round(config(TIMEOUT))));
			break;
		default:
			break;
		}
	}
	return 0;
}

int buildPacket(uint16_t brightness, uint32_t delay, uint8_t* packet)
{
	/*std::stringstream brightnessStream;
	brightnessStream << std::setfill('0') << std::setw(4) << std::right << std::hex << __builtin_bswap16(brightness);
	std::stringstream delayStream;
	delayStream << std::setfill('0') << std::setw(8) << std::right << std::hex << __builtin_bswap32(delay);*/
	auto swpbrightness = __builtin_bswap16(brightness);
	auto swpdelay = __builtin_bswap32(delay);

	packet[0] = sizeof(packet);
	packet[3] = 0x34;
	packet[4] = 0xb4;
	packet[5] = 0x3c;
	packet[6] = 0xf0;
	packet[7] = 0x84;
	packet[22] = 0x01;
	packet[23] = 0x0d;
	packet[32] = 0x75;
	memcpy(&packet[36], &swpbrightness, sizeof(brightness));
	memcpy(&packet[38], &swpdelay, sizeof(delay));

	/*packet[36] = std::stoul("0x" + brightnessStream.str().substr(2, 2), nullptr, 16);
	packet[37] = std::stoul("0x" + brightnessStream.str().substr(4, 2), nullptr, 16);
	packet[38] = std::stoul("0x" + delayStream.str().substr(2, 2), nullptr, 16);
	packet[39] = std::stoul("0x" + delayStream.str().substr(4, 2), nullptr, 16);
	packet[40] = std::stoul("0x" + delayStream.str().substr(6, 2), nullptr, 16);
	packet[41] = std::stoul("0x" + delayStream.str().substr(8, 2), nullptr, 16);*/

	return 0;
}

int pwrLed(bool powerLevel)
{
	std::ofstream fs;
	fs.open("/sys/class/leds/led0/brightness");
	fs << (powerLevel ? "0" : "255");
	return 0;
}

double config(ConfigKey key)
{
	std::string filename;
	switch (key)
	{
	case TIMEOUT:
		filename = "timeout";
		break;
	case STARTTIME:
		filename = "start";
		break;
	case STOPTIME:
		filename = "stop";
		break;
	default:
		log("Error: Key value not in ConfigKey enum");
		std::cerr << "Error: Key value not in ConfigKey enum" << std::endl;
	}
	std::ifstream fs;
	try {
		fs.open(configLocation + filename + ".val");
		if (fs.is_open())
		{
			double num = 0;
			fs >> num;
			return num;
		}
	}
	catch (const std::exception& err)
	{
		log("Error opening file: " + (std::string)err.what());
		std::cerr << "Error opening file: " << err.what() << std::endl;
		throw;
	}
	return -1;
}

double hour()
{
	time_t now = time(0);
	tm* ltm = localtime(&now);

	double minute = (double)(ltm->tm_min) / (double)60;
	double hourDecimal = ltm->tm_hour + minute;
	return hourDecimal;
}

int log(std::string input)
{
	char message[input.size() + 1];
	strcpy(message, input.c_str());

	char buff[20];
	struct tm* sTm;

	time_t now = time(0);
	sTm = localtime(&now);

	strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
	std::cout << message << std::endl;

	ofstream logfile{ "log.txt", std::ios_base::app };
	if (logfile.is_open())
		logfile << "[" << buff << "] " << message << std::endl;
	logfile.close();
	return 0;
}