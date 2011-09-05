#include <libgeodecomp/config.h>
#ifdef LIBGEODECOMP_FEATURE_MPI
#include <boost/assign/std/vector.hpp>
#include <libgeodecomp/mpilayer/typemaps.h>
#include <libgeodecomp/io/bovwriter.h>
#include <libgeodecomp/io/image.h>
#include <libgeodecomp/io/simpleinitializer.h>
#include <libgeodecomp/io/ppmwriter.h>
#include <libgeodecomp/io/simplecellplotter.h>
#include <libgeodecomp/io/simpleinitializer.h>
#include <libgeodecomp/io/tracingwriter.h>
#include <libgeodecomp/loadbalancer/oozebalancer.h>
#include <libgeodecomp/loadbalancer/tracingbalancer.h>
#include <libgeodecomp/mpilayer/mpilayer.h>
#include <libgeodecomp/parallelization/serialsimulator.h>
#include <libgeodecomp/parallelization/stripingsimulator.h>

using namespace boost::assign;
using namespace LibGeoDecomp;

class ConwayCell
{
public:   
    typedef Topologies::Cube<2>::Topology Topology;

    static inline unsigned nanoSteps() 
    { 
        return 1; 
    }

    ConwayCell(const bool& _alive = false) :
        alive(_alive)
    {}

    int countLivingNeighbors(const CoordMap<ConwayCell>& neighborhood)
    {
        int ret = 0;
        for (int y = -1; y < 2; ++y)
            for (int x = -1; x < 2; ++x)
                ret += neighborhood[Coord<2>(x, y)].alive;
        ret -= neighborhood[Coord<2>(0, 0)].alive;
        return ret;
    }

    void update(const CoordMap<ConwayCell>& neighborhood, const unsigned&)
    {
        int livingNeighbors = countLivingNeighbors(neighborhood);
        alive = neighborhood[Coord<2>(0, 0)].alive;
        if (alive) {
            alive = (2 <= livingNeighbors) && (livingNeighbors <= 3);
        } else {
            alive = (livingNeighbors == 3);
        }
    }

    bool alive;
};

class CellInitializer : public SimpleInitializer<ConwayCell>
{
public:
    CellInitializer() : SimpleInitializer<ConwayCell>(Coord<2>(160, 90), 800)
    {}

    virtual void grid(GridBase<ConwayCell, 2> *ret)
    {
        CoordBox<2> rect = ret->boundingBox();
        Coord<2>::Vector startCells;
        // start with a single glider...
        //          x
        //           x
        //         xxx
        startCells +=
            Coord<2>(11, 10),
            Coord<2>(12, 11),
            Coord<2>(10, 12), Coord<2>(11, 12), Coord<2>(12, 12);


        // ...add a Diehard pattern...
        //                x
        //          xx
        //           x   xxx
        startCells +=
            Coord<2>(55, 70), Coord<2>(56, 70), Coord<2>(56, 71), 
            Coord<2>(60, 71), Coord<2>(61, 71), Coord<2>(62, 71), 
            Coord<2>(61, 69);
        
        // ...and an Acorn pattern:
        //        x 
        //          x
        //       xx  xxx
        startCells +=
            Coord<2>(111, 30),
            Coord<2>(113, 31),
            Coord<2>(110, 32), Coord<2>(111, 32), 
            Coord<2>(113, 32), Coord<2>(114, 32), Coord<2>(115, 32);
        

        for (Coord<2>::Vector::iterator i = startCells.begin();
             i != startCells.end();
             ++i) 
            if (rect.inBounds(*i))
                ret->at(*i - rect.origin) = ConwayCell(true);
    }
};

class CellToColor {
public:
    Color operator()(const ConwayCell& cell)
    {
        int val = (int)cell.alive * 255;
        return Color(val, val, val);
    }
};

class StateSelector
{
public:
    typedef double VariableType;

    const double operator()(const ConwayCell& in)
    {
        return in.alive;
    }

    static std::string dataFormat()
    {
        return "DOUBLE";
    }
};

void runSimulation()
{
    int outputFrequency = 1;

    // StripingSimulator<ConwayCell> sim(
    //     new CellInitializer(),
    //     MPILayer().rank() ? 0 : new TracingBalancer(new OozeBalancer()), 
    //     10, 
    //     MPI::BOOL); 
    // new BOVWriter<ConwayCell, StateSelector>("game", &sim, outputFrequency, "alive");

    SerialSimulator<ConwayCell> sim(
        new CellInitializer());
    new PPMWriter<ConwayCell, SimpleCellPlotter<ConwayCell, CellToColor> >(
        "./gameoflife", 
        &sim,
        outputFrequency,
        12,
        12);

    new TracingWriter<ConwayCell>(&sim, 1);

    sim.run();
}

int main(int argc, char *argv[])
{
    MPI::Init(argc, argv);
    Typemaps::initializeMaps();

    runSimulation();
    
    MPI::Finalize(); 
    return 0;
}
#endif
