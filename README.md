# PROGMEM: 8266 ESP-01 Meets The Canterbury Tales
Load a very long, 700-year-old poem onto an ESP-01 8266-based module. Minimize the impact on RAM by storing the strings in flash memory.

By David "IowaDave" Sparks

The data structure for this project consists of 903 distinctly named strings, one for each line of the poem, plus an array of 903 pointers to the strings. It occupies more than 43,000 bytes of flash memory. Yet, it requires fewer than 200 bytes of RAM to access and print out all of that text.

### PROGMEM for 8266 and Arduino IDE
Strings take up room in memory. By default, the Arduino IDE's compiler may place the bytes representing a string's characters into RAM, for declarations like the following:

<pre><code style="font-family: monospace; ">
const char ramHog[] = "This string will consume fifty-nine bytes of precious RAM!";
</code></pre>

It will do the same thing with strings defined inline with various code statements, such as this:

<pre><code style="font-family: monospace; ">
  Serial.println("Here is another string that could pitch its tent in your RAM!");
</code></pre>

For strings like those, which your code will never change, RAM might not be the best storage location. 

The 8266 hardware provides 80 KiB (81,920 bytes) of dynamic RAM for program variables. RAM is best used for values that need to change. Flash memory for program storage is much larger. Rercent versions of ESP-01 modules provide almost one megabyte of flash memory. The PROGMEM modifier instructs the compiler to store strings that never change in the larger flash memory, rather than in the RAM.

### PROGMEM Documentation Online -- not *quite* perfect!
Links listed below connect to web sites and PDFs that explain PROGMEM in detail. They give examples that really reward close inspection. One detail needs modification for use with 8266, as explained below in *Dereferencing PROGMEM pointers for 8266*.

