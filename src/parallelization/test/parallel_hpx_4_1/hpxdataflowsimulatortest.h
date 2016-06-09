#include <cxxtest/TestSuite.h>
#include <hpx/hpx.hpp>

#include <libgeodecomp/io/initializer.h>
#include <libgeodecomp/misc/apitraits.h>
#include <libgeodecomp/storage/unstructuredgrid.h>
#include <libgeodecomp/parallelization/hpxdataflowsimulator.h>

using namespace LibGeoDecomp;

namespace LibGeoDecomp {

class DummyMessage
{
public:
    DummyMessage(int senderID = -1,
                 int receiverID = -1,
                 int timestep = -1,
                 int data = -1) :
        senderID(senderID),
        receiverID(receiverID),
        timestep(timestep),
        data(data)
    {}

    template<typename ARCHIVE>
    void serialize(ARCHIVE& archive, int)
    {
        archive & senderID;
        archive & receiverID;
        archive & timestep;
        archive & data;
    }

    int senderID;
    int receiverID;
    int timestep;
    int data;
};

}

LIBGEODECOMP_REGISTER_HPX_COMM_TYPE(DummyMessage)

namespace LibGeoDecomp {

class DummyModel
{
public:
    class API :
        public APITraits::HasUnstructuredTopology,
        public APITraits::HasCustomMessageType<DummyMessage>
    {};

    DummyModel(int id = -1, const std::vector<int>& neighbors = std::vector<int>()) :
        id(id),
        neighbors(neighbors)
    {}

    // fixme: use move semantics here
    template<typename HOOD>
    void update(
        HOOD& hood,
        // fixme: make sure nanosteps are being issued here, not global steps:
        int nanoStep)
    {
        // fixme: don't check for nanoStep, but step. also: why 1, not 0?
        if (nanoStep > 1) {
            for (auto&& neighbor: neighbors) {
                // fixme: use actual step AND nanoStep here
                int expectedData = 10000 * (nanoStep - 1) + neighbor * 100 + id;
                TS_ASSERT_EQUALS(hood[neighbor].data,       expectedData);
                TS_ASSERT_EQUALS(hood[neighbor].timestep,   (nanoStep - 1));
                TS_ASSERT_EQUALS(hood[neighbor].senderID,   neighbor);
                TS_ASSERT_EQUALS(hood[neighbor].receiverID, id);
            }
        }

        for (auto&& neighbor: neighbors) {
            // fixme: use actual step AND nanoStep here
            DummyMessage dummyMessage(id, neighbor, nanoStep, 10000 * nanoStep + 100 * id + neighbor);
            // fixme: strip this from signature
            hood.send(neighbor, dummyMessage, nanoStep);
        }
    }

private:
    int id;
    std::vector<int> neighbors;
};

 class DummyInitializer : public Initializer<DummyModel>
 {
 public:
     DummyInitializer(int gridSize, int myMaxSteps) :
         gridSize(gridSize),
	 myMaxSteps(myMaxSteps)
     {}

     void grid(GridBase<DummyModel, 1> *grid)
     {
	 CoordBox<1> box = grid->boundingBox();

	 for (CoordBox<1>::Iterator i = box.begin(); i != box.end(); ++i) {
             DummyModel cell(i->x(), getNeighbors(i->x()));
	     grid->set(*i, cell);
	 }
     }

    virtual Coord<1> gridDimensions() const
    {
	return Coord<1>(gridSize);
    }

    unsigned startStep() const
    {
	return 0;
    }

    unsigned maxSteps() const
    {
	return myMaxSteps;
    }

    boost::shared_ptr<Adjacency> getAdjacency(const Region<1>& region) const
    {
	boost::shared_ptr<Adjacency> adjacency(new RegionBasedAdjacency());

	for (Region<1>::Iterator i = region.begin(); i != region.end(); ++i) {
            std::vector<int> neighbors = getNeighbors(i->x());
            for (auto&& neighbor: neighbors) {
                adjacency->insert(i->x(), neighbor);
            }
	}

	return adjacency;
    }

 private:
     int gridSize;
     int myMaxSteps;

     std::vector<int> getNeighbors(int id) const
     {
         std::vector<int> neighbors;

         if (id > 0) {
             neighbors << (id - 1);
         }
         if (id > 1) {
             neighbors << (id - 2);
         }

         if (id < (gridSize - 1)) {
             neighbors << (id + 1);
         }
         if (id < (gridSize - 2)) {
             neighbors << (id + 2);
         }

         return neighbors;
     }
};

class HpxDataflowSimulatorTest : public CxxTest::TestSuite
{
public:
    void setUp()
    {
    }

    void testBasic()
    {
        Initializer<DummyModel> *initializer = new DummyInitializer(50, 13);
        HPXDataflowSimulator<DummyModel> sim(initializer);
        sim.run();
    }
};

}
