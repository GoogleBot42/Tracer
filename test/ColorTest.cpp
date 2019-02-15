#include "Image.h"

#include <gtest/gtest.h>

using Tracer::Color;

///
/// \brief Only a partial test is needed.  Most functionality is inherrited from Tracer::Vector
///
class ColorTest : public ::testing::Test {
protected:
    Color c = Color(.0F,.1F,.8F);
};

TEST_F(ColorTest, Accessors) {
    EXPECT_EQ(c.R(), .0F);
    EXPECT_EQ(c.G(), .1F);
    EXPECT_EQ(c.B(), .8F);
    c.R() = .8F;
    c.G() = .1F;
    c.B() = .0F;
    EXPECT_EQ(c.R(), .8F);
    EXPECT_EQ(c.G(), .1F);
    EXPECT_EQ(c.B(), .0F);
}
