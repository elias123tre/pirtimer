/* =============================================================================
 *  Author: Adam Dobos
 *
 *  Description:
 *  This class lets you access the GPIO pins on the Raspberry Pi through an
 *  easy-to-use, modern C++ interface.
 *  Please keep in mind that this was only tested to work on Raspbian.
 *
 *  A C++ 14 compiler is required!
 * =============================================================================
 */

#ifndef _GPIO_HPP_
#define _GPIO_HPP_

#if !defined(__cplusplus) || __cplusplus < 201300L
#error Error: A C++ 14 compatible compiler is required!
#else

#include <string>
#include <chrono>
#include <stdexcept>
#include <fcntl.h>
#include <sys/mman.h>
#include <poll.h>
#include <unistd.h>
#include <memory>
#include <cstdio>
using namespace std;

/* 
 * The register manipulation method is based on the code available at:
 *
 * http://elinux.org/RPi_GPIO_Code_Samples#C
 *
 */

// Access from ARM Running Linux
#define BCM2708_PERI_BASE 0x20000000
#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#define PAGE_SIZE (4 * 1024)
#define BLOCK_SIZE PAGE_SIZE

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio + ((g) / 10)) &= ~(7 << (((g) % 10) * 3))
#define OUT_GPIO(g) *(gpio + ((g) / 10)) |= (1 << (((g) % 10) * 3))
#define SET_GPIO_ALT(g, a) *(gpio + (((g) / 10))) |= (((a) <= 3 ? (a) + 4 : (a) == 4 ? 3 : 2) << (((g) % 10) * 3))

#define GPIO_SET *(gpio + 7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio + 10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio + 13) & (1 << g)) // 0 if LOW, (1<<g) if HIGH

// Make it easier to use chrono...
#define CHRONO_NOW chrono::high_resolution_clock::now()
//#define CHRONO_DURATION(first,last) chrono::duration_cast<chrono::duration<double>>(last-first).count()

/// Defines how the GPIO pins are accessed
typedef enum
{
    /// In this mode, the pins will be accessed directly through registers. If you don't know what that means, please stay away from using this! This mode will give you ~19 MHz of maximum output toggling frequency.
    GPIO_MODE_DIRECT,
    /// In this mode, the pins will be accessed through the OS' "sysfs" interface. This is the default mode, since it's a lot safer than the other method. This mode will give you ~8 kHz of maximum output toggling frequency.
    GPIO_MODE_SYSFS
} GPIOMode;

/// Defines whether the pin should be configured for input or output.
typedef enum
{
    /// Sets up the pin as an input
    GPIO_INPUT,
    /// Sets up the pin as an output, with the initial value of 0
    GPIO_OUTPUT_INIT_LOW,
    /// Sets up the pin as an output, with the initial value of 1
    GPIO_OUTPUT_INIT_HIGH
} GPIODirection;

/// Defines edge type
typedef enum
{
    /// Falling edge
    GPIO_EDGE_FALLING,
    /// Rising edge
    GPIO_EDGE_RISING
} GPIOEdge;

inline long filesize(FILE *_file)
{
    auto orPos = ftell(_file);
    fseek(_file, 0, SEEK_END);
    auto fSize = ftell(_file);
    fseek(_file, orPos, SEEK_SET);
    return fSize;
}

/// This class provides a simplified interface for accessing the GPIO pins on the Raspberry Pi.
class GPIO
{
public:
    /// This function opens a connection to a specific pin on your Pi, creating a new instance of the GPIO class. If you wish to access multiple pins, you will have to instantiate the GPIO class multiple times. Each instance is responsible for controlling one pin only.
    /// @param n The number of the GPIO pin you wish to access (see http://elinux.org/File:GPIOs.png )
    /// @param direction This defines the direction of the pin (input or output)
    /// @return A shared_ptr to a newly created instance of the GPIO class. This object will let you access the GPIO pin you requested.
    static shared_ptr<GPIO> openGPIO(int32_t n, GPIODirection direction);

    /// This function changes how the GPIO pins are accessed. Please DON'T change around the access mode unless you have a good reason to do so! Notice that this is a static function, so you only want to call it once, and the change will apply for all the GPIO connections. IMPORTANT: it's only safe call this function BEFORE creating any instances of the class!
    /// @param mode The access method you wish to use
    static void setMode(GPIOMode mode);

