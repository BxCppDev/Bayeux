/* seed_manager.cc
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <ctime>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <map>
#include <limits>
#include <cstdlib>
//#include <random> /// only with new "-std=c++0x" support

#include <sys/types.h>
#include <unistd.h>

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

#include <mygsl/seed_manager.h>
#include <mygsl/random_utils.h>
#include <mygsl/rng.h>

#include <datatools/exception.h>
#include <datatools/logger.h>

namespace mygsl {

  using namespace std;

  const std::string seed_manager::INIT_SEED_FROM_ENV_NAME = "MYGSL_SEED_MANAGER_INIT_SEED_FROM";

  datatools::logger::priority seed_manager::get_logging_priority () const
  {
    return _logging_priority_;
  }

  void seed_manager::set_logging_priority (datatools::logger::priority priority_)
  {
    _logging_priority_ = priority_;
    return;
  }

  void seed_manager::clear ()
  {
    _dict_.clear ();
    return;
  }

  size_t seed_manager::size () const
  {
    return _dict_.size ();
  }

  bool seed_manager::empty () const
  {
    return _dict_.empty ();
  }

  void seed_manager::all_time_seeds ()
  {
    for (dict_type::iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++)
      {
        i->second = random_utils::SEED_TIME;
      }
    return;
  }

  void seed_manager::_set_init_seed_flags_ ()
  {
    if (_init_seed_flags_ == INIT_SEED_FROM_UNDEFINED)
      {
        std::string init_seed_from_env;
        char * dummy = getenv(INIT_SEED_FROM_ENV_NAME.c_str ());
        if (dummy != 0)
          {
            init_seed_from_env = dummy;
            if (init_seed_from_env == "current_time")
              {
                _init_seed_flags_ = INIT_SEED_FROM_CURRENT_TIME;
              }
            else if (init_seed_from_env == "current_pid")
              {
                _init_seed_flags_ = INIT_SEED_FROM_CURRENT_PID;
              }
            else if (init_seed_from_env == "urandom")
              {
                _init_seed_flags_ = INIT_SEED_FROM_URANDOM;
              }
            else
              {
                DT_LOG_WARNING(get_logging_priority (),
                               "Invalid value ('" << init_seed_from_env << "') for the '" << INIT_SEED_FROM_ENV_NAME << "' environment variable !");
              }
          }
      }
    // set default if not explicitely set :
    if (_init_seed_flags_ == INIT_SEED_FROM_UNDEFINED)
      {
        _init_seed_flags_ = INIT_SEED_FROM_DEFAULT;
      }
    return;
  }

  int32_t seed_manager::_set_seed_for_seeds ()
  {
    _set_init_seed_flags_ ();
    int32_t seed = 0;
    if (_init_seed_flags_ & INIT_SEED_FROM_CURRENT_TIME )
      {
        seed += (int32_t) time (0);
        DT_LOG_TRACE (get_logging_priority (), "seed=" << seed << " (from current time)");
      }
    if (_init_seed_flags_ & INIT_SEED_FROM_CURRENT_PID )
      {
        seed += (int32_t) getpid ();
        DT_LOG_TRACE (get_logging_priority (), "seed=" << seed << " (from current PID)");
      }
    if (_init_seed_flags_ & INIT_SEED_FROM_URANDOM )
      {
        std::string dev_urandom = "/dev/urandom";
        DT_THROW_IF (! boost::filesystem::exists (dev_urandom.c_str ()),
                     std::runtime_error,
                     "No '" << dev_urandom << "' entropy source on this host !");
        unsigned int useed = 0;
        FILE *urandom;
        urandom = fopen (dev_urandom.c_str (), "r");
        fread (&useed, sizeof (useed), 1, urandom);
        seed += (int32_t) useed & 0x7FFFFFFF;
        fclose (urandom);
        DT_LOG_TRACE (get_logging_priority (), "seed=" << seed << " (from '/dev/urandom')");
      }
    // Not supported yet :
    // if (_init_seed_flags_ | INIT_SEED_RANDOM_DEVICE )
    //   {
    //     std::random_device rd;
    //     seed += (int32_t) rd ();
    //   }
    seed &= 0x7FFFFFFF;
    DT_LOG_TRACE (get_logging_priority (), "final seed=" << seed);
    return seed;
  }

  void seed_manager::transform_time_seeds (bool allow_duplication_)
  {
    int32_t seed = _set_seed_for_seeds ();
    mygsl::rng r;
    r.initialize (mygsl::rng::DEFAULT_RNG_TYPE, seed);
    for (dict_type::iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++)
      {
        // only SEED_TIME records are changed :
        if (i->second == random_utils::SEED_TIME)
          {
            // Randomize the seed :
            i->second = 1 + r.uniform_int (std::numeric_limits<int32_t>::max () / 2);
          }
      }
    if (! allow_duplication_)
      {
        _ensure_different_seeds (&r);
      }
    return;
  }

  void seed_manager::ensure_different_seeds ()
  {
    int32_t seed = _set_seed_for_seeds ();
    mygsl::rng r;
    r.initialize (mygsl::rng::DEFAULT_RNG_TYPE ,seed);
    _ensure_different_seeds (&r);
    return;
  }


  bool seed_manager::has_invalid_seeds () const
  {
    for (dict_type::const_iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++)
      {
        int32_t seed = i->second;
        if (seed == random_utils::SEED_INVALID)
          {
            return true;
          }
      }
    return false;
  }


  bool seed_manager::has_time_seeds () const
  {
    for (dict_type::const_iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++)
      {
        int32_t seed = i->second;
        if (seed == random_utils::SEED_TIME)
          {
            return true;
          }
      }
    return false;
  }


  bool seed_manager::all_different_seeds () const
  {
    std::map<int32_t, int> mseeds;
    for (dict_type::const_iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++)
      {
        int32_t seed = i->second;
        if (mseeds.find (seed) == mseeds.end ())
          {
            mseeds[seed] = 0;
          }
        mseeds[seed] += 1;
        if (mseeds[seed] > 1)
          {
            return false;
          }
      }
    return true;
  }

  void seed_manager::_ensure_different_seeds (mygsl::rng * random_)
  {
    int32_t seed = (int32_t) time (0);
    mygsl::rng * r = random_;
    mygsl::rng r2;
    if (r == 0)
      {
        r = &r2;
      }
    if (! r->is_initialized ())
      {
        r->initialize (mygsl::rng::DEFAULT_RNG_TYPE ,seed);
      }

    typedef std::multimap<int32_t, std::string> seed_mmap_type;
    typedef seed_mmap_type::iterator            iter_type;

    seed_mmap_type mmseeds;
    for (dict_type::const_iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++)
      {
        const std::string & l = i->first;
        int32_t s = i->second;
        mmseeds.insert (std::make_pair (s,l));
      }
    // showing contents:
    DT_LOG_TRACE (get_logging_priority (), "Seed manager contains:");
    iter_type it, it2;
    for (iter_type it = mmseeds.begin();
         it != mmseeds.end();
         it = it2)
      {
        int32_t theKeySeed = (*it).first;
        std::pair<iter_type, iter_type> keyRange = mmseeds.equal_range(theKeySeed);

        DT_LOG_TRACE (get_logging_priority (), "  for seed==" << theKeySeed << " , PRNGs are : " );
        for (it2 = keyRange.first;  it2 != keyRange.second;  ++it2)
          {
            DT_LOG_TRACE (get_logging_priority (), '"' << (*it2).second << '"' << " ");
          }
        if ((theKeySeed != random_utils::SEED_INVALID)
            && (mmseeds.count (theKeySeed) > 1))
          {
            DT_LOG_TRACE (get_logging_priority (), "Detected identical seeds for this group of PRNGs.");
            int count = 0;
            for (it2 = keyRange.first;  it2 != keyRange.second;  ++it2)
              {
                const std::string & prng_label = (*it2).second;
                bool do_it = false;
                if (count > 0 || theKeySeed == random_utils::SEED_TIME)
                  {
                    do_it = true;
                  }
                if (do_it)
                  {
                    DT_LOG_TRACE (get_logging_priority (), "Setting a new seed for PRNG '" << prng_label << "'...");
                    while (true)
                      {
                        int32_t theNewSeed = 1 + r->uniform_int (std::numeric_limits<int32_t>::max () / 2);
                        if (mmseeds.count (theNewSeed) == 0)
                          {
                            update_seed (prng_label, theNewSeed);
                            DT_LOG_TRACE (get_logging_priority (), "PRNG '" << prng_label << "' has now seed = "
                                          << theNewSeed);
                            break;
                          }
                      }

                  }
                count++;
              }
          }
      }
    DT_LOG_NOTICE(get_logging_priority (), "All seeds have been made different.");
    return;
  }

  void seed_manager::invalidate ()
  {
    for (dict_type::iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++)
      {
        i->second = random_utils::SEED_INVALID;
      }
    return;
  }

  // static
  bool seed_manager::seed_is_valid (int32_t seed_)
  {
    if (seed_ < random_utils::SEED_INVALID) return false;
    return true;
  }

  void seed_manager::get_labels (vector<string> & labels_) const
  {
    labels_.clear ();
    labels_.reserve (_dict_.size ());
    for (dict_type::const_iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++)
      {
        labels_.push_back (i->first);
      }
    return;
  }

  bool seed_manager::has_seed (const string & label_) const
  {
    return _dict_.find (label_) != _dict_.end ();
  }

  int32_t seed_manager::get_seed (const string & label_) const
  {
    dict_type::const_iterator found = _dict_.find (label_);
    DT_THROW_IF (found == _dict_.end (), std::logic_error, "No seed with label '" << label_ << "' is defined !");
    return found->second;
  }

  void seed_manager::update_seed (const string & label_, int32_t seed_)
  {
    dict_type::iterator found = _dict_.find (label_);
    if (found == _dict_.end ())
      {
        add_seed (label_, seed_);
      }
    else
      {
        DT_THROW_IF (! seed_is_valid (seed_), std::logic_error, "Seed value '" << seed_ << "' is not allowed !");
        found->second = seed_;
      }
    return;
  }

  void seed_manager::add_seed (const string & label_, int32_t seed_)
  {
    DT_THROW_IF (label_.empty (), std::logic_error, "Label '" << label_ << "' is empty !");
    DT_THROW_IF (_dict_.find (label_) != _dict_.end (),
                 std::logic_error, "Label '" << label_ << "' is already used !");
    DT_THROW_IF (! seed_is_valid (seed_), std::logic_error,
                 "Seed value '" << seed_ << "' is not allowed !");
    _dict_[label_] = seed_;
    return;
  }

  void seed_manager::set_init_seed_flags (uint32_t f_)
  {
    _init_seed_flags_ = f_;
    return;
  }

  uint32_t seed_manager::get_init_seed_flags () const
  {
    return _init_seed_flags_;
  }

  // ctor:
  seed_manager::seed_manager ()
  {
    _logging_priority_ = datatools::logger::PRIO_WARNING;
    _init_seed_flags_ = INIT_SEED_FROM_UNDEFINED;
    //INIT_SEED_DEFAULT;
    return;
  }

  // dtor:
  seed_manager::~seed_manager ()
  {
    return;
  }

  void seed_manager::dump (ostream & out_) const
  {
    out_ << "mygsl::seed_manager:" << endl;
    for (dict_type::const_iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++)
      {
        out_ << "|-- ";
        out_.width (25) ;
        out_ << i->first << " : ";
        out_.width (10);
        out_ << i->second << " ";
        if (i->second == mygsl::random_utils::SEED_TIME)
          {
            out_ << "[set randomly]";
          }
        else if (i->second == mygsl::random_utils::SEED_INVALID)
          {
            out_ << "[undefined]";
          }
        else
          {
            out_ << "[set by value]";
          }
        out_ << endl;
      }
    out_ << "`-- " << "End" << endl;

    return;
  }

  // friend :
  ostream & operator<< (ostream & out_, const seed_manager & sm_)
  {
    out_ << '{';
    for (seed_manager::dict_type::const_iterator i = sm_._dict_.begin ();
         i != sm_._dict_.end ();
         i++)
      {
        if (i != sm_._dict_.begin ()) out_ << "; ";
        out_ << i->first << "=" << i->second;
      }
    out_ << '}';
    return out_;
  }

  // friend :
  istream & operator>> (istream & in_, seed_manager & sm_)
  {
    sm_._dict_.clear ();
    in_ >> ws;
    char c = 0;
    in_.get (c);
    if (c != '{')
      {
        in_.setstate (ios::failbit);
        return in_;
      }
    string content;
    getline (in_, content, '}');
    if (! in_)
      {
        in_.setstate (ios::failbit);
        return in_;
      }
    typedef boost::tokenizer<boost::char_separator<char> > tokenizator;
    boost::char_separator<char> sep(";");
    tokenizator tok (content, sep);
    for (tokenizator::iterator beg = tok.begin ();
         beg != tok.end ();
         ++beg)
      {
        boost::char_separator<char> sep2 ("=");
        tokenizator tok (*beg, sep2);
        string label;
        int32_t seed = -1;
        int count = 0;
        for (tokenizator::iterator beg2 = tok.begin ();
             beg2 != tok.end ();
             ++beg2)
          {
            count++;
            if (count > 2)
              {
                in_.setstate (ios::failbit);
                return in_;
              }
            if (count == 1)
              {
                istringstream iss2 (*beg2);
                iss2 >> ws >> label;
                if (! iss2 || label.empty ())
                  {
                    in_.setstate (ios::failbit);
                    return in_;
                  }
              }
            if (count == 2)
              {
                istringstream iss2 (*beg2);
                iss2 >> ws >> seed;
                if (! iss2 || seed < -1)
                  {
                    in_.setstate (ios::failbit);
                    return in_;
                  }
              }
          }
        if (count != 2)
          {
            in_.setstate (ios::failbit);
            return in_;
          }
        if (! seed_manager::seed_is_valid (seed))
          {
            in_.setstate (ios::failbit);
            return in_;
          }
        if (sm_.has_seed (label))
          {
            in_.setstate (ios::failbit);
            return in_;
          }
        sm_.add_seed (label, seed);
      }

    return in_;
  }

}  // end of namespace mygsl

// end of seed_manager.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
