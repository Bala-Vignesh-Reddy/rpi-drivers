On embedded systems, devices are often not connected through a bus, allowing enumeration
or hotplugging for these devices..Such devices, instead of being dynamically detected, 
must be statically described described by using two different methods:

 - By direct instantiation of platform_device structures, as done on a few old ARM
   non-Device Tree based platforms. The definition is done in the board-specific or SoC
   specific code.
 - In the Device Tree, a hardware description file used on some architectures. The device
   driver matches with the physical devices described in the .dts file. After this matching, the
   driver´s probe() function is called. An .of_match_table has to be included in the driver code
   to allow this matching.

UART controllers, Ethernet controllers, SPI controllers, graphic or audio devices, etc. 
In the Linux kernel, a special bus, called the platform bus, 

- The probe() function is called when the "bus driver" pairs the "device" to the "device driver". 
  The probe() function is responsible for initializing the device and registering it in the 
  appropriate kernel framework
- The suspend() and resume() functions are used by devices that support low power management features.

- The platform driver responsible for the platform device should be registered to the platform 
  core by using the platform_driver_register() function.
- module_platform_driver() macro. This is a helper macro for drivers, this eliminates a lot of boilerplate 

When the kernel module is loaded, the platform device driver registers itself with the platform 
bus driver by using the platform_driver_register() function. The probe() function is called when 
the platform device driver matches the value of one of its compatible char strings (included in 
one of its of_device_id structures) with the compatible property value of the DT device node. 
The process of associating a device with a device driver is called binding

###Interaction with hardware
- pin represents a physical input or output carrying an electrical signal.
- even though a single pin can only perform one function at a time, it can be configured internally
  to perform different functions, known as pin multiplexing

- in old linux pin muxing code, each arch had its own pin-muxing code with specific API, the pin-muxing
  had to be done at SoC level, and it couldn't requested by device drivers.
- to overcome Pinctrl subsytem is used.. it is implemented in drivers/pinctrl/ in kernel source tree
  - Pinctrl provides an API to register a pinctrl driver
  - An API for device drivers to request the muxing of a certain set of pins.
  - Interaction with the GPIO drivers of the SoC.


Pinctrl subsystem in Linux deals with:
• Enumerating and naming controllable pins.
• Multiplexing of pins. In the SoC, several pins can also form a pin group for a specific
  function. The pin control subsystem must manage all pin groups.
• Configuration of pins, such as software-controlled biasing, and driving mode specific pins,
  such as pull-up/down, open drain, load capacitance, etc.

- pinctrl_dev structure will be used to instantiate a pin controller and register a descriptor
  to the pinctrl subsystem
- The pinctrl_desc structure (declared in include/linux/pinctrl/pinctrl.h
  in the kernel source tree) contains the pin definition, the pin configuration operation interface,
  the pin muxing operation interface, and the pin group operation interface supported by the Pin
  Controller of a SoC.
- struct pin_desc. This structure is mainly used to record the use count of the pin and describes 
  the current configuration information of the pin (function and group information the pin currently belongs to).


- Few imp fields of pinctrl_desc strcutre:
  - struct pinctrl_ops - global pin control operations, to be implemented by pin controller drivers.
  - struct pinconf_ops - pin config operations, to be implemented by pin configuration capable drivers
  - struct pinmux_ops - pinmux operations, to be implemented by pin controller drivers that support pinmuxing

- struct pinctrl_desc is registered against the Pinctrl subsystem by calling the devm_pinctrl_register() 
  function (defined in drivers/pinctrl/core.c in the kernel source tree), called in the probe() function of the pinctrl driver
- devm_pinctrl_register() function calls pinctrl_register(), which is mainly divided into two
  functions: pinctrl_init_controller() and pinctrl_enable()

- The pinctrl_init_controller() function is used to describe the information of a Pin Controller,
  including the description of its supported pins, its pin multiplexing operation interfaces and its
  group related pin configuration interfaces.
- The pinctrl_init_controller() function will create a struct pin_desc for each pin and add it to the 
  cardinal tree pinctrl_dev->pin_desc_tree. It will also check whether the callback operations in the 
  pinmux_ops, pinctrl_ops and pinconf_ops variables are defined and do legality detection.
- The pinctrl_enable function mainly adds the pinctrl device to the pinctrldev_list. At this time, the
  registration of the pinctrl device driver is basically completed.


##GPIO controller driver
- In the Linux kernel, the GPIO subsystem is implemented by GPIOlib. GPIOlib is a framework that
  provides an internal Linux kernel API for managing and configuring GPIOs, acting as a bridge
  between the Linux GPIO controller drivers and the Linux GPIO client drivers.
- GPIOlib subsystem has produced two different mechanisms for gpio management, one is gpio num
  other is based on gpio_desc descriptor form
- Inside each GPIO controller driver, individual GPIOs are identified by their hardware number,
  which is a unique number between 0 and n, n being the number of GPIOs managed by the chip.
  This number is only internal to the driver

- The main structure of a GPIO controller driver is struct gpio_chip (declared in include/linux/gpio/driver.h 
  in the kernel source tree), which includes GPIO operations specific to each GPIO controller of a SoC:
  • Methods to establish GPIO line direction.
  • Methods used to access GPIO line values.
  • Method to set the electrical configuration of a given GPIO line.
  • Method to return the IRQ number associated with a given GPIO line.
  • Flag saying whether calls to its methods may sleep.
  • Optional line names array to identify lines.
  • Optional debugfs dump method (showing extra state like pullup config).
  • Optional base number (will be automatically assigned if omitted).
  • Optional label for diagnostics and GPIO chip mapping using platform data.