    /// This sets the direction of a GPIO pin. Unless you need to change a pin's direction on the fly, you don't need to call this function, since every pin is initially given a direction of your choice, once you request access to it.
    /// @see openGPIO
    /// @param direction This defines the direction of the pin (input or output)
    void setDirection(GPIODirection direction);

    /// Returns the current direction of the pin
    GPIODirection getDirection();

    /// Writes either a HIGH or a LOW value to the GPIO pin. This is only possible if the pin was set up to be an output pin. If you try to write to an input pin, an exception will be thrown.
    /// @param value The value you wish to write to the pin.
    inline void write(bool value);

    /// Reads the current value of the pin, which can be either HIGH or LOW.
    /// @return The current state of the pin.
    inline bool read();

    /// This function will not return until a specified type of edge (change in value) is detected on the GPIO pin.
    /// @see waitForAnyEdge
    /// @param edgeType The type of edge to listen to.
    /// @param timeout (optional) Timeout in milliseconds.
    inline void waitForEdge(GPIOEdge edgeType, int32_t timeout = -1);

    /// This function will not return until any kind of edge (a change in value) is detected on the GPIO pin.
    /// @see waitForEdge
    /// @param timeout (optional) Timeout in milliseconds.
    /// @return The kind of edge that was detected.
    inline GPIOEdge waitForAnyEdge(int32_t timeout = -1);

    /// Returns the number of the GPIO pin (the same number you provided in the first argument of openGPIO)
    int32_t getNumber();

    /// Don't use these manually
    ~GPIO();
    GPIO(int32_t n);

private:
    void _export();
    void _unexport();
    inline void dEdgeInterruption(string getedg_str);
    static void setup_io();

    static string GPIODirectory;
    static int32_t mem_fd;
    static void *gpio_map;
    static volatile unsigned *gpio;
    static GPIOMode accessMode;
    string GPIONumberString;
    int32_t GPIONumber;
    static chrono::milliseconds timeOut;
    GPIODirection curDirection;

    string setval_str;
    string export_str;
    string unexport_str;
    string getedg_str;
    string getval_str;
    string setdir_str;
};

int32_t GPIO::mem_fd = 0;
void *GPIO::gpio_map = nullptr;
volatile unsigned *GPIO::gpio = nullptr;
GPIOMode GPIO::accessMode = GPIO_MODE_SYSFS;
string GPIO::GPIODirectory = "/sys/class/gpio/";
chrono::milliseconds GPIO::timeOut = 1000ms;

inline void GPIO::dEdgeInterruption(string getedg_str)
{
    FILE *fd = fopen(getedg_str.c_str(), "w");
    if (!fd)
        throw std::runtime_error("OPERATION FAILED: Unable to perform edge detection on GPIO"s +
                                 this->GPIONumberString);
    fwrite("none", 1, 4, fd);
    fclose(fd);
}

void GPIO::setup_io()
{
    /* open /dev/mem */
    if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
        throw std::runtime_error("Can't open /dev/mem (do you have root privileges?)");

    /* mmap GPIO */
    gpio_map = mmap(
        NULL,                   //Any adddress in our space will do
        BLOCK_SIZE,             //Map length
        PROT_READ | PROT_WRITE, // Enable reading & writting to mapped memory
        MAP_SHARED,             //Shared with other processes
        mem_fd,                 //File to map
        GPIO_BASE               //Offset to GPIO peripheral
    );

    close(mem_fd); //No need to keep mem_fd open after mmap

    if (gpio_map == MAP_FAILED)
        throw std::runtime_error("mmap failed!");

    // Always use volatile pointer!
    gpio = (volatile unsigned *)gpio_map;
}

GPIO::GPIO(int32_t n) : GPIONumber(n)
{
    GPIONumberString = to_string(n);
    setval_str = GPIODirectory + "gpio" + GPIONumberString + "/value";
    export_str = GPIODirectory + "export";
    unexport_str = GPIODirectory + "unexport";
    getedg_str = GPIODirectory + "gpio" + GPIONumberString + "/edge";
    getval_str = setval_str;
    setdir_str = GPIODirectory + "gpio" + GPIONumberString + "/direction";
    _export();
}

