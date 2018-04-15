#pragma once
#include <vector>
#include <fstream>
const std::string VERSION = "0.0.1";

enum Mode
{
    MODE_EDIT,
    MODE_COMMAND,
    MODE_JUMP
};

struct Buffer
{
    Buffer(std::string filename, int id, int cols, int rows)
    {
        mBufId = id;
        mNumCols = cols;
        mNumRows = rows;
        mFileName = filename;
        mName = filename;
        Scroll();
        mLedLine = DefaultLedLine();
    }

    // TODO add filename etc. here when we start doing that
    std::string DescribeSelf()
    {
        return "--Buffer-- "
            " name: " + mName +
            " id: " + std::to_string(mBufId) +
            " lc: " + std::to_string(mLines.size());
    }
    
    int mBufId;
    std::string mName;
    std::string mFileName = "foo.txt";
        
    int mNumCols;
    int mNumRows;

    // relative to the buffer
    int mCursX = 0;
    int mCursY = 0;
    
    // relative to the screen
    int GetScreenCursX() { return mCursX - mScrollX + 1; }
    int GetScreenCursY() { return mCursY - mScrollY + 1; }

    int mScrollY = 0;
    int mScrollX = 0;

    // TODO do we want a linked list here for efficient insertion?
    std::vector<std::string> mLines = {};

    std::string* CurrLine()
    {
        if(mLines.size() == 0)
        {
            mLines.push_back("");
        }
        return &mLines[mCursY];
    }
    
    void NextColumn()  { mCursX++; Scroll(); }
    void PrevColumn()  { mCursX--; Scroll(); }
    void NextRow()     { mCursY++; Scroll(); }
    void PrevRow()     { mCursY--; Scroll(); }

    void StartRow()    { mCursX = 0; Scroll(); }
    void StartColumn() { mCursY = 0; Scroll(); }
    void EndRow()      { mCursX = CurrLine()->size(); Scroll(); }
    void EndColumn()   { mCursY = mLines.size() - 1; Scroll(); }
    
    void Scroll()
    {
        if(mLines.size() > 0)
        {
            mCursY = std::min(mCursY, (int) mLines.size()-1);
            mCursY = std::max(mCursY, 0);
        
            mCursX = std::min(mCursX, (int) CurrLine()->length());
            mCursX = std::max(mCursX, 0);
        }
        

        // keep the cursor in the middle
        mScrollY = mCursY - (mNumRows/2);
    }

    void InsertChar(char c)
    {
        CurrLine()->insert(mCursX, std::string(1, c));

        // TODO vertical insert mode, just call NextRow() here
        NextColumn();
    }

    void InsertLine(std::string text)
    {
        mLines.push_back(text);
    }

    void DeleteCharForwards()
    {
        // pressing delete at end of line
        if(mCursX == (int) CurrLine()->length())
        {
            if(mCursY < (int) mLines.size()-1)
            {
                *CurrLine() += mLines[mCursY + 1];
            }
            if(mCursY != (int) mLines.size() - 1)
            {
                mLines.erase(mLines.begin() + mCursY + 1);
            }
        }
        else
        {
            CurrLine()->erase(mCursX, 1);
        }
        Scroll();
    }

    // currently unbound
    void DeleteCharBackwards()
    {
        // pressing backspace at start of line
        if(mCursX == 0)
        {
            if(mCursY > 0)
            {
                mCursX = mLines[mCursY - 1].length();
                mLines[mCursY - 1] += *CurrLine();
                mLines.erase(mLines.begin() + mCursY);
                mCursY--;
            }
            else if(mCursY == 0 && CurrLine()->empty())
            {
                mLines.erase(mLines.begin());
            }
        }
        else
        {
            CurrLine()->erase(mCursX-- -1, 1);
        }
        Scroll();
    }
    
    void KillForward()
    {
        if(CurrLine()->empty() || mCursX == (int) CurrLine()->length())
        {
            DeleteCharForwards();
        }
        else
        {
            CurrLine()->erase(mCursX);
        }
        Scroll();
    }

    void InsertNewLine()
    {
        // split current line aat the place where we pressed enter
        std::string partBefore = CurrLine()->substr(0, mCursX);
        std::string partAfter = CurrLine()->substr(mCursX);

        *CurrLine() = partBefore;
        mLines.insert(mLines.begin() + mCursY + 1, partAfter);
        NextRow();
        StartRow();
    }
    
    // line at the bottom of the buffer
    std::string mLedLine;
    
    std::string DefaultLedLine()
    {
        return "LED : " + mName;
    }
    std::string mCommandString;
    
    Mode mMode = MODE_EDIT;

    void EnterCommandMode()
    {
        mCommandString = "";
        mMode = MODE_COMMAND;
        mLedLine = "Command: ";
    }

    void InsertCommandChar(char c)
    {
        mCommandString += std::string(1, c);
        mLedLine += std::string(1, c);
    }
    
    void EnterJumpMode()
    {
        mMode = MODE_JUMP;
        mLedLine = "Jump";
    }

    void Cancel()
    {
        mMode = MODE_EDIT;
        mLedLine = DefaultLedLine();
        mCommandString = "";
    }

    void SaveToFile()
    {
        if(mFileName == "")
        {
            // TODO prompt for a filename
        }
        else
        {
            std::ofstream  outfile;
            outfile.open(mFileName);
            for(auto line : mLines)
            {
                outfile << line << "\n";
            }
            outfile.close();
        }
    }

    bool OpenFile(std::string filename)
    {
        std::ifstream infile(filename);
        if(!infile.is_open())
        {
            return false;
        }

        mFileName = filename;
        mName = filename;

        std::string line;
        while(getline(infile, line))
        {
            InsertLine(line);
        }
        infile.close();
        
        return true;
    }

    void MakeFile(std::string filename)
    {
        mFileName = filename;
        mName = filename;
    }
};
