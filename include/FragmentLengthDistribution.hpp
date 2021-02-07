#pragma once
#include <cmath>

class FragmentLengthDistribution
{
  public:
    FragmentLengthDistribution() = default;
    FragmentLengthDistribution(double mean, double sdv)
        : mean_(mean), sdv_(sdv)
    {
        
    }

    double getMean() const
    {
        return mean_;
    }

    double getSDV() const
    {
        return sdv_;
    }

    double getPDF(double len) const
    { 
        return 1.0/(sdv_*std::sqrt(2.0*M_PI)) * std::exp( -1.0*(len-mean_)*(len-mean_) / (2.0*sdv_*sdv_) );
    }
    
  private:
    double mean_;
    double sdv_;
};