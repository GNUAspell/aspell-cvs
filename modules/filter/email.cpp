// This file is part of The New Aspell
// Copyright (C) 2001 by Kevin Atkinson under the GNU LGPL license
// version 2.0 or 2.1.  You should have received a copy of the LGPL
// license along with this library if you did not you can find
// it at http://www.gnu.org/.

#include "config.hpp"
#include "indiv_filter.hpp"
#include "mutable_container.hpp"
#include "copy_ptr-t.hpp"
#include "loadable-filter-API.hpp"

namespace acommon {

  class EmailFilter : public IndividualFilter 
  {
    bool prev_newline;
    bool in_quote;
    int margin;
    int n;

    class QuoteChars : public MutableContainer {
      bool data[256];
    public:
      PosibErr<bool> add(ParmString s) {
        data[static_cast<unsigned char>(s[0])] = true;
        return true;
      }
      PosibErr<bool> remove(ParmString s) {
        data[static_cast<unsigned char>(s[0])] = false;
        return true;
      }
      PosibErr<void> clear() {
        memset(data, 0, sizeof(bool)*256);
	return no_err;
      }
      bool have(char c) {
        return data[static_cast<unsigned char>(c)];
      }
      QuoteChars() {clear();}
    };
    QuoteChars is_quote_char;
    
  public:
    PosibErr<bool> setup(Config *);
    void reset();
    void process(FilterChar * &, FilterChar * &);
  };

  PosibErr<bool> EmailFilter::setup(Config * opts) 
  {
    name_ = "email-filter";
    order_num_ = 0.85;
    reset();
    return true;
  }
  
  void EmailFilter::reset() 
  {
    prev_newline = true;
    in_quote = false;
    n = 0;
  }

  void EmailFilter::process(FilterChar * & str, FilterChar * & end)
  {
    FilterChar * line_begin = str;
    FilterChar * cur = str;
    while (cur < end) {
      if (prev_newline && is_quote_char.have(*cur))
	in_quote = true;
      if (*cur == '\n') {
	if (in_quote) {
	  for (FilterChar * i = line_begin; i != cur; ++i)
	    *i = ' ';
	}
	line_begin = cur;
	in_quote = false;
	prev_newline = true;
	n = 0;
      } else if (n < margin) {
	++n;
      } else {
	prev_newline = false;
      }
      ++cur;
    }
    if (in_quote)
      for (FilterChar * i = line_begin; i != cur; ++i)
	*i = ' ';
  }
  
ACTIVATE_FILTER(acommon,EmailFilter,email);
}

