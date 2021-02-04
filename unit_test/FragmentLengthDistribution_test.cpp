#include <iostream>
#include "FragmentLengthDistribution.hpp"

#include <gtest/gtest.h>

TEST(FragmentLengthDistribution, constructor)
{
    FragmentLengthDistribution FLD(100.0, 10.0);
    EXPECT_EQ(FLD.getPDF(90.0), 0.0241971);
    EXPECT_EQ(FLD.getPDF(100.0), 0.0398942);
}