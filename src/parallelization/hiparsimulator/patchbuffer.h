#include <libgeodecomp/config.h>
#ifdef LIBGEODECOMP_FEATURE_MPI
#ifndef _libgeodecomp_parallelization_hiparsimulator_patchbuffer_h_
#define _libgeodecomp_parallelization_hiparsimulator_patchbuffer_h_

#include <deque>
#include <libgeodecomp/misc/stringops.h>
#include <libgeodecomp/parallelization/hiparsimulator/patchaccepter.h>
#include <libgeodecomp/parallelization/hiparsimulator/patchprovider.h>

namespace LibGeoDecomp {
namespace HiParSimulator {

template<class GRID_TYPE1, class GRID_TYPE2, class CELL_TYPE>
class PatchBuffer : 
        public PatchAccepter<GRID_TYPE1>, 
        public PatchProvider<GRID_TYPE2>
{
public:
    const static int DIM = GRID_TYPE1::DIMENSIONS;

    virtual void put(
        const GRID_TYPE1& grid, 
        const Region<DIM>& /*validRegion*/, 
        const unsigned& nanoStep) 
    {
        // fixme: check whether validRegion is actually a superset of
        // the currently requested region.
        if (requestedNanoSteps.empty() || nanoStep < requestedNanoSteps.front())
            return;
        if (nanoStep > requestedNanoSteps.front()) 
            throw std::logic_error("expected nano step was left out");
        storedRegions.push_back(
            GridVecConv::gridToVector<CELL_TYPE>(
                grid, 
                *requestedRegions.front()));
        storedNanoSteps.push_back(requestedNanoSteps.front());
        requestedNanoSteps.pop_front();
        requestedRegions.pop_front();
    }

    virtual long nextRequiredNanoStep()
    {
        return requestedNanoSteps.front();
    }

    virtual void get(
        GRID_TYPE2& destinationGrid, 
        const Region<DIM>& patchRegion, 
        const unsigned& nanoStep) 
    {
        if (storedRegions.empty())
            throw std::logic_error("no region available");

        if (storedNanoSteps.front() != nanoStep) 
            throw std::logic_error(
                std::string("requested time step doesn't match expected nano step.") 
                + " expected: " + StringConv::itoa(storedNanoSteps.front()) 
                + " is: " + StringConv::itoa(nanoStep));

        GridVecConv::vectorToGrid<CELL_TYPE>(
            storedRegions.front(), 
            destinationGrid, 
            patchRegion);

        storedRegions.pop_front();
        storedNanoSteps.pop_front();
    }

    void pushRequest(const Region<DIM> *region, const long& nanoStep)
    {
        requestedRegions.push_back(region);
        requestedNanoSteps.push_back(nanoStep);
    }

private:
    std::deque<const Region<DIM>*> requestedRegions;
    std::deque<long> requestedNanoSteps;
    std::deque<SuperVector<CELL_TYPE> > storedRegions;
    std::deque<unsigned> storedNanoSteps;
};

}
}

#endif
#endif
