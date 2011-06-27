#ifndef _libgeodecomp_parallelization_distributedsimulator_h_
#define _libgeodecomp_parallelization_distributedsimulator_h_

#include <libgeodecomp/io/parallelwriter.h>
#include <libgeodecomp/misc/displacedgrid.h>
#include <libgeodecomp/parallelization/simulator.h>

namespace LibGeoDecomp {

template<typename CELL_TYPE>
class ParallelWriter; 

template<typename CELL_TYPE>
class DistributedSimulator : public Simulator<CELL_TYPE>
{
public:
    typedef typename CELL_TYPE::Topology Topology;
    typedef DisplacedGrid<CELL_TYPE, Topology> GridType;
    typedef std::vector<boost::shared_ptr<ParallelWriter<CELL_TYPE> > > WriterVector;

    inline DistributedSimulator(Initializer<CELL_TYPE> *_initializer) : 
        Simulator<CELL_TYPE>(_initializer)
    {}

    /**
     * Returns the fragment of the current grid, which is available to
     * the current instance.
     */
    virtual void getGridFragment(
        const GridType **grid, 
        const Region<Topology::DIMENSIONS> **validRegion) = 0;

    /**
     * register @a writer to receive the Writer callbacks.
     */
    virtual void registerWriter(ParallelWriter<CELL_TYPE> *writer)
    {
        writers.push_back(boost::shared_ptr<ParallelWriter<CELL_TYPE> >(writer));
    }

    /**
     * @return currently registered Writers.
     */
    virtual WriterVector getWriters() const
    {
        return writers;
    }

protected:
    WriterVector writers;

};

}

#endif
