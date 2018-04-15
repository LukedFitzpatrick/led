#pragma once
#include <sstream>
#include <istream>
#include "buffer.h"
#include <iostream>
#include <iterator>

constexpr char CtrlKey(char k) 
{
    return ((k) & 0x1f);
}

enum SpecialKeys
{
    BACKSPACE = 127,
    KEY_UP = 1000,
    KEY_DOWN = 1001,
    KEY_LEFT = 1002,
    KEY_RIGHT = 1003
};

struct Editor
{
    // read one key from std input
    int ReadKey()
    {
        int bytesRead;
        char c;

        while((bytesRead = read(STDIN_FILENO, &c, 1)) != 1)
        {
        }
        
        if (c == '\x1b')
        {
            char seq[3];
            if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
            if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';            

            Message(std::string(seq));
            
            if (seq[0] == '[') {
                
                switch (seq[1]) {
                    case 'A': return KEY_UP;
                    case 'B': return KEY_DOWN;
                    case 'C': return KEY_RIGHT;
                    case 'D': return KEY_LEFT;
                }
            }
            return 0;

        }
        else
        {
            return c;
        }
    }

    void Message(std::string message)
    {
        mCurrBuffer->InsertLine(message);
    }
    
    void RunCommand(std::string command)
    {
        // run a user inputted command

        // split into words
        std::stringstream ss(command);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> words(begin, end);

        // TODO write a proper tokeniser + parser here
    }

    std::vector<Buffer*> mBuffers = {};

    void AddBuffer(Buffer* buf)
    {
        mBuffers.push_back(buf);
    }
    
    // returns nullptr if it can't find it
    Buffer* GetBufferById(int id)
    {
        for(auto buf : mBuffers)
        {
            if(buf->mBufId == id)
            {
                return buf;
            }
        }
        return nullptr;
    }
    

    // returns true when it's time to exit
    bool HandleKey(int c)
    {
        auto buf = mCurrBuffer;
        
        // TODO read these keys from .led file
        switch(c)
        {
            case BACKSPACE:
            {
                buf->DeleteCharBackwards();
            } break;

            case '\t':
            {
                buf->Tab();
            } break;
            
            // quit
            case CtrlKey('q'):
            {
                return true;
            } break;

            case CtrlKey('s'):
            {
                buf->SaveToFile();
            } break;
            
            case CtrlKey('n'):
            {
                buf->NextRow();
            } break;

            case KEY_DOWN:
            {
                buf->NextRow();
            } break;

            case CtrlKey('p'):
            {
                buf->PrevRow();
            } break;

            case CtrlKey('f'):
            {
                buf->NextColumn();
            } break;

            case CtrlKey('b'):
            {
                buf->PrevColumn();
            } break;        

            case CtrlKey('a'):
            {
                buf->StartRow();
            } break;

            case CtrlKey('t'):
            {
                buf->StartColumn();
            } break;

            case CtrlKey('e'):
            {
                buf->EndRow();
            } break;

            case CtrlKey('z'):
            {
                buf->EndColumn();
            } break;
        
            case CtrlKey('g'):
            {
                buf->Cancel();
            } break;

            case CtrlKey('d'):
            {
                buf->DeleteCharForwards();
            } break;

            case CtrlKey('r'):
            {
                buf->DeleteCharBackwards();
            } break;
        
            case CtrlKey('k'):
            {
                buf->KillForward();
            } break;        
        
            case '\r':
            {
                if(buf->mMode == MODE_COMMAND)
                {
                    RunCommand(buf->mCommandString);
                    buf->Cancel();
                }
                else
                {
                    buf->InsertNewLine();
                }
            } break;

            case CtrlKey(' '):
            {
                buf->EnterCommandMode();
            } break;

            case CtrlKey('j'):
            {
                buf->EnterJumpMode();
            }
        
            default:
            {
                if(!iscntrl(c))
                {
                    if(buf->mMode == MODE_COMMAND)
                    {
                        buf->InsertCommandChar(c);
                    }
                    else
                    {
                        buf->InsertChar(c);
                    }
                }
            }
        }
        return false;
    }

    void DrawScreen()
    {
        auto buf = mCurrBuffer;
        // build up our one string to write to the screen so we don't flicker
        std::string writeString = "";

        // hide cursor
        writeString += "\x1b[?25l";

        // put cursor at top left corner
        writeString += "\x1b[H";

        int start = buf->mScrollY;
        int end = buf->mScrollY + buf->mNumRows;

        for(int y = start; y < end; y++)
        {
            // clear line
            writeString += "\x1b[K";
            if(y == end - 1)
            {

                writeString += "\x1b[103m";
                writeString += "\x1b[30m";
                // write the led line, centered
                std::string ledLine = buf->GetLedLine();
                int padding = (buf->mNumCols-ledLine.size()) / 2;
                int backpadding = buf->mNumCols - (padding + ledLine.size());
                while(padding-- > 0)
                {
                    writeString += " ";
                }
                writeString += ledLine;
                while(backpadding-- > 0)
                {
                    writeString += " ";
                }                
                writeString += "\x1b[40m";
                writeString += "\x1b[39m";
            }
            else
            {
                if((unsigned int) y < buf->mLines.size())
                {
                    writeString += buf->mLines[y];
                }
                else
                {
                    //writeString += ".";
                }
            }

            // write a newline after all but the last line
            if(y != end - 1)
            {
                writeString += "\r\n";
            }
        }

        // move the cursor to the right place
        writeString += "\x1b[" +
            std::to_string(buf->GetScreenCursY()) + ";" +
            std::to_string(buf->GetScreenCursX()) +  "H";

        // display the cursor
        writeString += "\x1b[?25h";

        write(STDOUT_FILENO, writeString.c_str(), writeString.size());
    }

    Buffer* mCurrBuffer;
};
