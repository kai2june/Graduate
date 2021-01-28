#include <iostream>
#include <fstream>
#include "Transcript.hpp"

#include <gtest/gtest.h>

std::string transcript_filename("/mammoth/flux_simulator_data/Drosophila_melanogaster.BDGP6.80_transcripts.fa");

TEST(Transcript, constructor)
{   
    std::ifstream transcript_fa(transcript_filename, std::ios_base::in);
    while(!transcript_fa.eof())
    {
        Transcript txp(transcript_fa);
        std::cerr << txp;
        EXPECT_GT(txp.getLength(), 0);
    }
}

// int main(int argc, char* argv[]) {
//     ::testing::InitGoogleTest(&argc, argv);
//     std::cout << "argc:" << argc << std::endl;
//     return RUN_ALL_TESTS();
// }