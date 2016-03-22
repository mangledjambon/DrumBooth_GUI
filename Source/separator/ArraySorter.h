/*
  ==============================================================================

    ArraySorter.cpp
    Created: 29 Nov 2015 1:19:58pm
    Author:  Sean

  ==============================================================================
*/
#include <complex>
class MyArraySorter
{
public:
    static int compareElements(std::complex<float> a, std::complex<float> b)
    {
        if (a.real() < b.real())
            return -1;
        else if (a.real() > b.real())
            return 1;
        else // if a == b
            return 0;
    }
};
