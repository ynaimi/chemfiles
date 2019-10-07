// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/files/TRRFile.hpp"

#include "chemfiles/Error.hpp"
#include "chemfiles/utils.hpp"
using namespace chemfiles;

#define STRING_0(x) #x
#define STRING(x) STRING_0(x)
#define CHECK(x) check_trr_error((x), (STRING(x)))

TRRFile::TRRFile(std::string path, File::Mode mode)
    : File(std::move(path), mode, File::DEFAULT), handle_(nullptr) {
    const char* openmode;
    if (mode == File::READ) {
        openmode = "r";
        CHECK(read_trr_header(this->path().c_str(), &natoms_, &nframes_, &offsets_));
    } else if (mode == File::WRITE) {
        openmode = "w";
    } else {
        openmode = "a";
        // Do not check return value, because the file might not exist
        // But if it does, we need the number of atoms and number of frames for appending
        read_trr_header(this->path().c_str(), &natoms_, &nframes_, &offsets_);
    }

    handle_ = xdrfile_open(this->path().c_str(), openmode);
    if (!handle_) {
        throw file_error("could not open the file at {}", this->path());
    }
}

TRRFile::~TRRFile() {
    xdrfile_close(handle_);
    free(offsets_);
}

unsigned long TRRFile::nframes() const { return nframes_; }

int64_t TRRFile::offset(size_t step) const { return offsets_[step]; }

int TRRFile::natoms() const { return natoms_; }

void TRRFile::set_natoms(int natoms) { natoms_ = natoms; }

void chemfiles::check_trr_error(int status, const std::string& function) {
    switch (status) {
    case exdrHEADER:
    case exdrSTRING:
    case exdrDOUBLE:
    case exdrINT:
    case exdrFLOAT:
    case exdrUINT:
    case exdr3DX:
    case exdrCLOSE:
    case exdrMAGIC:
    case exdrNOMEM:
    case exdrENDOFFILE:
    case exdrFILENOTFOUND:
    case exdrNR:
        throw file_error("error while calling {} in the TRR library: {}", function,
                         std::string(exdr_message[status]));
    case exdrOK:
        // Do nothing, this is good
        break;
    default:
        throw file_error("unknown status code from TRR library: {}", std::to_string(status));
        break;
    }
}