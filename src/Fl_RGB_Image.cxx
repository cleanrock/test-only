//
// "$Id: Fl_RGB_Image.cxx,v 1.13 2003/03/26 21:53:40 easysw Exp $"
//
// RGB_Image drawing code for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2003 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//

#include <config.h>
#include <fltk/rgbImage.h>
#include <fltk/events.h>
#include <fltk/draw.h>
#include <fltk/x.h>

#if 0
#if HAVE_LIBJPEG
extern "C" {
#  include <stdio.h>
#  include <jpeglib.h>
}
#endif // HAVE_LIBJPEG
#endif // 0

using namespace fltk;

void rgbImage::draw(int x, int y, int, int, Flags flags)
{
  if (!id) {
    Pixmap pixmap = fl_create_offscreen(w, h);
    id = (void*)pixmap;
    fl_begin_offscreen(pixmap);
    drawimage(data, 0, 0, w, h, depth);
    fl_end_offscreen();
  }
  _draw(x, y, flags);
}

#if 0 // MRS: remove lame-o JPEG code here; it doesn't belong here, it MUST
      //      be in the images library!
bool rgbImage::write_jpeg(const char *filename, int quality, int dpi)
{
#if HAVE_LIBJPEG
  int wdt, hgt;
  measure( wdt, hgt );
  if (!w || !h || !data ) return false;
  if ( depth!=3 ) return false; // for now, we only write RGB images

  JSAMPLE *image_buffer = (unsigned char*)data;
  int image_height = hgt;
  int image_width = wdt;
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE * outfile;
  JSAMPROW row_pointer[1];
  int row_stride;

  /* Step 1: allocate and initialize JPEG compression object */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  if ((outfile = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }
  jpeg_stdio_dest(&cinfo, outfile);

  /* Step 3: set parameters for compression */
  cinfo.image_width = image_width;
  cinfo.image_height = image_height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE);
  cinfo.density_unit = 1;
  cinfo.X_density = cinfo.Y_density = dpi;

  /* Step 4: Start compressor */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */
  row_stride = image_width * 3;
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);
  fclose(outfile);

  /* Step 7: release JPEG compression object */
  jpeg_destroy_compress(&cinfo);

  return true;

#else // HAVE_LIBJPEG
  return false;
#endif // HAVE_LIBJPEG
}
#endif // 0


//
// End of "$Id: Fl_RGB_Image.cxx,v 1.13 2003/03/26 21:53:40 easysw Exp $".
//
