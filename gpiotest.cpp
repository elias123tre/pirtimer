/*
 *  The following is a very simple example on using the GPIO pins on your Pi.
 *  In this example, an LED (connected to pin 25 and GND) will light up until
 *  a button (connected to pin 3 and GND) is pressed which turns the LED off
 *  and terminates the program.
 *  Note that pin 3 has an internal pull-up resistor, which means that pushing
 *  the button causes the pin to read LOW.
 */

#include "GPIO.hpp"
#include "timer.hpp"
#include <chrono>             // Chrono: time
#include <condition_variable> // Condition Variable
#include <iostream>           // cout
#include <thread>             // Thread
#include <unistd.h>           // usleep

int main()
{
    auto pir = GPIO::openGPIO(17, GPIO_INPUT);

    Timer t(15, [] {
        std::cout << "Timer done" << std::endl;
    });

    std::cout << pir->read() << std::endl;

    while (true)
    {
        auto edge = pir->waitForAnyEdge();
        if (edge == GPIO_EDGE_FALLING)
        {
            std::cout << "Starting timer" << std::endl;
            t.start();
        }
        if (edge == GPIO_EDGE_RISING)
        {
            std::cout << "Stopping timer" << std::endl;
            t.stop();
        }
    }

    return 0;
}