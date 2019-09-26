# Apple 1 Mini

*This build is currently in progress*

This project is a minified replica of the [**Apple 1**](https://en.wikipedia.org/wiki/Apple_I).  
It's built around a 6502 CPU, 6821 PIA (Peripheral Interface Adapter), RAM and ROM - just like the original Apple 1, but it uses an Arduino Nano to enable serial communication between the Apple 1 board and an external computer that acts as terminal.  
That means you can't hook up a keyboard and monitor directly to the board (yet!).  

The project is heavily inspired by the [*Briel Computers Apple 1 Replica*](http://www.brielcomputers.com/wordpress/?cat=17) and [*The Ben Heck Show*](https://github.com/thebenheckshow/158-tbhs-apple-1-replica)!

**What I have done:**
- [Redesigned the circuit](https://github.com/DutchMaker/Apple-1-Mini/blob/master/design/DesignSpark/revision1/schematic%20-%20Schematic.pdf) so I would fully understand every aspect of it.
- Designed a PCB that can be easily produced and included in a **modular DIY kit** for anyone that wants to build this at home.
- Programmed the ROM so it only contains BASIC, Krusader (assembler) and the Woz Monitor (memory monitor).
- Written firmware for the Arduino to enable serial communication in a way that gives the real *Apple 1 feeling*.
- Written a simple utility to upload data to the computer. This proved to be difficult with existing tools as there need to be specific delays in sending the data.

**What I have planned:**
- Design an expansion board that allows you to connect a keyboard an monitor directly to the board and run the Apple 1 standalone. The main PCB has an expansion header that allows an expansion board to be put on top of it (like a breakout board).
- Design a modular casing with laser cut parts. The casing will house the main board, but additional layers can be put on top of it to increase its size and allow for expansion boards to be added later on.
- Release several varations of a DIY kit for this project (PCB only, PCB + passives, PCB + passives + casing, complete kit, etc.) so others can easily build the computer at home.

**Pictures of revision 1:**  

<img src="https://github.com/DutchMaker/Apple-1-Mini/raw/master/docs/revision1.jpg" width="450" />  
<img src="https://github.com/DutchMaker/Apple-1-Mini/raw/master/docs/screenshot.png" width="450" />  
<img src="https://github.com/DutchMaker/Apple-1-Mini/raw/master/docs/running_basic.png" width="240" />
