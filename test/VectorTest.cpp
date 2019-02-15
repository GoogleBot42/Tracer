#include "Vector.h"

#include <gtest/gtest.h>

using Tracer::Vector;
using Tracer::Vector3;
using Tracer::Vector3f;

class VectorTest : public ::testing::Test {
protected:
    Vector<float,6> vecf6 = {1.1F,2.2F,3.3F,4.4F,5.5F,6.6F};
    const Vector<int,4> veci4 = {1,2,3,4};

    Vector3<float> vecf3 = {1.1F,2.2F,3.3F};
    Vector3<int> veci3 = {1,2,3};

    Vector3f vecf = {3.3F,2.2F,1.1F};
};

TEST_F(VectorTest, Constructor) {
    // check sizes
    EXPECT_EQ(vecf6.Size(), 6);
    EXPECT_EQ(veci4.Size(), 4);
    EXPECT_EQ(vecf3.Size(), 3);
    EXPECT_EQ(veci3.Size(), 3);
    EXPECT_EQ(vecf.Size(), 3);

    // check some contents
    EXPECT_EQ(vecf6[0], 1.1F);
    EXPECT_EQ(vecf6[1], 2.2F);
    EXPECT_EQ(vecf6[2], 3.3F);
    EXPECT_EQ(vecf6[3], 4.4F);
    EXPECT_EQ(vecf6[4], 5.5F);
    EXPECT_EQ(vecf6[5], 6.6F);

    EXPECT_EQ(veci4[3], 4);
    EXPECT_EQ(vecf3[2], 3.3F);
    EXPECT_EQ(veci3[1], 2);
    EXPECT_EQ(vecf[2], 1.1F);
}

TEST_F(VectorTest, Operator_Plus) {
    EXPECT_EQ(  (vecf6 + Vector<float,6>({6,5,4,3,2,1})),   (Vector<float,6>({7.1F,7.2F,7.3F,7.4F,7.5F,7.6F}))  );
    EXPECT_EQ(  (vecf + vecf3),                             (Vector3f(4.4F,4.4F,4.4F))                          );
    EXPECT_EQ(  (veci3 + Vector<int,3>({3,2,1})),           (Vector3<int>(4,4,4))                               );
    EXPECT_EQ(  (veci4 + Vector<int,4>({4,3,2,1})),         (Vector<int,4>({5,5,5,5}))                          );
}

TEST_F(VectorTest, Operator_Minus) {
    EXPECT_EQ(  (vecf6 - Vector<float,6>({6,5,4,3,2,1})),   (Vector<float,6>({-4.9F,-2.8F,-0.7F,1.4F,3.5F,5.6F}))    );
    EXPECT_EQ(  (vecf - vecf3),                             (Vector3f(2.2F,0.F,-2.2F))                              );
    EXPECT_EQ(  (veci3 - Vector<int,3>({3,2,1})),           (Vector3<int>(-2,0,2))                                  );
    EXPECT_EQ(  (veci4 - Vector<int,4>({4,3,2,1})),         (Vector<int,4>({-3,-1,1,3}))                            );
}

TEST_F(VectorTest, Operator_Times) {
    EXPECT_EQ(  (vecf6 * 0.1F), (Vector<float,6>({.11F,.22F,.33F,.44F,.55F,.66F}))  );
    EXPECT_EQ(  (vecf * 0.2F),  (Vector3f(.66F,.44F,.22F))                          );
    EXPECT_EQ(  (veci3 * 0.5F), (Vector3<int>(0,1,1))                               );
    EXPECT_EQ(  (veci4 * 2.0F), (Vector<int,4>({2,4,6,8}))                          );
}

TEST_F(VectorTest, Operator_PlusEquals) {
    EXPECT_EQ(  (vecf6 += Vector<float,6>({6,5,4,3,2,1})),  (Vector<float,6>({7.1F,7.2F,7.3F,7.4F,7.5F,7.6F}))  );
    EXPECT_EQ(   vecf6,                                     (Vector<float,6>({7.1F,7.2F,7.3F,7.4F,7.5F,7.6F}))  );
    EXPECT_EQ(  (vecf += vecf3),                            (Vector3f(4.4F,4.4F,4.4F))                          );
    EXPECT_EQ(   vecf,                                      (Vector3f(4.4F,4.4F,4.4F))                          );
    EXPECT_EQ(  (veci3 += Vector<int,3>({3,2,1})),          (Vector3<int>(4,4,4))                               );
    EXPECT_EQ(   veci3,                                     (Vector3<int>(4,4,4))                               );

    // Compile failure becuase veci4 is a const :)
    //EXPECT_EQ(  (veci4 += Vector<int,4>({4,3,2,1})),        (Vector<int,4>({5,5,5,5}))                          );
    //EXPECT_EQ(   veci4,                                     (Vector<int,4>({5,5,5,5}))                          );
}

