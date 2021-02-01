#include <iostream>
#include <fstream>
#include <vector>
#include "SAMHeader.hpp"
#include "SAM.hpp"
#include "ReadPair.hpp"
#include "Transcript.hpp"

#include <gtest/gtest.h>

std::string transcript_filename("/mammoth/Graduate/unit_test/data/tiny_transcript.fa");
std::string sam_filename("/mammoth/Graduate/unit_test/data/tiny_sorted.sam");

TEST(ReadPair, all)
{
    std::ifstream transcript_fa(transcript_filename, std::ios_base::in);
    std::vector<Transcript> vec;
    while(!transcript_fa.eof())
    {
        Transcript txp(transcript_fa);
        vec.emplace_back(std::move(txp));
    }

    ReadPair rp(vec, std::make_pair("read111:A", "AAAA"));
    rp.setReadTwo(std::make_pair("read111:S", "CCCC"));
    rp.appendAlignedTxps("FBtr0330654");
    rp.appendAlignedTxps("FBtr0300690");
    rp.appendAlignedTxps("FBtr0330654");

    EXPECT_EQ(rp.getRefTxps(), vec);
    EXPECT_EQ(rp.getAlignedTxps(), (std::set<Transcript*, ReadPair::TranscriptPtrComp>{&vec[2], &vec[1]}) );
    EXPECT_EQ(rp.getReadOne().first, "read111:A");
    EXPECT_EQ(rp.getReadOne().second, "AAAA");
    EXPECT_EQ(rp.getReadTwo().first, "read111:S");
    EXPECT_EQ(rp.getReadTwo().second, "CCCC");
}