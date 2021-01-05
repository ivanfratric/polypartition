/*************************************************************************/
/* Copyright (c) 2011-2021 Ivan Fratric and contributors.                */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#pragma once

#define IMGTYPE_RAW 0
#define IMGTYPE_BMP 1
#define IMGTYPE_PPM 2
#define IMGTYPE_PGM 3
#define IMGTYPE_UNSUPPORTED 999

// Handles basic image file input/output.
// Only supports uncompressed image formats (RAW, BMP, PPM, PGM).
class ImageIO {
  protected:
  // Gets the file extension from the file name.
  const char *GetFileExtension(const char *filename);

  // Determines the image format from the file name.
  int GetImageType(const char *filename);

  public:
  // Loads the image from `filename` into `image`.
  // This method automatically determines the image format.
  void LoadImage(const char *filename, Image *image);

  // Loads the image from a file named `filename` into `image`,
  // using the format given as `imageType`.
  void LoadImage(const char *filename, Image *image, int imageType);

  // Saves the image into file named `filename`.
  // This method automatically determines the image format.
  void SaveImage(const char *filename, Image *image);

  // Saves the image into file named `filename`,
  // using the format given as `imageType`.
  void SaveImage(const char *filename, Image *image, int imageType);

  // Loads the uncompressed BMP image from `filename` into `image`.
  void LoadImageBMP(const char *filename, Image *image);
  // Saves the image into file named `filename` in uncompressed BMP format.
  void SaveImageBMP(const char *filename, Image *image);

  // Loads the PPM image from `filename` into `image`.
  void LoadImagePPM(const char *filename, Image *image);
  // Saves the image into file named `filename` in PPM format.
  void SaveImagePPM(const char *filename, Image *image);

  // Loads the PGM image from `filename` into `image`.
  void LoadImagePGM(const char *filename, Image *image);
  // Saves the image into file named `filename` in PGM format.
  void SaveImagePGM(const char *filename, Image *image);

  // Loads the image from the file named `filename`.
  // The file is assumed to be structured so that it only contains
  // an array of raw (gray) pixel values, as the file does not contain
  // the image width and height, those are passed as parameters to the
  // function. If width and height are 0, the image is assumed to be
  // square and the width and height are computed based on the file size.
  void LoadImageRAW(const char *filename, Image *image, long width = 0, long height = 0);

  // Saves the image to a file named `filename`.
  // Only the array of raw (gray) pixel values are stored,
  // without additional information such as image size.
  void SaveImageRAW(const char *filename, Image *image);
};