TEST_F(VectorTest, Operator_MinusEquals) {
    EXPECT_EQ(  (vecf6 -= Vector<float,6>({6,5,4,3,2,1})),  (Vector<float,6>({-4.9F,-2.8F,-0.7F,1.4F,3.5F,5.6F}))   );
    EXPECT_EQ(   vecf6,                                     (Vector<float,6>({-4.9F,-2.8F,-0.7F,1.4F,3.5F,5.6F}))   );
    EXPECT_EQ(  (vecf -= vecf3),                            (Vector3f(2.2F,0.F,-2.2F))                              );
    EXPECT_EQ(   vecf,                                      (Vector3f(2.2F,0.F,-2.2F))                              );
    EXPECT_EQ(  (veci3 -= Vector<int,3>({3,2,1})),          (Vector3<int>(-2,0,2))                                  );
    EXPECT_EQ(   veci3,                                     (Vector3<int>(-2,0,2))                                  );

    // Compile failure becuase veci4 is a const :)
    //EXPECT_EQ(  (veci4 -= Vector<int,4>({4,3,2,1})),        (Vector<int,4>({-3,-1,1,3}))                            );
    //EXPECT_EQ(   veci4,                                     (Vector<int,4>({-3,-1,1,3}))                            );
}

TEST_F(VectorTest, Operator_Equal) {
    // most other tests use this operator already but there is more testing needed
    EXPECT_TRUE(vecf6 == (Vector<float,6>({1.1F,2.2F,3.3F,4.4F,5.5F,6.6F}))  );
    EXPECT_FALSE(vecf6 == (Vector<float,6>({0,0,0,0,0,0}))                   );
    // test floating point approximately equal
    EXPECT_TRUE(Vector3f(1.000001F) == Vector3f(1.F));
    EXPECT_FALSE(Vector3f(1.1F) == Vector3f(1.F));
}

TEST_F(VectorTest, Operator_NotEqual) {
    EXPECT_FALSE(vecf6 != (Vector<float,6>({1.1F,2.2F,3.3F,4.4F,5.5F,6.6F}))  );
    EXPECT_TRUE(vecf6 != (Vector<float,6>({0,0,0,0,0,0}))                     );
    // test floating point approximately equal
    EXPECT_FALSE(Vector3f(1.000001F) != Vector3f(1.F));
    EXPECT_TRUE(Vector3f(1.1F) != Vector3f(1.F));
}

TEST_F(VectorTest, Multiply) {
    EXPECT_EQ(  (vecf6.Multiply(Vector<float,6>({6,5,4,3,2,1}))),   (Vector<float,6>({6.6F,11.F,13.2F,13.2F,11.F,6.6F}))    );
    EXPECT_EQ(  (veci4.Multiply(Vector<int,4>({4,3,2,1}))),         (Vector<int,4>({4,6,6,4}))                              );
}

TEST_F(VectorTest, Dot) {
    EXPECT_EQ(  (vecf6.Dot(Vector<float,6>({6,5,4,3,2,1}))),   61.6F);
    EXPECT_EQ(  (veci4.Dot(Vector<int,4>({4,3,2,1}))),         20.F);
}

TEST_F(VectorTest, Length) {
    EXPECT_FLOAT_EQ(vecf6.Length(), 10.49333122F);
    EXPECT_FLOAT_EQ(veci4.Length(), 5.477225575F);
}

TEST_F(VectorTest, Cross) {
    EXPECT_EQ( Vector3f( 1,0,0).Cross(Vector3f(0,1,0)),  Vector3f(0,0, 1)   );
    EXPECT_EQ( Vector3f(-1,0,0).Cross(Vector3f(0,1,0)),  Vector3f(0,0,-1)   );
    EXPECT_EQ( Vector3f(1,2,3).Cross(Vector3f(-4,5,6)),  Vector3f(-3,-18,13));
}

TEST_F(VectorTest, Clamp) {
    EXPECT_EQ( Vector3f(100,-10,0.5f).Clamp(),  Vector3f(1,0,0.5f)  );
}

TEST_F(VectorTest, Normalize) {
    EXPECT_FLOAT_EQ(vecf.Normalize().Length(), 1.F);
}

TEST_F(VectorTest, Accessors) {
    EXPECT_EQ(vecf3[0], vecf3.X());
    EXPECT_EQ(vecf3[1], vecf3.Y());
    EXPECT_EQ(vecf3[2], vecf3.Z());

    EXPECT_EQ(vecf[0], vecf.X());
    EXPECT_EQ(vecf[1], vecf.Y());
    EXPECT_EQ(vecf[2], vecf.Z());

    // test const
    const Vector3<int> v(10,11,12);
    EXPECT_EQ(v[0], v.X());
    EXPECT_EQ(v[1], v.Y());
    EXPECT_EQ(v[2], v.Z());
}
