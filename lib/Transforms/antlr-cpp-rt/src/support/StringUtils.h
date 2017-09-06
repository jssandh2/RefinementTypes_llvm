/*
 * [The "BSD license"]
 *  Copyright (c) 2016 Mike Lischke
 *  Copyright (c) 2014 Dan McLaughlin
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "antlr4-common.h"

namespace antlrcpp {
  // For all conversions utf8 <-> utf32.
  // VS 2015 has a bug in std::codecvt_utf8<char32_t> (VS 2013 works fine).
#if defined(_MSC_VER) && _MSC_VER >= 1900 && _MSC_VER < 2000
  static std::wstring_convert<std::codecvt_utf8<__int32>, __int32> utfConverter;
#else
  static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utfConverter;
#endif

  //the conversion functions fails in VS2017, so we explicitly use a workaround
  template<typename T>
  inline std::string utf32_to_utf8(T _data)
  {
    #if _MSC_VER > 1900 && _MSC_VER < 2000
      auto p = reinterpret_cast<const int32_t *>(_data.data());
      return antlrcpp::utfConverter.to_bytes(p, p + _data.size());
    #else
      return antlrcpp::utfConverter.to_bytes(_data);
    #endif
  }

  inline auto utf8_to_utf32(const char* first, const char* last) ->
    #if _MSC_VER > 1900 && _MSC_VER < 2000
      std::u32string
    #else
      decltype(antlrcpp::utfConverter.from_bytes(first, last))
    #endif
  {
    #if _MSC_VER > 1900 && _MSC_VER < 2000
      auto r = antlrcpp::utfConverter.from_bytes(first, last);
      std::u32string s = reinterpret_cast<const char32_t *>(r.data());
      return s;
    #else
      return antlrcpp::utfConverter.from_bytes(first, last);
    #endif
  }

  void replaceAll(std::string& str, const std::string& from, const std::string& to);

  // string <-> wstring conversion (UTF-16), e.g. for use with Window's wide APIs.
  ANTLR4CPP_PUBLIC std::string ws2s(const std::wstring &wstr);
  ANTLR4CPP_PUBLIC std::wstring s2ws(const std::string &str);
}
