# data_logger_software
Contains modules to gather data from the accelerometer, up to eight analog signals (input only), and the megasquirt ECU. This is then stored onto an SD card in the form of a CSV file.

The data logger was built using platform.io in Visual Studio Code on the teensy 4.1 platform. A PCB was designed to hold the teensy and an accelerometer along with the peripherals necessary to step down power from the 12v, protect against overvoltage and protect against rversed battery polarity.

PCB: https://github.com/Washington-State-University-Formula-SAE/data_logger_pcb
