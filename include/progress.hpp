#pragma once

#include <ostream>

#include <dottorrent/metafile.hpp>
#include <dottorrent/storage_hasher.hpp>
#include <dottorrent/storage_verifier.hpp>

void run_with_progress(std::ostream& os, dottorrent::storage_hasher& verifier, const dottorrent::metafile& m);

void run_with_simple_progress(std::ostream& os, dottorrent::storage_hasher& hasher, const dottorrent::metafile& m);

void run_quiet(std::ostream& os, dottorrent::storage_hasher& hasher, const dottorrent::metafile& m);

void run_with_progress(std::ostream& os, dottorrent::storage_verifier& verifier, const dottorrent::metafile& m);

void run_with_simple_progress(std::ostream& os, dottorrent::storage_verifier& verifier, const dottorrent::metafile& m);

void run_quiet(std::ostream& os, dottorrent::storage_verifier& verifier, const dottorrent::metafile& m);

void print_completion_statistics(std::ostream& os, const dottorrent::metafile& m, std::chrono::system_clock::duration duration);
