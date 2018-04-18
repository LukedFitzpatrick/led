#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "buffer.h"
#include "term_setup.h"
#include "editor.h"
    
int main(int argc, char* argv[])
{
    TermSetup t;

    Editor led;

    Buffer buf("", 1, t.mNumCols, t.mNumRows);
    
    // open files
    if(argc > 1)
    {
        if(!buf.OpenFile(std::string(argv[1])))
        {
            return 1;
        }
        led.AddBuffer(&buf);
        led.mCurrBuffer = &buf;        
    }
    else
    {
        led.AddBuffer(&buf);
        led.mCurrBuffer = &buf;        
    }

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
