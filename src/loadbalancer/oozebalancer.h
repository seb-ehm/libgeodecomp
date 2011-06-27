#ifndef _libgeodecomp_loadbalancer_oozebalancer_h_
#define _libgeodecomp_loadbalancer_oozebalancer_h_

#include <cmath>
#include <libgeodecomp/loadbalancer/loadbalancer.h>

//fixme: check this number
#define GOLDEN_RATIO  1.6180339887498948482
#define EULERS_NUMBER 2.71828182845904523536

namespace LibGeoDecomp {

/**
 * The OozeBalancer is based on the (false) assumption that each node
 * is equally fast, that each item (see LoadBalancer) costs about the
 * same time to be computed (this is also false) and that these costs
 * don't change all too much during the runtime (again: plain wrong).
 * 
 * From all these questionable assumptions a possibly optimal new load
 * distribution is derived but, to keep errors at bounds, the
 * OozeBalancer will return a weighted linear combination of the old
 * distribution (currentLoads) and the new one is returned.
 */
class OozeBalancer : public LoadBalancer
{
    friend class OozeBalancerTest1;
    friend class OozeBalancerTest2;
public:
    /**
     * returns a new OozeBalancer instance, whose weighting for new
     * load distributions is set to @a newLoadWeight . A higher value
     * will increase balancing speed (in terms of migrated items per
     * balancing step), but also decrease accuracy (because of
     * unfulfilled preconditions, see above) and vice versa. A
     * quotient of the Golden Ratio and Eulers Number is believed to
     * be optimal for most applications.
     */
    OozeBalancer(double newLoadWeight = GOLDEN_RATIO / EULERS_NUMBER);

    virtual UVec balance(const UVec& currentLoads, const DVec& relativeLoads);

private:
    /**
     * Given that each node \f$n\f$ works on \f$currentLoads[n]\f$
     * items, that this takes him \f$relativeLoads[n]\f$ time and that
     * all nodes are roughly equally fast, then the optimal
     * distribution \f$dist\f$ can be determined by cutting the
     * sequence of items into \f$n\f$ subsets \f$s_i\f$ so that 
     *
     * \f[
     * \bigwedge_i \int_{t \in s_i} f(t) dt = l

     * \f]
     *
     * where \f$l\f$ is the target load per node 
     *
     * \f[
     * l = \frac{1}{n} \sum_{i=0}^{i<n} \cdot \mbox{currentLoads}[i]
     * \f]
     * 
     * and \f$f(t)\f$ it the load (or time) necessary to compute item \f$t\f$:
     *
     * \f[
     * f(t) = \frac{\mbox{relativeLoads}[a]}{\mbox{currentLoads}[a]}
     * \f]
     *
     * (\f$t\f$ is currently computed on node \f$a\f$.)
     */
    DVec expectedOptimalDistribution(const UVec& currentLoads, 
                                     const DVec& relativeLoads) const;

private:
    double _newLoadWeight;

    UVec equalize(const DVec& loads);
    DVec linearCombo(const UVec& loadsA, const DVec& loadsB);
};

};

#endif
