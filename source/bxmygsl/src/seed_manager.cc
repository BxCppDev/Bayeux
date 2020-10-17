/* seed_manager.cc
 *
 * Copyright (C) 2011-2020 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <mygsl/seed_manager.h>

// Standard library:
#include <ctime>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <map>
#include <limits>
#include <cstdlib>
#include <random> /// only with new "-std=c++0x" support

// Third party:
// - Boost:
// Wrap Boost's tokenizer header
// This header, causes "unused parameter" warnings from its
// static void assign(Iterator b, Iterator e, Token &t)
// function.
// This should be an innocuous warning, so remove diagnostic for this
// header only.
// We only use clang pragmas for now because GCC's are highly version
// dependent - so need a bit more thought.
// To be removed when Boost fix their headers...
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include <boost/tokenizer.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#include <boost/filesystem.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/utils.h>

// This project:
#include <mygsl/random_utils.h>
#include <mygsl/rng.h>

namespace mygsl {
  
  // static
  bool seed_manager::seed_is_valid(int32_t seed_)
  {
    return random_utils::is_valid_seed(seed_);
  }

  seed_manager::seed_manager()
  {
    _init_seed_flags_ = INIT_SEED_FROM_UNDEFINED;
    return;
  }

  seed_manager::~seed_manager()
  {
    return;
  }

  void seed_manager::clear()
  {
    _dict_.clear();
    return;
  }

  size_t seed_manager::size() const
  {
    return _dict_.size();
  }

  bool seed_manager::empty() const
  {
    return _dict_.empty();
  }

  void seed_manager::all_time_seeds()
  {
    all_auto_seeds();
    return;
  }

  void seed_manager::all_auto_seeds()
  {
    for (dict_type::iterator i = _dict_.begin();
         i != _dict_.end(); i++) {
      i->second = random_utils::SEED_AUTO;
    }
    return;
  }

  void seed_manager::_set_init_seed_flags_()
  {
    if (_init_seed_flags_ == INIT_SEED_FROM_UNDEFINED) {
      _init_seed_flags_ = INIT_SEED_FROM_DEFAULT;
    }
    return;
  }

  int32_t seed_manager::_set_seed_for_seeds_()
  {
    _set_init_seed_flags_();
    int32_t seed = 0;
    if (_init_seed_flags_ & INIT_SEED_FROM_CURRENT_TIME) {
      seed += (int32_t) time(0);
    }
    if (_init_seed_flags_ & INIT_SEED_FROM_CURRENT_PID) {
      seed += (int32_t) getpid();
    }
    if (_init_seed_flags_ & INIT_SEED_FROM_URANDOM) {
      std::string dev_urandom = "/dev/urandom";
      DT_THROW_IF(! boost::filesystem::exists(dev_urandom.c_str()),
                  std::runtime_error,
                  "No '" << dev_urandom << "' entropy source on this host !");
      unsigned int useed = 0;
      FILE *urandom;
      urandom = fopen(dev_urandom.c_str(), "r");
      size_t sz = fread(&useed, sizeof(useed), 1, urandom);
      DT_THROW_IF(sz != 1, std::runtime_error,
                  "fread returned unexpected number of items (sz=" << sz << " != " << 1 << ") !");
      seed += (int32_t) useed & mygsl::random_utils::SEED_MAX;
      fclose(urandom);
    }
    if (_init_seed_flags_ & INIT_SEED_FROM_RANDOM_DEVICE) {
      std::random_device rd;
      seed += (int32_t) rd();
    }
    seed &= mygsl::random_utils::SEED_MAX;
    return seed;
  }

  void seed_manager::transform_time_seeds(bool allow_duplication_)
  {
    int32_t seed = _set_seed_for_seeds_();
    mygsl::rng r;
    r.initialize(mygsl::rng::DEFAULT_RNG_TYPE, seed);
    for (dict_type::iterator i = _dict_.begin(); i != _dict_.end(); i++) {
      // Only SEED_AUTO records are changed :
      if (i->second == random_utils::SEED_AUTO) {
        // Randomize the seed :
        i->second = 1 + r.uniform_int(std::numeric_limits<int32_t>::max() / 2);
      }
    }
    if (! allow_duplication_) {
      _ensure_different_seeds_(&r);
    }
    return;
  }

  void seed_manager::ensure_different_seeds()
  {
    int32_t seed = _set_seed_for_seeds_();
    mygsl::rng r;
    r.initialize(mygsl::rng::DEFAULT_RNG_TYPE ,seed);
    _ensure_different_seeds_(&r);
    return;
  }


  bool seed_manager::has_invalid_seeds() const
  {
    for (dict_type::const_iterator i = _dict_.begin(); i != _dict_.end(); i++) {
      int32_t seed = i->second;
      if (seed == random_utils::SEED_INVALID) {
        return true;
      }
    }
    return false;
  }


  bool seed_manager::has_time_seeds() const
  {
    return has_auto_seeds();
  }


  bool seed_manager::has_auto_seeds() const
  {
    for (dict_type::const_iterator i = _dict_.begin(); i != _dict_.end(); i++) {
      int32_t seed = i->second;
      if (seed == random_utils::SEED_AUTO) {
        return true;
      }
    }
    return false;
  }


  bool seed_manager::all_different_seeds() const
  {
    std::map<int32_t, int> mseeds;
    for (dict_type::const_iterator i = _dict_.begin(); i != _dict_.end(); i++) {
      int32_t seed = i->second;
      if (mseeds.find(seed) == mseeds.end()) {
        mseeds[seed] = 0;
      }
      mseeds[seed] += 1;
      if (mseeds[seed] > 1) {
        return false;
      }
    }
    return true;
  }

  void seed_manager::_ensure_different_seeds_(mygsl::rng * random_)
  {
    int32_t seed = (int32_t) time(0);
    mygsl::rng * r = random_;
    mygsl::rng r2;
    if (r == nullptr) {
      r = &r2;
    }
    if (! r->is_initialized()) {
      r->initialize(mygsl::rng::DEFAULT_RNG_TYPE ,seed);
    }

    typedef std::multimap<int32_t, std::string> seed_mmap_type;
    typedef seed_mmap_type::iterator            iter_type;

    seed_mmap_type mmseeds;
    for (dict_type::const_iterator i = _dict_.begin(); i != _dict_.end(); i++) {
      const std::string & l = i->first;
      int32_t s = i->second;
      mmseeds.insert(std::make_pair(s,l));
    }
    // showing contents:
    iter_type it, it2;
    for (it = mmseeds.begin(); it != mmseeds.end(); it = it2) {
      int32_t theKeySeed = (*it).first;
      std::pair<iter_type, iter_type> keyRange = mmseeds.equal_range(theKeySeed);

      // Tho it seemingly does nothing, this loop *must* be run otherwise test
      // deadlocks/infinite loops(?)
      for (it2 = keyRange.first;  it2 != keyRange.second;  ++it2) {
        //DT_LOG_TRACE(get_logging_priority(), '"' << (*it2).second << '"' << " ");
      }
      if ((theKeySeed != random_utils::SEED_INVALID)
          && (mmseeds.count(theKeySeed) > 1)) {
        int count = 0;
        for (it2 = keyRange.first; it2 != keyRange.second;  ++it2) {
          const std::string & prng_label = (*it2).second;
          bool do_it = false;
          if (count > 0 || theKeySeed == random_utils::SEED_AUTO) {
            do_it = true;
          }
          if (do_it) {
            while (true) {
              int32_t theNewSeed = 1 + r->uniform_int(std::numeric_limits<int32_t>::max() / 2);
              if (mmseeds.count(theNewSeed) == 0) {
                update_seed(prng_label, theNewSeed);
                break;
              }
            }
          }
          count++;
        }
      }
    }
  }

  void seed_manager::invalidate()
  {
    for (dict_type::iterator i = _dict_.begin(); i != _dict_.end(); i++) {
      i->second = random_utils::SEED_INVALID;
    }
    return;
  }


  void seed_manager::get_labels(std::vector<std::string> & labels_) const
  {
    labels_.clear();
    labels_.reserve(_dict_.size());
    for (dict_type::const_iterator i = _dict_.begin(); i != _dict_.end(); i++) {
      labels_.push_back(i->first);
    }
    return;
  }

  bool seed_manager::has_seed(const std::string & label_) const
  {
    return _dict_.find(label_) != _dict_.end();
  }

  int32_t seed_manager::get_seed(const std::string & label_) const
  {
    dict_type::const_iterator found = _dict_.find(label_);
    DT_THROW_IF(found == _dict_.end(), std::logic_error, "No seed with label '" << label_ << "' is defined !");
    return found->second;
  }

  void seed_manager::update_seed(const std::string & label_, int32_t seed_)
  {
    dict_type::iterator found = _dict_.find(label_);
    if (found == _dict_.end()) {
      add_seed(label_, seed_);
    } else {
      DT_THROW_IF(! seed_is_valid(seed_),
                  std::logic_error,
                  "Seed value '" << seed_ << "' is not allowed for PRNG '" << label_ << "'!");
      found->second = seed_;
    }
    return;
  }

  void seed_manager::invalidate_seed(const std::string & label_)
  {
    dict_type::iterator found = _dict_.find(label_);
    if (found == _dict_.end()) {
      _set_seed_(label_, random_utils::SEED_INVALID);
    } else {
      found->second = random_utils::SEED_INVALID;
    }
    return;
  }

  void seed_manager::add_seed(const std::string & label_, int32_t seed_)
  {
    DT_THROW_IF(has_seed(label_),
                std::logic_error, "Label '" << label_ << "' is already used !");
    DT_THROW_IF(! seed_is_valid(seed_), std::logic_error,
                "Seed value '" << seed_ << "' is not allowed !");
    _set_seed_(label_, seed_);
    return;
  }

  void seed_manager::_set_seed_(const std::string & label_, int32_t seed_)
  {
    DT_THROW_IF(!datatools::name_validation(label_, datatools::NV_INSTANCE),
                std::logic_error,
                "Label '" << label_ << "' is not valid !");
    int32_t seed = random_utils::SEED_INVALID;
    if (seed_is_valid(seed_)) {
      seed = seed_;
    }
    _dict_[label_] = seed;
    return;
  }

  void seed_manager::set_init_seed_flags(uint32_t f_)
  {
    _init_seed_flags_ = f_;
    return;
  }

  uint32_t seed_manager::get_init_seed_flags() const
  {
    return _init_seed_flags_;
  }


  void seed_manager::dump(std::ostream & out_) const
  {
    out_ << "mygsl::seed_manager:" << std::endl;
    for (dict_type::const_iterator i = _dict_.begin(); i != _dict_.end(); ++i) {
      out_ << "|-- ";
      out_.width(25) ;
      out_ << i->first << " : ";
      out_.width(10);
      out_ << i->second << " ";
      if (i->second == mygsl::random_utils::SEED_AUTO) {
        out_ << "[set randomly]";
      } else if(i->second == mygsl::random_utils::SEED_INVALID) {
        out_ << "[undefined]";
      } else {
        out_ << "[set by value]";
      }
      out_ << std::endl;
    }
    out_ << "`-- " << "End" << std::endl;
    return;
  }

  // friend :
  std::ostream & operator<<(std::ostream & out_, const seed_manager & sm_)
  {
    out_ << '{';
    for (seed_manager::dict_type::const_iterator i = sm_._dict_.begin();
         i != sm_._dict_.end(); ++i) {
      if (i != sm_._dict_.begin()) out_ << "; ";
      out_ << i->first << "=" << i->second;
    }
    out_ << '}';
    return out_;
  }

  // friend :
  std::istream & operator>>(std::istream & in_, seed_manager & sm_)
  {
    sm_._dict_.clear();
    in_ >> std::ws;
    char c = 0;
    in_.get(c);
    if (c != '{') {
      in_.setstate(std::ios::failbit);
      return in_;
    }
    std::string content;
    std::getline(in_, content, '}');
    if (! in_) {
      in_.setstate(std::ios::failbit);
      return in_;
    }
    typedef boost::tokenizer<boost::char_separator<char> > tokenizator;
    boost::char_separator<char> sep(";");
    tokenizator tok_iter(content, sep);
    for (tokenizator::iterator beg = tok_iter.begin(); beg != tok_iter.end(); ++beg) {
      boost::char_separator<char> sep2("=");
      tokenizator tok(*beg, sep2);
      std::string label;
      int32_t seed = -1;
      int count = 0;
      for (tokenizator::iterator beg2 = tok.begin(); beg2 != tok.end(); ++beg2) {
        count++;
        if (count > 2) {
          in_.setstate(std::ios::failbit);
          return in_;
        }
        if (count == 1) {
          std::istringstream iss2(*beg2);
          iss2 >> std::ws >> label;
          if (! iss2 || label.empty()) {
            in_.setstate(std::ios::failbit);
            return in_;
          }
        }
        if (count == 2) {
          std::istringstream iss2(*beg2);
          iss2 >> std::ws >> seed;
          if (! iss2 || seed < -1) {
            in_.setstate(std::ios::failbit);
            return in_;
          }
        }
      }
      if (count != 2) {
        in_.setstate(std::ios::failbit);
        return in_;
      }
      if (! seed_manager::seed_is_valid(seed) && seed != random_utils::SEED_INVALID) {
        in_.setstate(std::ios::failbit);
        return in_;
      }
      if (sm_.has_seed(label)) {
        in_.setstate(std::ios::failbit);
        return in_;
      }
      sm_._set_seed_(label, seed);
    }

    return in_;
  }

}  // end of namespace mygsl

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
