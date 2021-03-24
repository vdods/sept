// 2020.04.04 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/Projection.hpp"

namespace sept {
namespace proj {

class ElementExtraction : public EmbeddableProjectionBase {
public:

    virtual ~ElementExtraction () { }

    virtual Data project (Data const &source, Data const &parameters) const override;
    virtual Data project_with_complement (Data const &source, Data const &parameters) const override;
    virtual bool can_embed (Data const &projection_with_complement, Data const &target) const override;
    virtual void embed (Data const &projection_with_complement, Data &target) const override;

private:

    static uint64_t compute_absolute_index (uint64_t array_size, int64_t cyclic_index);
};

} // end namespace proj
} // end namespace sept