[Expressif's official PDF](https://www.espressif.com/sites/default/files/documentation/save_esp8266ex_ram_with_progmem_en.pdf) :: I believe this reference is authoritative and accurate, as far as it goes. It does not address how to work with arrays of strings, however. We found that in the next source.

[Arduino.cc PROGMEM reference](https://www.arduino.cc/reference/en/language/variables/utilities/progmem/) :: This also impresses me as authoritative -- when writing code for Arduinos! 

In addition to the detail mentioned above, there are some differences of syntax between the examples in the Arduino.cc reference compared to corresponding examples in the Expressif document. These differences might not affect the result. The discussion that follows illustrates examples of the syntax from both of the references. 

### Construct an array of strings for PROGMEM with 8266

**Declare the strings**

The example given in the Arduino.cc reference is shown below. Notice how the text of the string is delimited by both curly brackets and quotes.

<pre><code style="font-family: monospace; ">
/* Arduino.cc reference example *
const char variableName[] PROGMEM = {"This string is enclosed within both quotes and curly brackets."};
</code></pre>

The Expressif documentation for 8266 further qualifies the string variable as *static*, but does not use the curly brackets:

<pre><code style="font-family: monospace; ">
/* Expressif reference example */
static const char variableName[] PROGMEM = "This string is enclosed only in quotes.";
</code></pre>

Comment: both of these "official" examples appear to compile successfully. I found also that omitting both the curly brackets and the "static" token produces compact code that compiles successfully:

<pre><code style="font-family: monospace; ">
/* IowaDave example */
const char variableName[] PROGMEM = "This string is enclosed only in quotes.";
</code></pre>

**Declare the array**

Only the Arduino.cc reference gives an example for declaring an array of string pointers to be stored in flash memory.  I modified the example slightly for brevity and clarity.

<pre><code style="font-family: monospace; ">
/*
 * Arduino.cc reference example
 */
// Setting up the strings is a two-step process. First, define the strings.
const char string_0[] PROGMEM = "Store me in flash.";
const char string_1[] PROGMEM = "Also store me in flash.";
const char string_2[] PROGMEM = "Hey! Store me in flash, too!";

// Then set up a table to refer to the strings.
const char *const string_table[] PROGMEM = {string_0, string_1, string_2};
</code></pre>

The formulation of the declaration in the Arduino.cc example looks strange to me. Why does the token, "const", appear twice? It seems at best unnecessary. By trial and error, I found that a shorter, alternate form also compiles and functions correctly, as shown below. 

<pre><code style="font-family: monospace; ">
/*
 * 8266 working example
 */
// Define the strings
const char string_0[] PROGMEM = "Store me in flash";
const char string_1[] PROGMEM = "Also store me in flash";
const char string_2[] PROGMEM = "Hey! Store me in flash, too!";

// The second "const" token in the table's declaration was omitted, like this:
const char * string_table[] PROGMEM = {string_0, string_1, string_2};
</code></pre>

I prefer the alternate form because it seems clear and complete. The sketch in this repository follows the alternate syntax. Readers may certainly adhere closely to the Arduino.cc reference if they prefer. It would be the first trouble-shooting step to try in the event my alternate format fails to compile.

## Retrieve strings from flash memory for further processing
Attempts to access directly the strings stored in flash memory can crash the 8266 program. The references explain why. I won't repeat the reasons here. Special methods must be used to access strings stored in flash.

The method I used copies the strings -- one at a time -- into a small buffer located in RAM. The longest line in the poem has fewer than 60 characters. Therefore, a single, c-type string having 60-character capacity can serve as the buffer, as follows:

<pre><code style="font-family: monospace; ">
// Define a string buffer in RAM
char stringBuf[60];
</code></pre>

This is magic. The sketch declares 903 strings, one for each line in the poem. It declares an array of 903 pointers to the strings. All of that text -- almost 40,000 bytes -- is stored in flash memory. Then it takes only 60 bytes of RAM to process the text.

**Pseudo-code for this part**

Print the strings by looping through the array of pointers.
* dereference the pointer
* copy the string it points to, into the buffer
* print the buffer

In real code, both the dereferencing and the copying are handled within a single statement. 

<pre><code style="font-family: monospace; ">
// de-reference and type-cast the pointer, then move its target into the buffer
strcpy_P(stringBuf, (char*)pgm_read_dword(&(string_table[i])));
</code></pre>

**Dereferencing PROGMEM pointers for 8266**

The pointers in the array are 32-bit values, also known as double-words, or dwords. They must be retrieved from the flash memory then re-cast as pointers (i.e., memory addresses) before the sketch can access the text stored at the corresponding address.

I join with the Arduino.cc documentation, in admitting that the dereferencing step looks mysterious. Do as they recommend: just copy it. 

***By the way...***

Look closely at the Arduino.cc reference and you will see an example that assumes the pointer variables are of "word"-size. However, as mentioned previously, the pointers for 8266 are of a larger, "dword" size. *This is one detail found in the documentation that you must modify for use with the 8266*. *Replace "word" with "dword"*.

**Copy the string from flash memory into the buffer**

Copying the string uses a special instruction, strcpy_P(). The "_P" suffix signifies that it is part of the PROGMEM set of special functions that are designed to access data from flash memory without crashing the 8266. The documentation provides a list and explains the different functions.

strcpy_P() is safe to use if you exercise care to declare a buffer that is larger than the longest string that will be copied into it. Ensuring the safety of this operation is the programmer's job.

### Prepare the text
I chose the Chaucer poem because its Middle English vernacular is rather charming and its length affords a good example for this project.

The poem is more than 700 years old and in the public domain. I found examples of it in various places on the internet, including [poetryfoundation.org](https://www.poetryfoundation.org/poems/43926/the-canterbury-tales-general-prologue) and [Project Gutenberg](https://www.gutenberg.org/files/22120/22120-h/22120-h.htm#prologue).

It needed some work to prepare the text for coding into an Arduino sketch. I had to convert some Unicode characters into ASCII substitutes, then tidy up a few other, minor things. That work is outside the scope of this article.

After the text was ready, I piped it through a short C program (makePROGMEM.c in this repository) for conversion into lines of code. The program also counts the lines, determines the length of the longest line, and prepares the declaration of the string pointer array.

This is a nice example of writing code that writes code. It would have been foolish of me to sit there and type 903 string declarations by hand. Too many mistakes! This way, all I had to do was to copy the program's output and paste it into the Arduino sketch.

### Compile and upload the sketch onto the 8266.
The sketch in this repository follows the example given in the Arduino.cc reference, modified to use Chaucer's 903-line poem for the strings and the "dword" size for the pointers.

It might amuse the reader to compare the memory-usage statistics provided by the Arduino IDE compiler for this "Chaucer" sketch to those for an empty sketch such as the following:

<pre><code style="font-family: monospace; ">
void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}
</code></pre>

The empty sketch gives the following statistics:
- *Sketch uses 255664 bytes (26%) of program storage space. Global variables use 26772 bytes (32%) of dynamic memory, leaving 55148 bytes for local variables.*

Compare those figures to the Chaucer sketch:
- *Sketch uses 299052 bytes (31%) of program storage space. Global variables use 26956 bytes (32%) of dynamic memory, leaving 54964 bytes for local variables.*

The difference, in program storage space usage, is 43,388 bytes. That quantity reflects the data structures stored in flash by the PROGMEM modifier. Perhaps it also measures the inclusion of some additional code from the 8266 core libraries, related to PROGMEM.

The difference, in RAM usage, is only 184 bytes. 

### View the output
The sketch waits 10 seconds after starting the Serial port. Then it sends the strings out via Serial. An easy place to view the output is in the Serial Monitor window of the Arduino IDE. Different 8266-based modules behave differently when switching modes, that is, when changing between uploading new firmware and executing the firmware.

* A NodeMCU-style development board with an ESP-12E or -12F might run the serial output automatically about ten seconds after the code uploads. 
* Other 8266-based modules may need to be re-set from "program" mode to "run" mode before they will run the sketch.
  * The ESP-01 module would be in a programming adapter of some type. Pull the adapter out of the computer's USB port, place the adapter in "UART" mode, then reinstall the adapter onto the computer. Open the Serial Monitor and wait a few seconds.
  * A Wemos-style module for the ESP-12F also might need a power cycle before it will start in "run" mode. Disconnect it from the USB cable then plug it back in. Open the Serial Monitor and wait a few seconds.
  

### Why use PROGMEM?
It is a reasonable question, even when a lengthy poem, as in this example, could still fit within RAM. For small numbers of short strings in simple sketches, the modest extra effort and complexity to use PROGMEM might not be worthwhile.

However, code writers designing sketches to have: 

* large numbers of strings;
* lengthy strings (such as web pages; or
* a lot of data manipulation to perform in RAM,

may wish to consider conserving precious RAM by using PROGMEM to store the strings that the sketch will only use but will never change.

**Going Beyond Const: Try Formatted PROGMEM Strings**

A string stored in flash can serve your purposes in all the usual ways after it has been moved into a RAM buffer. For example, the string can contain formatting codes for use with the Serial.printf() statement. An example of this delightful concept is available in this repository. See the file named, *PROGMEM_example_with_formatted_strings.ino*.

Happy coding!
