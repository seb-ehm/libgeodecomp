#include <libgeodecomp/config.h>
#ifdef LIBGEODECOMP_FEATURE_OPENCL

#ifndef _libgeodecomp_parallelization_hiparsimulator_openclstepper_h_
#define _libgeodecomp_parallelization_hiparsimulator_openclstepper_h_

#include <CL/cl.h>
#define __CL_ENABLE_EXCEPTIONS
#include <libgeodecomp/parallelization/hiparsimulator/cl.hpp>
#include <boost/shared_ptr.hpp>

#include <libgeodecomp/misc/displacedgrid.h>
#include <libgeodecomp/parallelization/hiparsimulator/stepperhelper.h>

namespace LibGeoDecomp {
namespace HiParSimulator {

template<typename CELL_TYPE>
class OpenCLStepper : public StepperHelper<
    DisplacedGrid<CELL_TYPE, typename CELL_TYPE::Topology, false> >
{
public:
    const static int DIM = CELL_TYPE::Topology::DIMENSIONS;
    friend class OpenCLStepperTest;
    typedef DisplacedGrid<
        CELL_TYPE, typename CELL_TYPE::Topology, false> GridType;
    typedef class StepperHelper<GridType> ParentType;
    typedef PartitionManager< 
        DIM, typename CELL_TYPE::Topology> MyPartitionManager;

    inline OpenCLStepper(
        boost::shared_ptr<MyPartitionManager> _partitionManager,
        boost::shared_ptr<Initializer<CELL_TYPE> > _initializer) :
        ParentType(_partitionManager, _initializer)
    {
        // fixme: openCL selection routine
        int platformId = 0;
        int deviceId = 0;
        try {
            std::vector<cl::Platform> platforms;
            cl::Platform::get(&platforms);
            std::vector<cl::Device> devices;
            platforms.at(platformId).getDevices(CL_DEVICE_TYPE_ALL, &devices);
            cl::Device usedDevice = devices.at(deviceId);
            context = cl::Context(devices);
            cmdQueue = cl::CommandQueue(context, usedDevice);
        }
        /*catch (cl::Error &err) {
            std::cerr << "OpenCL error: " << err.what() << "(" << err.err() << ")" << std::endl;
        }*/
        curStep = initializer().startStep();
        curNanoStep = 0;
        initGrids();
    }

    inline virtual std::pair<int, int> currentStep() const
    {
        return std::make_pair(curStep, curNanoStep);
    }

    inline virtual void update(int nanoSteps) 
    {
        // fixme: implement me (later)
        
    }

    inline virtual const GridType& grid() const
    {
        cmdQueue.enqueueReadBuffer(deviceGrid, true, 0, 
            hostGrid->getDimensions().prod() * sizeof(CELL_TYPE), hostGrid->baseAddress());
        return *hostGrid;
    }


private:
    int curStep;
    int curNanoStep;
    boost::shared_ptr<GridType> hostGrid;

    cl::Buffer deviceGrid;
    cl::Context context;
    cl::CommandQueue cmdQueue;

    inline void initGrids()
    {
        const CoordBox<DIM>& gridBox = 
            partitionManager().ownRegion().boundingBox();
        hostGrid.reset(new GridType(gridBox, CELL_TYPE()));
        initializer().grid(&*hostGrid);
        
        deviceGrid = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
            hostGrid->getDimensions().prod() * sizeof(CELL_TYPE), hostGrid->baseAddress());
    }

    inline MyPartitionManager& partitionManager() 
    {
        return this->getPartitionManager();
    }

    inline const MyPartitionManager& partitionManager() const
    {
        return this->getPartitionManager();
    }

    inline Initializer<CELL_TYPE>& initializer() 
    {
        return this->getInitializer();
    }

    inline const Initializer<CELL_TYPE>& initializer() const
    {
        return this->getInitializer();
    }
};

}
}

#endif
#endif
