#ifndef LIBGEODECOMP_MISC_GRIDBASE_H
#define LIBGEODECOMP_MISC_GRIDBASE_H

#include <libgeodecomp/misc/coord.h>
#include <libgeodecomp/misc/coordbox.h>

namespace LibGeoDecomp {

/**
 * This is an abstract base class for all grid classes. It's generic
 * because all methods are virtual, but not very efficient -- for the
 * same reason.
 */
template<typename CELL, int DIMENSIONS>
class GridBase
{
public:
    typedef CELL CellType;
    const static int DIM = DIMENSIONS;

    class ConstIterator
    {
    public:
	ConstIterator(const GridBase<CELL, DIM> *grid, const Coord<DIM>& origin) :
	    grid(grid),
	    cursor(origin)
	{
	    cell = grid->get(cursor);
	}

	const CELL& operator*() const
	{
	    return cell;
	}

	const CELL *operator->() const
	{
	    return &cell;
	}

	ConstIterator& operator>>(CELL& target)
	{
	    target = cell;
	    ++(*this);
	    return *this;
	}

	void operator++()
	{
	    ++cursor.x();
	    cell = grid->get(cursor);
	}

    private:
	const GridBase<CELL, DIM> *grid;
	Coord<DIM> cursor;
	CELL cell;
    };

    virtual ~GridBase()
    {}

    // fixme: add functions for getting/setting streak of cells. use these in mpiio.h, writers, steerers, mpilayer...
    virtual void set(const Coord<DIM>&, const CELL&) = 0;
    virtual CELL get(const Coord<DIM>&) const = 0;
    virtual void setEdge(const CELL&) = 0;
    virtual const CELL& getEdge() const = 0;
    virtual CoordBox<DIM> boundingBox() const = 0;

    ConstIterator at(const Coord<DIM>& coord) const
    {
	return ConstIterator(this, coord);
    }

    Coord<DIM> dimensions() const
    {
        return boundingBox().dimensions;
    }


    bool operator==(const GridBase<CELL, DIMENSIONS>& other) const
    {
        if (getEdge() != other.getEdge()) {
            return false;
        }

        CoordBox<DIM> box = boundingBox();
        if (box != other.boundingBox()) {
            return false;
        }

        for (typename CoordBox<DIM>::Iterator i = box.begin(); i != box.end(); ++i) {
            if (get(*i) != other.get(*i)) {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const GridBase<CELL, DIMENSIONS>& other) const
    {
        return !(*this == other);
    }
};

}

#endif
