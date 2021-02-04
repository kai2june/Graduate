#include "EMAlgorithm.hpp"
#include "SAMToReadPairs.hpp"

#include <gtest/gtest.h>

std::string transcript_filename("/root/Graduate/unit_test/data/tiny_transcript.fa");
std::string sam_filename("/root/Graduate/unit_test/data/tiny_sorted.sam");

TEST(EMAlgorithm, constructor)
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
    // for(const auto& i : vector_readpairs.getReadPairs())
    //     std::cerr << i << std::endl;

    EMAlgorithm em(vector_readpairs);
}