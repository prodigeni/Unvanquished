/*
===========================================================================
Daemon BSD Source Code
Copyright (c) 2013-2014, Daemon Developers
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Daemon developers nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL DAEMON DEVELOPERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
===========================================================================
*/

#include <string>
#include <climits>

#ifndef COMMON_LINE_EDIT_DATA_H_
#define COMMON_LINE_EDIT_DATA_H_

//FIXME: the namespace and the class name aren't very explicit
namespace Util {

    class LineEditData {
        public:
            static const int defaultScrollSize = 16;

            LineEditData(int size, int scrollSize = defaultScrollSize);

            const std::u32string& GetText() const;
            std::u32string& GetText();
            const char32_t* GetViewText() const;
            unsigned GetViewStartPos() const;
            unsigned GetCursorPos() const;
            unsigned GetViewCursorPos() const;

            void SetText(std::u32string text);

            void CursorLeft(int times = 1);
            void CursorRight(int times = 1);
            void CursorStart();
            void CursorEnd();
            void SetCursor(int pos);

            void DeleteNext(int times = 1);
            void DeletePrev(int times = 1);
            void DeleteEnd();

            void AddChar(char32_t a);
            void SwapWithNext();

            void Clear();

            void SetWidth(int width);
            unsigned GetWidth() const;

        private:
            void UpdateScroll();
            std::u32string buffer;
            int scrollSize;
            unsigned width;
            unsigned scroll;
            unsigned cursor;
    };

    // Implementation details

    inline void LineEditData::DeleteEnd()
    {
        DeleteNext(INT_MAX);
    }
}

#endif // COMMON_LINE_EDIT_DATA_H_
