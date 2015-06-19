#include <cuda.h>
#include <cxxtest/TestSuite.h>
#include <libgeodecomp/io/testinitializer.h>
#include <libgeodecomp/io/testwriter.h>
#include <libgeodecomp/misc/testcell.h>
#include <libgeodecomp/parallelization/cudasimulator.h>

using namespace LibGeoDecomp;

namespace LibGeoDecomp {

class CudaSimulatorTest : public CxxTest::TestSuite
{
public:
    typedef TestCell<1, Stencils::VonNeumann<1, 1>, Topologies::Cube<1>::Topology,
                     TestCellHelpers::EmptyAPI, TestCellHelpers::NoOutput> TestCell1dCube;
    typedef TestCell<1, Stencils::Moore<1, 1>, Topologies::Torus<1>::Topology,
                     TestCellHelpers::EmptyAPI, TestCellHelpers::NoOutput> TestCell1dTorus;

    typedef TestCell<2, Stencils::VonNeumann<2, 1>, Topologies::Cube<2>::Topology,
                     TestCellHelpers::EmptyAPI, TestCellHelpers::NoOutput> TestCell2dCube;
    typedef TestCell<2, Stencils::Moore<2, 1>, Topologies::Torus<2>::Topology,
                     TestCellHelpers::EmptyAPI, TestCellHelpers::NoOutput> TestCell2dTorus;

    typedef TestCell<3, Stencils::VonNeumann<3, 1>, Topologies::Cube<3>::Topology,
                     TestCellHelpers::EmptyAPI, TestCellHelpers::NoOutput> TestCell3dCube;
    typedef TestCell<3, Stencils::Moore<3, 1>, Topologies::Torus<3>::Topology,
                     TestCellHelpers::EmptyAPI, TestCellHelpers::NoOutput> TestCell3dTorus;

    typedef TestInitializer<TestCell1dCube> TestInitializer1dCube;
    typedef TestInitializer<TestCell1dTorus> TestInitializer1dTorus;

    typedef TestInitializer<TestCell2dCube> TestInitializer2dCube;
    typedef TestInitializer<TestCell2dTorus> TestInitializer2dTorus;

    typedef TestInitializer<TestCell3dCube> TestInitializer3dCube;
    typedef TestInitializer<TestCell3dTorus> TestInitializer3dTorus;

    void test1dCube()
    {
        Coord<1> dim(777);
        int numSteps = 33;
        CudaSimulator<TestCell1dCube> sim(new TestInitializer1dCube(dim, numSteps));

        TestWriter<TestCell1dCube> *writer = new TestWriter<TestCell1dCube>(1, 0, numSteps);
        sim.addWriter(writer);

        sim.run();
        TS_ASSERT(writer->allEventsDone());
    }

    void test1dTorus()
    {
        Coord<1> dim(666);
        int startStep = 40;
        int endStep = 70;
        CudaSimulator<TestCell1dTorus> sim(new TestInitializer1dTorus(dim, endStep, startStep));

        TestWriter<TestCell1dTorus> *writer = new TestWriter<TestCell1dTorus>(3, startStep, endStep);
        sim.addWriter(writer);

        sim.run();
        TS_ASSERT(writer->allEventsDone());
    }

    void test2dCube()
    {
        Coord<2> dim(421, 351);
        int numSteps = 21;
        CudaSimulator<TestCell2dCube> sim(new TestInitializer2dCube(dim, numSteps));

        TestWriter<TestCell2dCube> *writer = new TestWriter<TestCell2dCube>(1, 0, numSteps);
        sim.addWriter(writer);

        sim.run();
        TS_ASSERT(writer->allEventsDone());
    }

    void test2dTorus()
    {
        Coord<2> dim(421, 351);
        int startStep = 35;
        int endStep = 60;
        CudaSimulator<TestCell2dTorus> sim(new TestInitializer2dTorus(dim, endStep, startStep));

        TestWriter<TestCell2dTorus> *writer = new TestWriter<TestCell2dTorus>(3, startStep, endStep);
        sim.addWriter(writer);

        sim.run();
        TS_ASSERT(writer->allEventsDone());
    }

    void test3dCube()
    {
        Coord<3> dim(50, 20, 10);
        int numSteps = 5;
        CudaSimulator<TestCell3dCube> sim(new TestInitializer3dCube(dim, numSteps));

        TestWriter<TestCell3dCube> *writer = new TestWriter<TestCell3dCube>(1, 0, numSteps);
        sim.addWriter(writer);

        sim.run();
        TS_ASSERT(writer->allEventsDone());
    }

    void test3DTorus()
    {
        Coord<3> dim(50, 20, 10);
        int startStep = 28;
        int endStep = 38;
        int ioPeriod = 3;
        CudaSimulator<TestCell3dTorus> sim(new TestInitializer3dTorus(dim, endStep, startStep));

        TestWriter<TestCell3dTorus> *writer = new TestWriter<TestCell3dTorus>(ioPeriod, startStep, endStep);
        sim.addWriter(writer);

        sim.run();
        TS_ASSERT(writer->allEventsDone());
    }
};

}
