#include <vector>
#include "EMAlgorithm.hpp"
#include "SAMToReadPairs.hpp"

#include <gtest/gtest.h>

// std::string transcript_filename("/mammoth/Graduate/unit_test/data/tiny_transcript.fa");
// std::string sam_filename("/mammoth/Graduate/unit_test/data/tiny_sorted.sam");
std::string transcript_filename("/mammoth/flux_simulator_data/Drosophila_melanogaster.BDGP6.80_transcripts.fa");
std::string sam_filename("/mammoth/flux_simulator_data/salmon_bias/0126_transcriptome/droso_10million_sorted.sam");

// TEST(EMAlgorithm, constructor)
// {
    // std::ifstream transcript_fa(transcript_filename), sam_file(sam_filename);
    // std::vector<Transcript> vec;
    // while(!transcript_fa.eof())
    // {
    //     Transcript txp(transcript_fa);
    //     vec.emplace_back(std::move(txp));
    // }
    // Header sam_header(sam_file);
    // SAMToReadPairs vector_readpairs(sam_header, sam_file, vec);
    // // for(const auto& i : vector_readpairs.getReadPairs())
    // //     std::cerr << i << std::endl;

    // EMAlgorithm em(vec, vector_readpairs);
    // /* init() */
    // EXPECT_EQ(vec[0].getTotalCount(), 4.0/3.0);
    // EXPECT_EQ(vec[1].getTotalCount(), 1.0/3.0);
    // EXPECT_EQ(vec[2].getTotalCount(), 4.0/3.0);

    // /* Estep */
    
// } 

int main()
{
    std::ifstream transcript_fa(transcript_filename), sam_file(sam_filename);
    std::vector<Transcript> vec;
    while(!transcript_fa.eof())
    {
        Transcript txp(transcript_fa);
        vec.emplace_back(std::move(txp));
    }
    std::cerr << "Finish reading transcript_fa." << std::endl;

    Header sam_header(sam_file);
    std::vector<SAM> vec_sam;
    std::size_t i{0};
    while(!sam_file.eof())
    {
        if(i++ % 100000 == 0)
            std::cerr << i << "th record to readpair" << std::endl;
        SAM sam(sam_header);
        SAM::get_obj(sam_file, sam);
        vec_sam.emplace_back(sam);
    }
    std::cerr << "Finish reading sam_file." << std::endl;

    SAMToReadPairs vector_readpairs(vec_sam, vec);

    EMAlgorithm em(vec, vector_readpairs);


    return 0;
}
