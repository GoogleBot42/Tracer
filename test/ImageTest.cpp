#include "Image.h"

#include <gtest/gtest.h>

using Tracer::Image;
using Tracer::Pixel;

///
/// \brief Only a partial test is needed.  Most functionality is inherrited from Tracer::Vector
///
class ImageTest : public ::testing::Test {
protected:
    ImageTest() {
        img.SetPixel(0,0,Pixel(1,1,1));
    }
    Image img = Image(10,10);
};

TEST_F(ImageTest, Accessors) {
    EXPECT_EQ(img.GetWidth(), 10);
    EXPECT_EQ(img.GetHeight(),10);
    EXPECT_EQ(img.GetDataSize(),100);
    EXPECT_EQ(img.GetData(), &img.GetPixel(0,0));
    EXPECT_EQ(img.GetPixel(0,0), Pixel(1,1,1));
    EXPECT_EQ(img.GetPixel(0,1), Pixel(0,0,0));
    EXPECT_EQ(img.GetPixel(1,1), Pixel(0,0,0));
    img.GetPixel(0,0) = Pixel(0,0,0);
    img.SetPixel(0,1,Pixel(1,1,1));
    img.SetPixel(1,1,Pixel(1,1,1));
    EXPECT_EQ(img.GetPixel(0,0), Pixel(0,0,0));
    EXPECT_EQ(img.GetPixel(0,1), Pixel(1,1,1));
    EXPECT_EQ(img.GetPixel(1,1), Pixel(1,1,1));
}

TEST_F(ImageTest, MoveConstructor) {
    Image newImg = std::move(img);

    EXPECT_EQ(newImg.GetPixel(0,0), Pixel(1,1,1));
    EXPECT_EQ(newImg.GetWidth(), 10);
    EXPECT_EQ(newImg.GetHeight(),10);
    EXPECT_EQ(img.GetData(), nullptr);
    EXPECT_EQ(img.GetWidth(), 0);
    EXPECT_EQ(img.GetHeight(),0);
}
