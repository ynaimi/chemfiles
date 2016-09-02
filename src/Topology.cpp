/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemfiles/Error.hpp"
#include "chemfiles/Topology.hpp"

using namespace chemfiles;

void Topology::resize(size_t natoms) {
    for (auto bond : bonds()) {
        if (bond[0] >= natoms || bond[1] >= natoms) {
            throw Error(
                "Can not resize the topology to " + std::to_string(natoms) +
                " as there is a bond between atoms " + std::to_string(bond[0]) +
                "-" + std::to_string(bond[1]) + ".");
        }
    }
    atoms_.resize(natoms, Atom(Atom::UNDEFINED));
}

void Topology::append(Atom atom) {
    atoms_.emplace_back(std::move(atom));
}

void Topology::reserve(size_t natoms) {
    atoms_.reserve(natoms);
}

void Topology::remove(size_t idx) {
    atoms_.erase(atoms_.begin() + static_cast<std::ptrdiff_t>(idx));
    auto bonds = connect_.bonds();
    for (auto& bond : bonds) {
        if (bond[0] == idx || bond[1] == idx) {
            connect_.remove_bond(bond[0], bond[1]);
        }
    }
}

std::vector<Bond> Topology::bonds() const {
    std::vector<Bond> res;
    res.insert(res.begin(), connect_.bonds().begin(), connect_.bonds().end());
    return res;
}

std::vector<Angle> Topology::angles() const {
    std::vector<Angle> res;
    res.insert(res.begin(), connect_.angles().begin(), connect_.angles().end());
    return res;
}

std::vector<Dihedral> Topology::dihedrals() const {
    std::vector<Dihedral> res;
    res.insert(res.begin(), connect_.dihedrals().begin(), connect_.dihedrals().end());
    return res;
}

bool Topology::isbond(size_t i, size_t j) const {
    auto bonds = connect_.bonds();
    auto pos = bonds.find(Bond(i, j));
    return pos != bonds.end();
}

bool Topology::isangle(size_t i, size_t j, size_t k) const {
    auto angles = connect_.angles();
    auto pos = angles.find(Angle(i, j, k));
    return pos != angles.end();
}

bool Topology::isdihedral(size_t i, size_t j, size_t k, size_t m) const {
    auto dihedrals = connect_.dihedrals();
    auto pos = dihedrals.find(Dihedral(i, j, k, m));
    return pos != dihedrals.end();
}
