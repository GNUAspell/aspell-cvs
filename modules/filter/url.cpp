// This file is part of The New Aspell
// Copyright (C) 2001 by Kevin Atkinson under the GNU LGPL license
// version 2.0 or 2.1.  You should have received a copy of the LGPL
// license along with this library if you did not you can find
// it at http://www.gnu.org/.

#define COMPILE_IN_FILTER // url filter is always static

#include "indiv_filter.hpp"
#include "key_info.hpp"
#include "loadable-filter-API.hpp"

namespace acommon {

  class UrlFilter : public IndividualFilter {
  public:
    PosibErr<bool> setup(Config *);
    void reset() {}
    void process(FilterChar * &, FilterChar * &);
  };

  PosibErr<bool> UrlFilter::setup(Config *) 
  {
    name_ = "url-filter";
    order_num_ = 0.95;
    return true;
  }

  void UrlFilter::process(FilterChar * & str0, FilterChar * & end)
  {
    enum {slash, who_cares} prev_char;
    enum {wbegin, wmiddle, wend} word_pos;
    bool blank_out;
    int point_chars;
    FilterChar * str = str0;
    FilterChar * cur = str;

    while (cur < end) {
      prev_char = who_cares;
      word_pos = wbegin;
      blank_out = false;
      point_chars = 0;
      do {
	if (cur + 1 == end ||
	    cur[1] == ' ' || cur[1] == '\n' || cur[1] == '\t')
	  word_pos = wend;
	if (word_pos == wmiddle) {
	  switch (*cur) {
	  case '@':
	    blank_out = true;
	    prev_char = who_cares;
	    break;
	  case '.':
	    ++point_chars;
	    prev_char = who_cares;
	    break;
	  case '/':
	    if (prev_char == slash) blank_out = true;
	    prev_char = slash;
	    break;
	  default:
	    prev_char = who_cares;
	  }
	}
	if (word_pos == wbegin) word_pos = wmiddle;
	++cur;
      } while (word_pos != wend);
      if (point_chars > 1) blank_out = true;
      if (blank_out) {
	for (FilterChar * i = str; i != cur; ++i)
	  *i = ' ';
      }
      str = cur;
    }
  }

  ACTIVATE_FILTER(acommon,UrlFilter,url)
}
