/************************************************************************
    slight_DebugMenu Library
        for more information changelog / history see slight_DebugMenu.cpp
    written by stefan krueger (s-light),
        git@s-light.eu, http://s-light.eu, https://github.com/s-light/
*************************************************************************/
/************************************************************************
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
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*************************************************************************/

// include guard
#ifndef SLIGHT_DEBUGMENU_H_
#define SLIGHT_DEBUGMENU_H_

// Includes Core Arduino functionality
#include <Arduino.h>

class slight_DebugMenu {
 public:
    // slight_DebugMenu();
    // slight_DebugMenu(Stream &outin_ref, uint8_t input_length_new);
    slight_DebugMenu(
        Stream &stream_input_ref,
        Print &stream_out_ref,
        uint8_t input_length_new
    );

    // callback
    typedef void (* callback_t) (slight_DebugMenu  *pInstance);


    void begin();
    void begin(bool print_on_start);

    void update();

    // bool get_flag_SkipRest();
    bool get_flag_BF();
    void set_flag_EOC(bool flag);

    void set_user_EOC_char(char user_EOC_char_new);
    char get_user_EOC_char();
    void clear_user_EOC_char();

    char* get_command_input_pointer();
    char* get_command_current_pointer();

    Print& get_stream_out_ref();

    void set_callback(callback_t callback_on_EOC_new);




    static void print_Binary_8(
        Print &stream_out, uint8_t value);
    static void print_Binary_12(
        Print &stream_out, uint16_t value);
    static void print_Binary_16(
        Print &stream_out, uint16_t value);
    static void print_Binary_32(
        Print &stream_out, uint32_t value);

    static void print_HEX(
        Print &stream_out, uint8_t value);

    static uint8_t parse_HEX(char *input);


    static uint8_t count_digits(
        uint32_t value);

    // TODO: find smooth way to extract align right thing..
    // static void print_align_right(
    //     Print &stream_out, uint8_t value, uint8_t padding_width);
    // --> use templates!!!!

    static void print_uint8_align_right(
        Print &stream_out, uint8_t value);
    static void print_uint16_align_right(
        Print &stream_out, uint16_t value);
    static void print_uint32_align_right(
        Print &stream_out, uint32_t value);

    static void print_int8_align_right(
        Print &stream_out, int16_t value);
    static void print_int16_align_right(
        Print &stream_out, int16_t value);
    static void print_int32_align_right(
        Print &stream_out, int32_t value);

    #if defined(ARDUINO_ARCH_AVR)
        // AVR-specific code
        static int8_t print_float_align_right(
          Print &stream_out, double value,
          int8_t total_width, uint8_t precission, int8_t clip = 0);
    #elif defined(ARDUINO_ARCH_SAM)
      // SAM-specific code
    #else
      // generic, non-platform specific code
    #endif

    static void print_uint8_array(
        Print &stream_out, uint8_t *array, size_t count);
    static void print_int8_array(
        Print &stream_out, int8_t *array, size_t count);
    static void print_uint8_array(
        Print &stream_out, char *array, size_t count);
    static void print_uint8_array_HEX(
        Print &stream_out, uint8_t *array, size_t count);
    static void print_uint16_array(
        Print &stream_out, uint16_t *array, size_t count);
    static void print_int16_array(
        Print &stream_out, int16_t *array, size_t count);


    static void print_MAC(
        Print &stream_out, uint8_t *array);
    static void print_IP(
        Print &stream_out, uint8_t *array);


 protected:
        bool ready;

        Stream &stream_input;
        Print &stream_out;

        char *command_input;
        char *command_current;

        bool user_EOC_char_active;
        char user_EOC_char;

        uint8_t input_length;

        bool flag_BF;  // BufferFull
        bool flag_EOC;  // End of Command
        bool flag_CR;
        bool flag_LF;
        bool flag_USER;
        // bool flag_LongLine;
        // bool flag_SkipRest;

        callback_t callback_on_EOC;

        void handle_input_available();
        void check_EOC();
};

class DummyWriter : public Print{
 public:
    DummyWriter();
    size_t write(uint8_t data);
};

#endif  // SLIGHT_DEBUGMENU_H_
