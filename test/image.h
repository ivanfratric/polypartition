/*************************************************************************/
/* Copyright (c) 2011-2020 Ivan Fratric and contributors.                */
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

// A simple image class with some (very basic) image processing operations.
class Image {
  friend class ImageIO;

  protected:
  unsigned char *data;
  long width;
  long height;

  long round(float x);
  float interpolate(float x1, float x2, float x3, float x4, float dx, float dy);

  public:
  struct Pixel {
    unsigned char B;
    unsigned char G;
    unsigned char R;
  };

  // Constructors and destructor.
  Image(void);
  Image(long width, long height);
  Image(const Image &src);
  Image &operator=(const Image &src);
  ~Image(void);

  // Initializes the image of specified width and height.
  // All pixels are set to black.
  void Init(long width, long height);

  // Property getters.
  long GetWidth();
  long GetHeight();
  unsigned char *GetData();

  // Pixel getters and setters.
  unsigned char GetPixelGray(long x, long y);
  unsigned char GetPixelRed(long x, long y);
  unsigned char GetPixelGreen(long x, long y);
  unsigned char GetPixelBlue(long x, long y);
  Image::Pixel GetPixelColor(long x, long y);
  void SetPixelGray(long x, long y, unsigned char c);
  void SetPixelColor(long x, long y, Image::Pixel rgb);
  void SetPixelRed(long x, long y, unsigned char c);
  void SetPixelGreen(long x, long y, unsigned char c);
  void SetPixelBlue(long x, long y, unsigned char c);

  // Returns the color of a pixel at real coordinates (x, y)
  // using bilinear interpolation.
  Image::Pixel GetPixelBilinear(float x, float y);

  // Some basic image processing functions.

  // Returns the mean value of pixel intensity.
  unsigned char GetMeanGray();

  // Computes the histogram of image intensity
  // and returns it via histogram parameter.
  // params:
  //    histogram:
  //       An array of 256 components, used to return the histogram.
  void GetHistogramGray(long *histogram);

  // Binarizes the image (posterize with two levels). All pixels with the
  // intensity lower than threshold become black, and all others become white.
  void Binarize(unsigned char threshold);

  // Flips the image in horizontal direction.
  void FlipHorizontal();

  // Flips the image in vertical direction.
  void FlipVertical();

  // inverts image colors
  void Invert();

  // Crops the image. Returns the sub-image with upper left corner at
  // (posx, posy) with the size of (width, height).
  Image Crop(int posx, int posy, int width, int height);

  // Resizes the image by an integer factor.
  // For example, if factor is 2, returns the image with size
  // (width/2, height/2). Each pixel in a new image is obtained as
  // an average of corresponding pixels in the original image.
  Image Resize(int factor);

  // Computes the convolution of image and a filter.
  // params:
  //    filter:
  //       An array containing the filter coefficients.
  //    filterwidth:
  //       The width of the filter array.
  //    filterheight:
  //       The height of the filter array.
  Image Filter(float *filter, long filterwidth, long filterheight);

  // Filters the image using Gaussian blur.
  // params:
  //    sigma:
  //       The standard deviation of the Gaussian filter.
  //    masksize:
  //       The size of the corresponding filter.
  //       If set to 0, the masksize will be calculated as sigma * 2 * 2 + 1.
  Image GaussBlur(float sigma, long masksize = 0);

  // Paints the whole image with the given color.
  void Clear(Pixel color);

  // Draws a line from point (x1, y1) to point (x2, y2) in the given color.
  void DrawLine(int x1, int y1, int x2, int y2, Pixel color);
};

inline unsigned char Image::GetPixelGray(long x, long y) {
  unsigned char c;
  long index = 3 * ((y * width) + x);
  c = (unsigned char)(((long)(data[index]) + (long)(data[index + 1]) + (long)(data[index + 2])) / 3);
  return c;
}

inline unsigned char Image::GetPixelRed(long x, long y) {
  long index = 3 * ((y * width) + x);
  return data[index];
}

inline unsigned char Image::GetPixelGreen(long x, long y) {
  long index = 3 * ((y * width) + x) + 1;
  return data[index];
}

inline unsigned char Image::GetPixelBlue(long x, long y) {
  long index = 3 * ((y * width) + x) + 2;
  return data[index];
}

inline void Image::SetPixelRed(long x, long y, unsigned char c) {
  long index = 3 * ((y * width) + x);
  data[index] = c;
}

inline void Image::SetPixelGreen(long x, long y, unsigned char c) {
  long index = 3 * ((y * width) + x) + 1;
  data[index] = c;
}

inline void Image::SetPixelBlue(long x, long y, unsigned char c) {
  long index = 3 * ((y * width) + x) + 2;
  data[index] = c;
}

inline Image::Pixel Image::GetPixelColor(long x, long y) {
  Image::Pixel rgb;
  long index = 3 * ((y * width) + x);
  rgb.B = data[index];
  rgb.G = data[index + 1];
  rgb.R = data[index + 2];
  return rgb;
}

inline void Image::SetPixelGray(long x, long y, unsigned char c) {
  long index = 3 * ((y * width) + x);
  data[index] = c;
  data[index + 1] = c;
  data[index + 2] = c;
}

inline void Image::SetPixelColor(long x, long y, Image::Pixel rgb) {
  if (x < 0) {
    return;
  }
  if (y < 0) {
    return;
  }
  if (x >= width) {
    return;
  }
  if (y >= height) {
    return;
  }
  long index = 3 * ((y * width) + x);
  data[index] = rgb.B;
  data[index + 1] = rgb.G;
  data[index + 2] = rgb.R;
}

inline long Image::GetWidth() {
  return width;
}

inline long Image::GetHeight() {
  return height;
}

inline unsigned char *Image::GetData() {
  return data;
}
