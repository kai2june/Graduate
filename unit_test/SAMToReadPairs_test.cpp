#include <iostream>
#include <fstream>
#include <string>
#include "SAMToReadPairs.hpp"
#include "SAMHeader.hpp"
#include "ReadPair.hpp"

#include <gtest/gtest.h>

std::string transcript_filename("/mammoth/Graduate/unit_test/data/tiny_transcript.fa");
std::string sam_filename("/mammoth/Graduate/unit_test/data/tiny_sorted.sam");

TEST(SAMToReadPairs, constructor)
{
    std::ifstream transcript_fa(transcript_filename), sam_file(sam_filename);
    std::vector<Transcript> vec;
    while(!transcript_fa.eof())
    {
        Transcript txp(transcript_fa);
        vec.emplace_back(std::move(txp));
    }
    Header sam_header(sam_file);
    SAMToReadPairs vector_readpairs(sam_header, sam_file, vec);

    ReadPair ans0(
        vec
      , std::set<Transcript*, ReadPair::TranscriptPtrComp>{&vec[0], &vec[1], &vec[2]}
      , std::make_pair("2L:7529-9484W:FBtr0330654:2:1844:125:316:A"
                     , "CCTACATACCCACCACATTTGACCTCCTCTCAGACGATGAGGAGTCGTCACAGAGAGTTGCCAACGCCGGGCCAT"
        )
      , std::make_pair("2L:7529-9484W:FBtr0330654:2:1844:125:316:S"
                     , "CCGAGGACCTGATTTACAAGAAGTCCATGGGCGAGCGGGATCAGCCACAGAGCTCAGAGCGGATCTCAATATTTA"
        )
      , "FBtr0330654"
      , std::make_pair<uint32_t, uint32_t>(126, 192)
    );
    ans0.addPosTlen("FBtr0300690", std::make_pair<uint32_t, uint32_t>(126, 192));
    ans0.addPosTlen("FBtr0300689", std::make_pair<uint32_t, uint32_t>(126, 192));

    ReadPair ans1(
        vec
      , std::set<Transcript*, ReadPair::TranscriptPtrComp>{&vec[0], &vec[2]}
      , std::make_pair("2L:7529-9484W:FBtr0330654:3:1844:943:1127:A"
                     , "CATAACTACCGAAGACATATGCACGTTTATTAATGGGAAATGGCTTAACGACGAGGTCATTAACTTTTACATGTC"
        )
      , std::make_pair("2L:7529-9484W:FBtr0330654:3:1844:943:1127:S"
                     , "TCGACGGTATGGCAACGAATATATTGATGTCTTTCGTACCCATTGAAAACGTTGTGGTGCTTGCGCTTTAAAATC"
        )
      , "FBtr0300689"
      , std::make_pair<uint32_t, uint32_t>(980, 185)
    );
    ans1.addPosTlen("FBtr0330654", std::make_pair<uint32_t, uint32_t>(944, 185));

    ReadPair ans2(
        vec
      , std::set<Transcript*, ReadPair::TranscriptPtrComp>{&vec[0], &vec[2]}
      , std::make_pair("2L:7529-9484W:FBtr0330654:4:1844:943:1127:A"
                     , "CATAACTACCGAAGACATATGCACGTTTATTAATGGGAAATGGCTTAACGACGAGGTCATTAACTTTTACATGTC"
        )
      , std::make_pair("2L:7529-9484W:FBtr0330654:4:1844:943:1127:S"
                     , "TCGACGGTATGGCAACGAATATATTGATGTCTTTCGTACCCATTGAAAACGTTGTGGTGCTTGCGCTTTAAAATC"
        )
      , "FBtr0300689"
      , std::make_pair<uint32_t, uint32_t>(980, 185)
    );
    ans2.addPosTlen("FBtr0330654", std::make_pair<uint32_t, uint32_t>(944, 185));

    EXPECT_EQ(vector_readpairs.getReadPairs()[0], ans0);
    EXPECT_EQ(vector_readpairs.getReadPairs()[1], ans1);
    EXPECT_EQ(vector_readpairs.getReadPairs()[2], ans2);
    for(const auto& i : vector_readpairs.getReadPairs())
        std::cerr << i << std::endl;
}

TEST(SAMToReadPairs, modify_constReturnFunction)
{
    std::vector<Transcript> vec;
    vec.emplace_back("first_txp");
    vec.emplace_back("second_txp");
    
    SAMToReadPairs rps(vec);
    for(const Transcript& tr :rps.accessRefTxps())
        std::cerr << tr;
}