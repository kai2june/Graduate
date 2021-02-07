#include <iostream>
#include "FragmentLengthDistribution.hpp"

#include <gtest/gtest.h>

TEST(FragmentLengthDistribution, constructor)
{
    FragmentLengthDistribution FLD(100.0, 10.0);
    std::cerr << "truth: " << 0.0241971 << " mine: " << FLD.getPDF(90.0) << std::endl;
    std::cerr << "truth: " << 0.0398942 << " mine: " << FLD.getPDF(100.0) << std::endl;
}
