#include <iostream>
#include <fstream>
#include "Transcript.hpp"

#include <gtest/gtest.h>

std::string transcript_filename("/mammoth/Graduate/unit_test/data/tiny_transcript.fa");
std::string sam_filename("/mammoth/Graduate/unit_test/data/tiny_sorted.sam");

TEST(Transcript, constructor)
{   
    std::ifstream transcript_fa(transcript_filename, std::ios_base::in);
    uint32_t cnt=0;
    while(!transcript_fa.eof())
    {
        Transcript txp(transcript_fa);
        std::cerr << txp;
        EXPECT_GT(txp.getLength(), 0);
        ++cnt;
    }
    EXPECT_EQ(cnt, 3);
}

TEST(Transcript, operatorLessThan)
{
    std::ifstream transcript_fa(transcript_filename, std::ios_base::in);
    std::vector<Transcript> vec;
    while(!transcript_fa.eof())
    {
        Transcript txp(transcript_fa);
        vec.emplace_back(std::move(txp));
    }
    EXPECT_LT(vec[0], vec[1]);
    EXPECT_LT(vec[1], vec[2]);
}
