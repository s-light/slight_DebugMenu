/******************************************************************************

    slight_DebugMenu Library
        helper library to build a simple debug menu for the stream_out terminal.

    written by stefan krueger (s-light),
        github@s-light.eu, http://s-light.eu, https://github.com/s-light/

******************************************************************************/
/******************************************************************************
    The MIT License (MIT)

    Copyright (c) 2015 Stefan Krüger

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    stream_out OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
******************************************************************************/


// Include yourselfs header file
#include "slight_DebugMenu.h"

// Includes Core Arduino functionality
#if ARDUINO < 100
    #include <WProgram.h>
#else
    #include <Arduino.h>
#endif


// slight_DebugMenu :: slight_DebugMenu(
//     Print &out_ref
// ) {
//     ready = false;
//     stream_out = out_ref;
// }

// slight_DebugMenu :: slight_DebugMenu(
//     Stream &inout_ref,
//     uint8_t input_length_new
// ) {
//     slight_DebugMenu(inout_ref, inout_ref, input_length_new);
// }

slight_DebugMenu :: slight_DebugMenu(
    Stream &stream_input_ref,
    Print &stream_out_ref,
    uint8_t input_length_new
) :
    stream_input(stream_input_ref),
    stream_out(stream_out_ref)
{
    ready = false;
    // in = in_ref;
    // stream_out = out_ref;
    input_length = input_length_new;
    command_input = (char *)malloc(input_length);
    command_current = (char *)malloc(input_length);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// functions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// public

void slight_DebugMenu::begin() {
    if (ready == false) {
        memset(command_input, '\0', input_length);
        memset(command_current, '\0', input_length);

        flag_BF = false;  // BufferFull
        flag_EOL = false;
        flag_CR = false;
        flag_LF = false;
        // flag_LongLine = false;
        // flag_SkipRest = false;

        ready = true;
    }
}

void slight_DebugMenu::begin(bool print_on_start) {
    if (print_on_start) {
        begin();
        command_input[0] = '?';
        flag_EOL = true;
        update();
    }
}

void slight_DebugMenu::update() {
    if (ready) {
        handle_input_available();
        check_EOL();
    }
}


void slight_DebugMenu::set_callback(callback_t callback_on_EOL_new) {
    callback_on_EOL = callback_on_EOL_new;
}


bool slight_DebugMenu::get_flag_BF() {
    return flag_BF;
}

void slight_DebugMenu::set_flag_EOL(bool flag = true) {
    flag_EOL = flag;
}

Print& slight_DebugMenu::get_stream_out_ref() {
    return stream_out;
}


char* slight_DebugMenu::get_command_input_pointer() {
    return command_input;
}

char* slight_DebugMenu::get_command_current_pointer() {
    return command_current;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// public static

void slight_DebugMenu::print_Binary_8(Print &stream_out, uint8_t value)  {
    for (size_t mask = 0b10000000; mask; mask >>= 1) {
        // check if this bit is set
        if (mask & value) {
            stream_out.print('1');
        } else {
            stream_out.print('0');
        }
    }
}

void slight_DebugMenu::print_Binary_12(Print &stream_out, uint16_t value)  {
    //                       B12345678   B12345678
    // for (unsigned int mask = 0x8000; mask; mask >>= 1) {
    for (size_t mask = 0b100000000000; mask; mask >>= 1) {
        // check if this bit is set
        if (mask & value) {
            stream_out.print('1');
        } else {
            stream_out.print('0');
        }
    }
}

void slight_DebugMenu::print_Binary_16(Print &stream_out, uint16_t value)  {
    for (size_t mask = 0b1000000000000000; mask; mask >>= 1) {
        // check if this bit is set
        if (mask & value) {
            stream_out.print('1');
        } else {
            stream_out.print('0');
        }
    }
}

void slight_DebugMenu::print_uint8_align_right(
    Print &stream_out,
    uint8_t value
) {
    // uint8_t bOffset = 0;
    if (value < 100) {
        if (value < 10) {
            // bOffset = 2;
            stream_out.print(F("  "));
        } else {
            // bOffset = 1;
            stream_out.print(F(" "));
        }
    }
    stream_out.print(value);
}

void slight_DebugMenu::print_uint8_array(
    Print &stream_out,
    uint8_t *array,
    size_t count
) {
    stream_out.print(F(" "));
    uint8_t index = 0;
    print_uint8_align_right(stream_out, array[index]);
    for (index = 1; index < count; index++) {
        stream_out.print(F(", "));
        print_uint8_align_right(stream_out, array[index]);
    }
}





//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// private


void slight_DebugMenu::handle_input_available() {
    // collect next input text
    while ((!flag_EOL) && (stream_input.available())) {
        // get the new byte:
        char charNew = (char)stream_input.read();
        /*stream_out.print(F("charNew '"));
        stream_out.print(charNew);
        stream_out.print(F("' : "));
        stream_out.println(charNew, DEC);*/

        // collect next full line
        // http://forums.codeguru.com/showthread.php?253826-C-String-What-is-the-difference-between-n-and-r-n
        //     '\n' == 10 == LineFeed == LF
        //     '\r' == 13 == Carriage Return == CR
        //     Windows: '\r\n'
        //     Linux: '\n'
        //     Apple: '\r'
        //
        // check for line end
        switch (charNew) {
            // case '\0': {
            //     stream_out.println(F("incoming char is \\0: set EOL"));
            //     flag_EOL = true;
            //     // flag_CR = false;
            //     // flag_LF = false;
            // } break;
            case '\r': {
                // stream_out.println(F("incoming char is \\r (CR): set EOL"));
                flag_EOL = true;
                flag_CR = true;
                // flag_LF = false;
            } break;
            case '\n': {
                // stream_out.println(F("incoming char is \\n (LF): set EOL"));
                // stream_out.println(F("  Flags:"));
                //     stream_out.print  (F("    flag_BF: '"));
                //     stream_out.println(flag_BF);
                //     stream_out.print  (F("    flag_CR: '"));
                //     stream_out.println(flag_CR);
                //     stream_out.print  (F("    flag_LF: '"));
                //     stream_out.println(flag_LF);
                //     stream_out.print  (F("    flag_EOL: '"));
                //     stream_out.println(flag_EOL);

                flag_LF = true;
                flag_EOL = true;

                // stream_out.println(F("  check for CR"));
                // check if last char was not CR -
                // if this is true set EOL - else ignore.
                // if(!flag_CR) {
                //     flag_EOL = true;
                // } else {
                //     flag_CR = false;
                // }
                //
                // stream_out.println(F("  Flags:"));
                //     stream_out.print  (F("    flag_BF: '"));
                //     stream_out.println(flag_BF);
                //     stream_out.print  (F("    flag_CR: '"));
                //     stream_out.println(flag_CR);
                //     stream_out.print  (F("    flag_LF: '"));
                //     stream_out.println(flag_LF);
                //     stream_out.print  (F("    flag_EOL: '"));
                //     stream_out.println(flag_EOL);

                // this check also works for windows double line ending
                // if (strlen(command_input) > 0) {
                //     flag_EOL = true;
                // }
            } break;
            default: {
                // normal char -
                // add it to the input:
                // stream_out.print(F("input char '"));
                // stream_out.print(charNew);
                // stream_out.println(F("'"));

                // stream_out.print(F("sizeof(command_input): "));
                // stream_out.print(sizeof(command_input));
                // stream_out.println();

                // check for length
                if (strlen(command_input) < (input_length-1)) {
                    command_input[strlen(command_input)] = charNew;
                } else {
                    // stream_out.println(F(
                    //     " line to long! ignoring rest of line"
                    // ));
                    // set complete flag so line will be parsed
                    // stream_out.println(F(
                    //     "Buffer is full: set EOL; set LongLine"
                    // ));
                    // flag_EOL = true;
                    flag_BF = true;
                    // skip rest of line
                    // flag_LongLine = true;
                }
            }  // case default
        }  // switch charNew
    }  // while input.available()
}

void slight_DebugMenu::check_EOL() {
    // complete line found:
    if (flag_EOL) {
        // Serial.println(F("check_EOL:"));
        // Serial.print(F("    command_input: '"));
        // Serial.print(command_input);
        // Serial.println(F("'"));

        // Serial.println(F("  Flags:"));
        // Serial.print  (F("    flag_BF: '"));
        // Serial.println(flag_BF);
        // Serial.print  (F("    flag_CR: '"));
        // Serial.println(flag_CR);
        // Serial.print  (F("    flag_LF: '"));
        // Serial.println(flag_LF);
        // Serial.print  (F("    flag_EOL: '"));
        // Serial.println(flag_EOL);


        // Serial.println(F("  copy command_input to command_current."));
        // copy to current buffer
        strcpy(command_current, command_input);

        // Serial.println(F("  clear command_input"));
        // reset memory
        memset(command_input, '\0', input_length-1);

        // Serial.println(F("  clear flag_EOL"));
        // reset flag
        flag_EOL = false;
        flag_LF = false;
        flag_CR = false;

        // parse line / run command
        if (callback_on_EOL) {
            callback_on_EOL(this);
        }

        // reset BufferFull
        flag_BF = false;

        // print info if things were skipped.
        // if (flag_BF) {
        //     stream_out.println(F(
        //         "input was to long. used first part - skipped rest."
        //     ));
        //     flag_BF = false;
        // }



        // Serial.print  (F("    command_input: '"));
        // Serial.print(command_input);
        // Serial.println(F("'"));
        // Serial.print  (F("    command_current: '"));
        // Serial.print(command_current);
        // Serial.println(F("'"));


        // // Serial.println(F("  check flag_SkipRest"));
        // if ( !flag_SkipRest) {
        //     // Serial.println(F("   parse Line:"));
        //
        //     if (flag_BF) {
        //         Serial.println(F("input was to long. used first part - skipped rest."));
        //         flag_BF = false;
        //     }
        //
        //     // parse line / run command
        //     menuSwitcher(Serial, command_current);
        //
        //     if(flag_LongLine) {
        //         flag_SkipRest = true;
        //         flag_LongLine = false;
        //     }
        // } else {
        //     // Serial.println(F("   skip rest of Line"));
        //     flag_SkipRest = false;
        // }

    }// if Flag complete
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DummyWriter
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

DummyWriter::DummyWriter() {
    // nothing to do.
}

size_t DummyWriter::write(uint8_t data) {
    // data is send to nirvana :-)
    return 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// THE END
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
