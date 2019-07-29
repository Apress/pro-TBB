/*
Copyright (C) 2019 Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom
the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*/

#define NOMINMAX

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "lodepng.h"
#include <tbb/tbb.h>

class PNGImage {
public:
  uint64_t frameNumber = -1;
  unsigned int width = 0, height = 0;
  std::shared_ptr<std::vector<unsigned char>> buffer; 
  static const int numChannels = 4; 
  static const int redOffset = 0; 
  static const int greenOffset = 1; 
  static const int blueOffset = 2; 

  PNGImage() {} 
  PNGImage(uint64_t frame_number, const std::string& file_name);
  PNGImage(const PNGImage& p);
  virtual ~PNGImage() {}
  void write() const;
};

int getNextFrameNumber();
PNGImage getLeftImage(uint64_t frameNumber);
PNGImage getRightImage(uint64_t frameNumber);
void increasePNGChannel(PNGImage& image, int channel_offset, int increase);
void mergePNGImages(PNGImage& right, const PNGImage& left);

void fig_2_30() {
  using Image = PNGImage;
  using ImagePair = std::pair<PNGImage, PNGImage>;
  tbb::parallel_pipeline(
    /* tokens */ 8,
    /* make the left image filter */
    tbb::make_filter<void, Image>(
      /* filter type */ tbb::filter::serial_in_order,
      [&](tbb::flow_control& fc) -> Image {
        if (uint64_t frame_number = getNextFrameNumber()) {
          return getLeftImage(frame_number);
        } else {
          fc.stop();
          return Image{};
        }
      }) & 
    tbb::make_filter<Image, ImagePair>(
      /* filter type */ tbb::filter::serial_in_order,
      [&](Image left) -> ImagePair {
        return ImagePair(left, getRightImage(left.frameNumber));
      }) & 
    tbb::make_filter<ImagePair, ImagePair>(
      /* filter type */ tbb::filter::parallel,
      [&](ImagePair p) -> ImagePair {
        increasePNGChannel(p.first, Image::redOffset, 10);
        return p;
      }) & 
    tbb::make_filter<ImagePair, ImagePair>(
      /* filter type */ tbb::filter::parallel,
      [&](ImagePair p) -> ImagePair {
        increasePNGChannel(p.second, Image::blueOffset, 10);
        return p;
      }) & 
    tbb::make_filter<ImagePair, Image>(
      /* filter type */ tbb::filter::parallel,
      [&](ImagePair p) -> Image {
        mergePNGImages(p.second, p.first);
        return p.second;
      }) & 
    tbb::make_filter<Image, void>(
      /* filter type */ tbb::filter::parallel,
      [&](Image img) {
        img.write();
      }) 
  );
}

PNGImage::PNGImage(uint64_t frame_number, const std::string& file_name) :
  frameNumber{frame_number}, buffer{std::make_shared< std::vector<unsigned char> >()} {
  if (lodepng::decode(*buffer, width, height, file_name)) {
     std::cerr << "Error: could not read PNG file!" << std::endl;
     width = height = 0;
  }
};

PNGImage::PNGImage(const PNGImage& p) : frameNumber{p.frameNumber}, 
                                        width{p.width}, height{p.height},
                                        buffer{p.buffer} {}

void PNGImage::write() const {
  std::string file_name = std::string("out") + std::to_string(frameNumber) + ".png";
  if (lodepng::encode(file_name, *buffer, width, height)) {
    std::cerr << "Error: could not write PNG file!" << std::endl;
  }
}

static int stereo3DFrameCounter = 0;
static int stero3DNumImages = 0;

void initStereo3D(int num_images) {
  stereo3DFrameCounter = 0;
  stero3DNumImages = num_images;
}

int getNextFrameNumber() {
  if ( stereo3DFrameCounter < stero3DNumImages ) {
    return ++stereo3DFrameCounter;
  } else {
    return 0;
  }
}

PNGImage getLeftImage(uint64_t frameNumber) {
  return PNGImage(frameNumber, "input1.png");
}

PNGImage getRightImage(uint64_t frameNumber) {
  return PNGImage(frameNumber, "input2.png");
}

void increasePNGChannel(PNGImage& image, int channel_offset, int increase) {
  const int height_base = PNGImage::numChannels * image.width;
  std::vector<unsigned char>& buffer = *image.buffer;

  // Increase selected color channel by a predefined value
  for (unsigned int y = 0; y < image.height; y++) {
    const int height_offset = height_base * y;
    for (unsigned int x = 0; x < image.width; x++) {
      int pixel_offset = height_offset + PNGImage::numChannels * x + channel_offset;
      buffer[pixel_offset] = static_cast<uint8_t>(std::min(buffer[pixel_offset] + increase, 255));
    }
  }
}

void mergePNGImages(PNGImage& right, const PNGImage& left) {
  const int channels_per_pixel = PNGImage::numChannels;
  const int height_base = channels_per_pixel * right.width;
  std::vector<unsigned char>& left_buffer = *left.buffer;
  std::vector<unsigned char>& right_buffer = *right.buffer;

  for (unsigned int y = 0; y < right.height; y++) {
    const int height_offset = height_base * y;
    for (unsigned int x = 0; x < right.width; x++) {
      const int pixel_offset = height_offset + channels_per_pixel * x;
      const int red_index = pixel_offset + PNGImage::redOffset;
      right_buffer[red_index] = left_buffer[red_index];
    }
  }
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}


int main() {
  int num_images = 3;

  initStereo3D(num_images);

  warmupTBB();
  double parallel_time = 0.0;
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_2_30();
    parallel_time = (tbb::tick_count::now() - t0).seconds();
  }
  std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;
  return 0;
}

