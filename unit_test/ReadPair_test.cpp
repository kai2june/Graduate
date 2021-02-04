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

    ReadPair rp(vec, std::make_pair("read111:A", "AAAA"), "FBtr0330654", std::make_pair(126, 192));
    rp.addAlignedTxps("FBtr0300690");
    rp.addPosTlen("FBtr0300690", std::make_pair(127, 193));
    rp.setReadTwo(std::make_pair("read111:S", "CCCC"));

    EXPECT_EQ(rp.getRefTxps(), vec);
    EXPECT_EQ(rp.getAlignedTxps(), (std::set<Transcript*, ReadPair::TranscriptPtrComp>{&vec[2], &vec[1]}) );
    EXPECT_EQ(rp.getReadOne(), (std::make_pair<std::string, std::string>("read111:A", "AAAA")) );
    EXPECT_EQ(rp.getReadTwo(), (std::make_pair<std::string, std::string>("read111:S", "CCCC")) );
    EXPECT_EQ(rp.getPosTlen().at("FBtr0330654"), (std::make_pair<uint32_t, uint32_t>(126, 192)) );
    EXPECT_EQ(rp.getPosTlen().at("FBtr0300690"), (std::make_pair<uint32_t, uint32_t>(127, 193)) );
    std::cerr << rp;
}