GPIO::~GPIO() { this->_unexport(); }

shared_ptr<GPIO> GPIO::openGPIO(int32_t n, GPIODirection direction)
{
    auto newGPIOObject = make_shared<GPIO>(n);
    newGPIOObject->setDirection(direction);
    return newGPIOObject;
}

void GPIO::setMode(GPIOMode mode)
{
    if ((GPIO::accessMode = mode) == GPIO_MODE_DIRECT)
        setup_io();
}

void GPIO::_export()
{
    FILE *exportgpio = fopen(export_str.c_str(), "w");
    if (!exportgpio)
        throw std::runtime_error("OPERATION FAILED: Unable to export GPIO"s +
                                 this->GPIONumberString);
    fwrite(this->GPIONumberString.c_str(), 1, this->GPIONumberString.size(), exportgpio);
    fclose(exportgpio);
    return;
}

void GPIO::_unexport()
{
    FILE *unexportgpio = fopen(unexport_str.c_str(), "w");
    if (!unexportgpio)
        throw std::runtime_error("OPERATION FAILED: Unable to unexport GPIO"s +
                                 this->GPIONumberString);
    fwrite(this->GPIONumberString.c_str(), 1, this->GPIONumberString.size(), unexportgpio);
    fclose(unexportgpio);
    return;
}

inline void GPIO::waitForEdge(GPIOEdge edgeType, int32_t timeout)
{
    int32_t edge_fd = open(getedg_str.c_str(), O_RDWR);
    if (edge_fd == -1)
        throw std::runtime_error("OPERATION FAILED: Unable to perform edge detection on GPIO"s +
                                 this->GPIONumberString);
    ::write(edge_fd, "both", 4);
    close(edge_fd);

    int32_t val_fd = open(getval_str.c_str(), O_RDONLY);
    pollfd pollData;
    pollData.fd = val_fd;
    pollData.events = POLLPRI | POLLERR;
    pollData.revents = 0;
    char buffer{'\0'};

    ::read(val_fd, &buffer, 1); // dummy read required before polling

    buffer = '\0';

    while ((int32_t)(buffer - '0') != edgeType)
    {
        poll(&pollData, 1, timeout);
        lseek(val_fd, 0, SEEK_SET);
        ::read(val_fd, &buffer, 1);
    }
    pollData.~pollfd();
    close(val_fd);

    dEdgeInterruption(getedg_str);
}

inline GPIOEdge GPIO::waitForAnyEdge(int32_t timeout)
{
    int32_t edge_fd = open(getedg_str.c_str(), O_RDWR);
    if (edge_fd == -1)
        throw std::runtime_error("OPERATION FAILED: Unable to perform edge detection on GPIO"s +
                                 this->GPIONumberString);
    ::write(edge_fd, "both", 4);
    close(edge_fd);

    int32_t val_fd = open(getval_str.c_str(), O_RDONLY);
    if (val_fd == -1)
        throw std::runtime_error("OPERATION FAILED: Unable to perform edge detection on GPIO"s +
                                 this->GPIONumberString);
    pollfd pollData;
    pollData.fd = val_fd;
    pollData.events = POLLPRI | POLLERR;
    pollData.revents = 0;
    char buffer{'\0'};

    ::read(val_fd, &buffer, 1); // a dummy read is required before polling

    poll(&pollData, 1, timeout);
    lseek(val_fd, 0, SEEK_SET);
    ::read(val_fd, &buffer, 1);
    pollData.~pollfd();
    close(val_fd);

    dEdgeInterruption(getedg_str);

    return ((buffer - '0') ? GPIO_EDGE_RISING : GPIO_EDGE_FALLING);
}

