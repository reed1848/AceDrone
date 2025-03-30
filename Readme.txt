********************************************************
Overview
********************************************************

This bank contains basic C and Ada applications which send/receive Queuing and
Sampling messages.

The C source is located in Applications/helloWorld_app.
The Ada source is located in Applications/MessagePrinter.

********************************************************
Building the system
********************************************************

To build this example, execute "make release" from a command window within this 
example's root directory.

The top-level makefile invokes the ADE configuration generator to create 
PartitionConfig.cfg in the bank directory based on the Module XML file in 
ConfigData. It then builds the application, placing the resulting executable in 
the bank directory.

********************************************************
Loading/Running the system
********************************************************

To run this example, use AAPLL to load helloworld-Bank then click start.

********************************************************
Expected Output
********************************************************

Output may be viewed using ACD.

<<< Partition 1 >>>
Hello World

<<< Partition 2 >>>
Hello Queuing!
Hello Sampling!