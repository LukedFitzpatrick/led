#pragma once
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>


struct TermSetup
{
    TermSetup()
    {
        // get struct with access to terminal attributes
        struct termios termAttributes;
        tcgetattr(STDIN_FILENO, &termAttributes);
        mOriginalSettings = termAttributes;

        // turn off echo, canonical mode, c-c/c-z stopping us, c-v, c-o
        // canonical mode = read byte by byte not line by line
        termAttributes.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

        // turn off c-s/c-q suspension, c-m newline
        termAttributes.c_iflag &= ~(ICRNL | IXON);

        // turn off output processing
        termAttributes.c_oflag &= ~(OPOST);

        // allow for real time loop
        termAttributes.c_cc[VMIN] = 0;
        termAttributes.c_cc[VTIME] = 1;
        
        // set terminal attributes to our changed version
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &termAttributes);

        struct winsize ws;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        mNumCols = ws.ws_col;
        mNumRows = ws.ws_row;
    }
    
    // on close, set the terminal settings back to normal
    ~TermSetup()
    {
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);        
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &mOriginalSettings);
    }

    struct termios mOriginalSettings;
    int mNumCols;
    int mNumRows;
};
