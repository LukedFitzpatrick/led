#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "buffer.h"
#include "term_setup.h"
#include "editor.h"

// TODO make backspace work
    
int main(int argc, char* argv[])
{
    TermSetup t;

    Editor led;
    Buffer buf("", 1, t.mNumCols, t.mNumRows);

    // open a file
    if(argc > 1)
    {
        bool success = buf.OpenFile(std::string(argv[1]));
        if(!success)
        {
            buf.MakeFile(std::string(argv[1]));
        }
    }



    led.AddBuffer(&buf);
    led.mCurrBuffer = &buf;        

    // does all the terminal setup/teardown in the constructor/destructor

    
    bool done = false;
    while(!done)
    {
        led.DrawScreen();
        int c = led.ReadKey();
        done = led.HandleKey(c);
    }

    (void) led;
    (void) done;
    return 0;
}