void GPIO::setDirection(GPIODirection direction)
{
    if (direction > 2)
        throw std::runtime_error("OPERATION FAILED: Unable to set direction of GPIO"s +
                                 this->GPIONumberString +
                                 " (invalid direction value received).");

    if (accessMode == GPIO_MODE_SYSFS)
    {
        FILE *setdirgpio = fopen(setdir_str.c_str(), "w");
        if (!setdirgpio)
        {
            auto _clk = CHRONO_NOW;
            while (CHRONO_NOW - _clk < this->timeOut)
            {
                setdirgpio = fopen(setdir_str.c_str(), "w");
                if (setdirgpio)
                    break;
                usleep(10000); // 10ms
            };
            if (!setdirgpio)
                throw std::runtime_error("OPERATION FAILED: Unable to set direction of GPIO"s +
                                         this->GPIONumberString);
        }
        string direction_str{""};
        switch (direction)
        {
        case GPIO_OUTPUT_INIT_HIGH:
            direction_str = "high"s;
            break;
        case GPIO_OUTPUT_INIT_LOW:
            direction_str = "low"s;
            break;
        case GPIO_INPUT:
            direction_str = "in"s;
            break;
        default:
            break;
        }
        fwrite(direction_str.c_str(), 1, direction_str.size(), setdirgpio);
        fclose(setdirgpio);
        this->curDirection = direction;
        return;
    }
    else if (accessMode == GPIO_MODE_DIRECT)
    {
        INP_GPIO(this->GPIONumber);
        switch (direction)
        {
        case GPIO_INPUT:
            return;
            break;
        case GPIO_OUTPUT_INIT_LOW:
            OUT_GPIO(this->GPIONumber);
            GPIO_CLR = 1 << this->GPIONumber;
            break;
        case GPIO_OUTPUT_INIT_HIGH:
            OUT_GPIO(this->GPIONumber);
            GPIO_SET = 1 << this->GPIONumber;
            break;
        default:
            break;
        }
        this->curDirection = direction;
        return;
    }
    else
        throw std::runtime_error("OPERATION FAILED: Unable to determine access mode for GPIO"s +
                                 this->GPIONumberString);
}

GPIODirection GPIO::getDirection() { return this->curDirection; }

inline void GPIO::write(bool value)
{
    if (this->curDirection == GPIO_INPUT)
        throw std::runtime_error("OPERATION FAILED: Unable to set the value of GPIO"s +
                                 this->GPIONumberString +
                                 " (it is not configured to be an output).");

    if (accessMode == GPIO_MODE_SYSFS)
    {
        FILE *setvalgpio = fopen(setval_str.c_str(), "w");
        if (!setvalgpio)
        {
            auto _clk = CHRONO_NOW;
            while (CHRONO_NOW - _clk < this->timeOut)
            {
                setvalgpio = fopen(setval_str.c_str(), "w");
                if (setvalgpio)
                    break;
                usleep(10000); // 10ms
            }
            if (!setvalgpio)
                throw std::runtime_error("OPERATION FAILED: Unable to set the value of GPIO"s +
                                         this->GPIONumberString);
        }
        fwrite((value) ? "1" : "0", 1, 1, setvalgpio);
        fclose(setvalgpio);
        return;
    }
    else
    {
        if (value)
            GPIO_SET = 1 << this->GPIONumber;
        else
            GPIO_CLR = 1 << this->GPIONumber;
        return;
    }
}

inline bool GPIO::read()
{
    if (accessMode == GPIO_MODE_SYSFS)
    {
        FILE *getvalgpio = fopen(getval_str.c_str(), "r");
        if (!getvalgpio)
        {
            auto _clk = CHRONO_NOW;
            while (CHRONO_NOW - _clk < this->timeOut)
            {
                getvalgpio = fopen(getval_str.c_str(), "r");
                if (getvalgpio)
                    break;
                usleep(10000); // 10ms
            }
            if (!getvalgpio)
                throw std::runtime_error("OPERATION FAILED: Unable to get the value of GPIO"s +
                                         this->GPIONumberString);
        }
        char buffer{'\0'};
        fread(&buffer, 1, 1, getvalgpio);
        fclose(getvalgpio);
        if (buffer == '0')
            return false;
        else if (buffer == '1')
            return true;
        else
            throw std::runtime_error("OPERATION FAILED: Unable to get the value of GPIO"s +
                                     this->GPIONumberString);
    }
    else
    {
        return GET_GPIO(this->GPIONumber);
    }
}

int32_t GPIO::getNumber() { return this->GPIONumber; }

#endif /* !defined(__cplusplus) || __cplusplus < 201300L */

#endif /* _GPIO_HPP_ */
