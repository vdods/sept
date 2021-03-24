// 2020.04.04 - Victor Dods

#include "sept/proj/ElementExtraction.hpp"

#include <lvd/util.hpp>
#include "sept/ArrayTerm.hpp"

namespace sept {
namespace proj {

Data ElementExtraction::project (Data const &source, Data const &parameters) const {
    ArrayTerm_c const &source_array = source.cast<ArrayTerm_c const &>();
    if (parameters.can_cast<int64_t>()) {
        return source_array[compute_absolute_index(source_array.size(), parameters.cast<int64_t>())];
    } else if (parameters.can_cast<ArrayTerm_c>()) {
        auto const &parameters_array = parameters.cast<ArrayTerm_c const &>();
        auto projection = Array(); // TODO: Could potentially figure out what narrowed type to use
        for (auto const &index : parameters_array) {
            if (!index.can_cast<int64_t>())
                throw std::runtime_error(LVD_FMT("expected int64_t elements for parameters to ElementExtraction, but got " << index.type().name()));
            projection.elements().emplace_back(source_array[compute_absolute_index(source_array.size(), index.cast<int64_t>())]);
        }
        return projection;
    } else {
        throw std::runtime_error(LVD_FMT("ElementExtraction does not support parameters type " << parameters.type().name()));
    }
}

Data ElementExtraction::project_with_complement (Data const &source, Data const &parameters) const {
    return make_array(project(source, parameters), parameters);
}

bool ElementExtraction::can_embed (Data const &projection_with_complement, Data const &target) const {
    return true;
}

void ElementExtraction::embed (Data const &projection_with_complement, Data &target) const {
    ArrayTerm_c const &array = projection_with_complement.cast<ArrayTerm_c const &>();
    // TODO: Make this type check more precise.
    if (array.size() != 2 || (array[1].type() != typeid(int64_t) && array[1].type() != typeid(ArrayTerm_c)))
        throw std::runtime_error(LVD_FMT("expected projection_with_complement to be ArrayTerm_c(value, int64_t(N)) for some N or ArrayTerm_c(value, ArrayTerm_c(...), but it was " << array));

    if (target.type() != typeid(ArrayTerm_c))
        throw std::runtime_error(LVD_FMT("expected target to be of type ArrayTerm_c, but it was of type " << target.type().name()));
    ArrayTerm_c &target_array = target.cast<ArrayTerm_c &>();

    // TODO: This would be much much easier if there were numpy-style indexing and slicing.

    Data const &extracted_element = array[0];
    if (array[1].can_cast<int64_t>()) {
        int64_t cyclic_index = array[1].cast<int64_t>();
        uint64_t absolute_index = compute_absolute_index(target_array.size(), cyclic_index);
        target_array[absolute_index] = extracted_element;
    } else {
        assert(array[0].can_cast<ArrayTerm_c>());
        assert(array[1].can_cast<ArrayTerm_c>());
        auto const &extracted_array = array[0].cast<ArrayTerm_c const &>();
        auto const &index_array = array[1].cast<ArrayTerm_c const &>();
        if (extracted_array.size() != index_array.size())
            throw std::runtime_error(LVD_FMT("mismatching in number of indices: " << LVD_REFLECT(extracted_array.size()) << " vs " << LVD_REFLECT(index_array.size())));
        for (size_t i = 0; i < index_array.size(); ++i) {
            auto const &index = index_array[i];
            auto const &extracted = extracted_array[i];
            // TODO: Improve this type check
            if (!index.can_cast<int64_t>())
                throw std::runtime_error(LVD_FMT("expected array indices to be int64_t, but got " << index.type().name()));
            int64_t cyclic_index = index.cast<int64_t>();
            uint64_t absolute_index = compute_absolute_index(target_array.size(), cyclic_index);
            target_array[absolute_index] = extracted;
        }
    }
}

uint64_t ElementExtraction::compute_absolute_index (uint64_t array_size, int64_t cyclic_index) {
    uint64_t absolute_index = lvd::remainder(cyclic_index, array_size);
    assert(absolute_index >= 0);
    assert(absolute_index < array_size);
    return absolute_index;
}

} // end namespace proj
} // end namespace sept
