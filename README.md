## PIO

In this branch I mess with pio. PIO is essentially programming state machines to make certain communication protocols that are not already builtin with the RP2350. One of these communication protocols is **I2S** which we will be using for our project.

### I2S
I2S is a communication protocol for speifically audio information. The PCM 5102A DAC we are using uses this protocol in order to convert digital values into analog ones.

### PIO  squarewave
In order to make a simple squarewave using PIO, we need to write pio assembly in order to tell our state machine what to do. Then we compile the assembly code into a header file so that our main c program can easily reference the assembly. Interesting thing to note, is that on Mac the pio assembler is **not** already built so you have to manually build it. For windows it's probably already built IDK tho.

#### Building PIO assembler for MacOS
First locate the pioasm directory:

```
cd ~/.platformio
find . -iname "pioasm"
```
The output of the find command should list the directory where your pioasm code files. It should look something like this:
```
./packages/framework-picosdk/tools/pioasm
```

To build the pio assembler (pioasm) from scratch do the following commands:
```
cd ./packages/framework-picosdk/tools/pioasm
mkdir build
cd build
cmake -DPIOASM_VERSION_STRING="2.2.0" ..
make
```
Your pioasm executable should now be built in the build folder!
<br />
<br />
**Compiling a PIO assembly file** <br /> <br />
Now that your pioasm is compiled, you can compile the pio assembly file by simply running. Make sure your pioasm executable file is in path or this command won't work. 

```
pioasm squarewave.pio squarewave.pio.h 
```