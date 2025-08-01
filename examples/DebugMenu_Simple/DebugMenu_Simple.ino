/******************************************************************************
    DebugMenu_Simple
        short description
        debugout on usbserial interface: 115200baud

    hardware:
        Board:
            Arduino compatible (with serial port)
            LED on pin 9

    written by stefan krueger (s-light),
        git@s-light.eu, http://s-light.eu, https://github.com/s-light/

    license: CC0 1.0 (Public Domain)
    https://creativecommons.org/publicdomain/zero/1.0/
******************************************************************************/

#include <slight_DebugMenu.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Info
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sketchinfo_print(Print &out) {
    out.println();
    //             "|~~~~~~~~~|~~~~~~~~~|~~~..~~~|~~~~~~~~~|~~~~~~~~~|"
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("|                       ^ ^                      |"));
    out.println(F("|                      (0,0)                     |"));
    out.println(F("|                      ( _ )                     |"));
    out.println(F("|                       \" \"                      |"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("| DebugMenu_Simple.ino"));
    out.println(F("|   Example to demonstrate the use of slight_DebugMenu..."));
    out.println(F("|"));
    out.println(F("| This Sketch has a debug-menu:"));
    out.println(F("| send '?'+Return for help"));
    out.println(F("|"));
    out.println(F("| dream on & have fun :-)"));
    out.println(F("|"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("|"));
    // out.println(F("| Version: Nov 11 2013  20:35:04"));
    out.print(F("| version: "));
    out.print(F(__DATE__));
    out.print(F("  "));
    out.print(F(__TIME__));
    out.println();
    out.println(F("|"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println();

    // out.println(__DATE__); Nov 11 2013
    // out.println(__TIME__); 20:35:04
}


// slight_DebugMenu(Stream &in_ref, Print &out_ref, uint8_t input_length_new);
slight_DebugMenu myDebugMenu(Serial, Serial, 15);

boolean infoled_state = 0;
const byte infoled_pin = 9; // D9

unsigned long liveSign_TimeStamp_LastAction = 0;
const uint16_t liveSign_UpdateInterval = 1000; // ms

boolean liveSign_Serial_Enabled = 0;
boolean liveSign_LED_Enabled = 1;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Menu System

void handleMenu_Main(slight_DebugMenu *pInstance) {
    Print &out = pInstance->get_stream_out_ref();
    char *command = pInstance->get_command_current_pointer();
    // out.print("command: '");
    // out.print(command);
    // out.println("'");
    switch (command[0]) {
    case 'h':
    case 'H':
    case '?': {
        // help
        out.println(
            F("____________________________________________________________"));
        out.println();
        out.println(F("Help for Commands:"));
        out.println();
        out.println(F("\t '?': this help"));
        out.println(F("\t 'i': sketch info"));
        out.println(F("\t 'y': toggle DebugOut livesign print"));
        out.println(F("\t 'Y': toggle DebugOut livesign LED"));
        out.println();
        out.println(F("\t 'A': Show 'HelloWorld' "));
        out.println(F("\t 'f': DemoFadeTo(ID, value) 'f1:65535'"));
        out.println(F("\t 'p': xxx(value1, value2) 'p255:65535'"));
        out.println();
        out.println(
            F("____________________________________________________________"));
    } break;
    case 'i': {
        sketchinfo_print(out);
    } break;
    case 'y': {
        out.println(F("\t toggle DebugOut livesign Serial:"));
        liveSign_Serial_Enabled = !liveSign_Serial_Enabled;
        out.print(F("\t liveSign_Serial_Enabled:"));
        out.println(liveSign_Serial_Enabled);
    } break;
    case 'Y': {
        out.println(F("\t toggle DebugOut livesign LED:"));
        liveSign_LED_Enabled = !liveSign_LED_Enabled;
        out.print(F("\t liveSign_LED_Enabled:"));
        out.println(liveSign_LED_Enabled);
    } break;
    //---------------------------------------------------------------------
    case 'A': {
        out.println(F("\t Hello World! :-)"));
    } break;
    case 'f': {
        out.print(F("\t DemoFadeTo "));
        // convert part of string to int
        // (up to first char that is not a number)
        uint8_t id = atoi(&command[1]);
        // convert single character to int representation
        // uint8_t id = &command[1] - '0';
        out.print(id);
        out.print(F(" : "));
        uint16_t value = atoi(&command[3]);
        out.print(value);
        out.println();
        // demo_fadeTo(id, value);
        out.println(F("\t demo for parsing values --> finished."));
    } break;
    case 'p': {
        out.print(F("\t parse values "));
        // convert part of string to int
        // (up to first char that is not a number)
        uint8_t touch = atoi(&command[1]);
        // convert single character to int representation
        // uint8_t id = &command[1] - '0';
        out.print(touch);
        out.print(F(" : "));
        uint8_t release = touch / 2;

        // check if second value is given.
        char *delimiter = strchr(&command[1], ',');
        if (delimiter != NULL) {
            release = atoi(delimiter + 1);
            out.print(release);
            out.println();
        } else {
            out.print(release);
            out.println();
            out.print(
                F("\t   --> no second parameter given. using 'touch/2'."));
            out.println();
        }
        // cap.setThresholds(touch, release);
        out.println("done.");
    } break;
    //---------------------------------------------------------------------
    default: {
        if (strlen(command) > 0) {
            out.print(F("command '"));
            out.print(command);
            out.println(F("' not recognized. try again."));
        }
        pInstance->get_command_input_pointer()[0] = '?';
        pInstance->set_flag_EOC(true);
    } // end default
    } // end switch
    // end Command Parser
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// other things..

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setup
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
    // initialise PINs
    pinMode(infoled_pin, OUTPUT);
    digitalWrite(infoled_pin, HIGH);

    // initialise serial
    Serial.begin(115200);
    // Wait for Serial Connection to be opened from Host or
    // timeout after 6second
    uint32_t timeStamp_Start = millis();
    while ((!Serial) && !((millis() - timeStamp_Start) > 6000)) {
        yield();
    }
    Serial.println();

    sketchinfo_print(Serial);

    // serial menu
    myDebugMenu.set_user_EOC_char(';');
    myDebugMenu.set_callback(handleMenu_Main);
    myDebugMenu.begin();

    Serial.println(F("Loop:"));
} /** setup **/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main loop
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop() {
    myDebugMenu.update();

    // debug output
    if ((millis() - liveSign_TimeStamp_LastAction) > liveSign_UpdateInterval) {
        liveSign_TimeStamp_LastAction = millis();

        if (liveSign_Serial_Enabled) {
            Serial.print(millis());
            Serial.print(F("ms;"));
        }

        if (liveSign_LED_Enabled) {
            infoled_state = !infoled_state;
            if (infoled_state) {
                // set LED to HIGH
                digitalWrite(infoled_pin, HIGH);
            } else {
                // set LED to LOW
                digitalWrite(infoled_pin, LOW);
            }
        }
    }
} /** loop **/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// THE END
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
