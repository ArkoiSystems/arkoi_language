#pragma once

template <typename T>
bool OrderedSet<T>::insert(const T& value) {
    if (this->contains(value)) return false;

    _index_map[value] = _vector.size();
    _vector.push_back(value);

    return true;
}

template <typename T>
bool OrderedSet<T>::erase(const T& value) {
    auto it = _index_map.find(value);
    if (it == _index_map.end()) return false;

    size_t index = it->second;
    
    // Swap with the last element to avoid shifting all elements
    if (index != _vector.size() - 1) {
        const T& last = _vector.back();
        _vector[index] = last;
        _index_map[last] = index;  // Update the swapped element's index
    }
    
    _vector.pop_back();
    _index_map.erase(it);

    return true;
}

template <typename T>
bool OrderedSet<T>::contains(const T& value) const {
    return _index_map.contains(value);
}

template <typename T>
void OrderedSet<T>::clear() {
    _vector.clear();
    _index_map.clear();
}

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================================
