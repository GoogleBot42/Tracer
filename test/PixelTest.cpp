#include "Image.h"

#include <gtest/gtest.h>

using Tracer::Pixel;
using Tracer::Color;

///
/// \brief Only a partial test is needed.  Most functionality is inherrited from Tracer::Vector
///
class PixelTest : public ::testing::Test {
protected:
    Pixel p = Pixel(50,100,200);
};

TEST_F(PixelTest, Accessors) {
    EXPECT_EQ(p.R(), 50);
    EXPECT_EQ(p.G(),100);
    EXPECT_EQ(p.B(),200);
    p.R() = 200;
    p.G() = 100;
    p.B() =  50;
    EXPECT_EQ(p.R(),200);
    EXPECT_EQ(p.G(),100);
    EXPECT_EQ(p.B(), 50);
}

TEST_F(PixelTest, CreatePixelFromColor) {
    p = Pixel(Color(1.F,.5F,1.F));
    EXPECT_EQ(p, Pixel(255,127,255));
}

TEST_F(PixelTest, GammaCorrection) {
    // color = 255 * (color/255)^(1/2.2)
    EXPECT_EQ(p.GammaCorrect(), Pixel(122,167,228));
}
