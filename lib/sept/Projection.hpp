// 2020.04.04 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/Data_t.hpp"
#include <sstream>
#include <stdexcept>

// Notes on projections
//
// A projection is a transformation of a data node which typically loses information.
// Examples:
// -    From an array A of length N, produce an array of type ArrayES(Type,N) whose elements
//      are the types of the elements of A.
// -    Extract an element of an Array or Map.
// -    Extract a subset of elements of an Array or Map.
// -    Given an array of type ArrayES(T1,N), produce ArrayES(T2,N).
// -    Given an array A, apply a function F to each element of A, producing an array of images
//      of those elements under F.  This function could itself be a projection.
//
// Passively, projections are useful for viewing a "slice" of given data.
//
// It is more useful to be able to integrate changes to a projected value back into the original
// value.  In order to reason about this, a couple of other concepts are necessary.
// -    A projection typically involves a loss of information.  Call this lost information the
//      "complement" to a projection.
// -    Let (P,C) denote the projection value and complement value of a projection operation.
//      In principle, it should be possible to reconstruct the source value whose projection
//      produced (P,C).  Call this process "embedding" (this term could be considered the opposite
//      of "projection" in a mathematical sense).
// -    In terms of embedding (P,C) to reproduce the original value, it's not strictly necessary
//      for C to be perfectly informationally independent from P.  In this sense, perhaps the word
//      "context" would be more appropriate for C.  In terms of using a (P',C) pair which represents
//      alterations P' of P, it's sufficient that any informational overlap of C with the projection
//      component not contradict the alteration of P to P'.  This will maintain the guarantee that in
//      princple it's still possible to embed (P',C) to produce a value of the type of the original
//      source data.
// -    The complement/context for a given projection depends on the projection itself, as does the
//      feasibility of embedding a (P,C) pair for that projection, or even the feasibility of
//      guaranteeing the independence of C from P with respect to alterations to P.

namespace sept {

// Base class for the Projection interface.
class ProjectionBase {
public:

    virtual ~ProjectionBase () { }

    virtual Data project (Data const &source, Data const &parameters) const = 0;
};

class EmbeddableProjectionBase : public ProjectionBase {
public:

    virtual ~EmbeddableProjectionBase () { }

    virtual Data project_with_complement (Data const &source, Data const &parameters) const = 0;
    virtual bool can_embed (Data const &projection_with_complement, Data const &target) const = 0;
    virtual void embed (Data const &projection_with_complement, Data &target) const = 0;
};

} // end namespace sept
