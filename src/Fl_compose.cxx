//
// "$Id$"
//
// Character compose processing for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

/**
\file Fl_compose.cxx
Utility functions to support text input.
*/

#include <FL/Fl.H>
#include <FL/Fl_System_Driver.H>

#ifndef FL_DOXYGEN
int Fl::compose_state = 0;
#endif


/** Any text editing widget should call this for each FL_KEYBOARD event.
 Use of this function is very simple.
 
 <p>If <i>true</i> is returned, then it has modified the
 Fl::event_text() and Fl::event_length() to a set of <i>bytes</i> to
 insert (it may be of zero length!).  It will also set the "del"
 parameter to the number of <i>bytes</i> to the left of the cursor to
 delete, this is used to delete the results of the previous call to
 Fl::compose().
 
 <p>If <i>false</i> is returned, the keys should be treated as function
 keys, and del is set to zero. You could insert the text anyways, if
 you don't know what else to do.
 
 <p>On the Mac OS platform, text input can involve marked text, that is, 
 temporary text replaced by other text during the input process. This occurs,
 e.g., when using dead keys or when entering CJK characters.
 Text editing widgets should preferentially signal
 marked text, usually underlining it. Widgets can use
 <tt>int Fl::compose_state</tt> <i>after</i> having called Fl::compose(int&)
 to obtain the length in bytes of marked text that always finishes at the
 current insertion point. It's the widget's task to underline marked text.
 Widgets should also call <tt>void Fl::reset_marked_text()</tt> when processing FL_UNFOCUS
 events. Optionally, widgets can also call
 <tt>void Fl::insertion_point_location(int x, int y, int height)</tt> to indicate the window 
 coordinates of the bottom of the current insertion point and the line height. 
 This way, auxiliary windows that help choosing among alternative characters 
 appear just below the insertion point. If widgets don't do that, 
 auxiliary windows appear at the widget's bottom. The
 Fl_Input and Fl_Text_Editor widgets underline marked text.
 If none of this is done by a user-defined text editing widget,
 text input will work, but will not signal to the user what text is marked.
 Finally, text editing widgets should call <tt>set_flag(MAC_USE_ACCENTS_MENU);</tt>
 in their constructor if they want to use the feature introduced with Mac OS 10.7 "Lion"
 where pressing and holding a key on the keyboard opens an accented-character menu window.
 
 <p>Though the current implementation returns immediately, future
 versions may take quite awhile, as they may pop up a window or do
 other user-interface things to allow characters to be selected.
 */
int Fl::compose(int& del) {
  return Fl_System_Driver::driver()->compose(del);
}

/**
 If the user moves the cursor, be sure to call Fl::compose_reset().
 The next call to Fl::compose() will start out in an initial state. In
 particular it will not set "del" to non-zero. This call is very fast
 so it is ok to call it many times and in many places.
 */
void Fl::compose_reset()
{
  Fl_System_Driver::driver()->compose_reset();
}

//
// End of "$Id$"
//

