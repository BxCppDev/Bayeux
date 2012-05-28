// -*- mode: c++; -*-
// version_id.cc
 
#include <datatools/utils/version_id.h>

#include <sstream>
#include <stdexcept>
#include <boost/tokenizer.hpp>
#include <cctype>

namespace datatools {

  namespace utils {

    const std::string version_id::ALPHA_TAG = "alpha";
    const std::string version_id::BETA_TAG  = "beta";
    const std::string version_id::RELEASE_CANDIDATE_PREFIX_TAG = "rc";
    
    
    version_id::version_id()
    {
      _major_ = INVALID_NUMBER;
      _minor_ = INVALID_NUMBER;
      _revision_ = INVALID_NUMBER;
      _tag_number_ = INVALID_NUMBER;
    }

    version_id::version_id(const std::string& tag_)
    {
      _major_ = INVALID_NUMBER;
      _minor_ = INVALID_NUMBER;
      _revision_ = INVALID_NUMBER;
      _tag_number_ = INVALID_NUMBER;
      this->set_tag(tag_);
    }

    version_id::version_id(int major_, 
                           int minor_, 
                           int revision_, 
                           const std::string& tag_)
    {
      _major_ = INVALID_NUMBER;
      _minor_ = INVALID_NUMBER;
      _revision_ = INVALID_NUMBER;
      _tag_number_ = INVALID_NUMBER;
      this->set_major(major_);
      this->set_minor(minor_);
      this->set_revision(revision_);
      this->set_tag(tag_);
    }


    bool version_id::is_valid() const
    {
      if (!this->has_major())
        {
          // no major implies a single valid tag : format = "tag" 
          return this->has_tag(); 
        }
      else 
        {
          if (this->has_revision() && ! this->has_minor())
            {
              return false;
            }
          // format = "major.minor.revision-tag" 
          if (this->has_tag())
            {
              return has_revision();
            }
        }    
      return true;
    }

    bool version_id::has_numbers_only() const
    {
      return this->has_major() && !this->has_tag();
    }

    bool version_id::has_major() const
    {
      return _major_ != INVALID_NUMBER;
    }

    int version_id::get_major() const
    {
      return _major_;
    }

    void version_id::set_major(int major_)
    {
      _major_ = major_ < 0 ? INVALID_NUMBER : major_;
    }

    bool version_id::has_minor() const
    {
      return _minor_ != INVALID_NUMBER;
    }

    int version_id::get_minor() const
    {
      return _minor_;
    }

    void version_id::set_minor(int minor_)
    {
      _minor_ = minor_ < 0 ? INVALID_NUMBER : minor_;
    }

    bool version_id::has_revision() const
    {
      return _revision_ != INVALID_NUMBER;
    }

    int version_id::get_revision() const
    {
      return _revision_;
    }

    void version_id::set_revision(int revision_)
    {
      _revision_ = revision_ < 0 ? INVALID_NUMBER : revision_;
    }

    bool version_id::has_tag_only() const
    {
      return this->has_tag() && !this->has_major();
    }

    bool version_id::has_tag() const
    {
     return ! _tag_.empty();
    }

    bool version_id::has_special_tag() const
    {
      if (_tag_.empty()) 
        {
          return false;
        }
      return _tag_number_ < INVALID_NUMBER;
    }

    bool version_id::has_numeric_tag() const
    {
      if (_tag_.empty()) 
        {
          return false;
        }
      // Specific test :
      {
        std::istringstream dummy(_tag_);
        int val;
        dummy >> val;
        if (!dummy)
          {
            // not starting with a digit
            return false;
          }
        std::string word;
        dummy >> word;
        if (!word.empty())
          {
            // existing trailing chars
            return false;
          }
      }
      return true;
    }

    const std::string & version_id::get_tag() const
    {
      return _tag_;
    }
 
    bool version_id::has_tag_number() const
    {
      return _tag_number_ != INVALID_NUMBER;
    }
 
    int version_id::get_tag_number() const
    {
      return _tag_number_;
    }

    void version_id::set_tag(const std::string& tag_)
    {
      std::istringstream tag_iss(tag_);
      std::string trimed_tag;
      tag_iss >> std::ws >> trimed_tag >> std::ws;
      if (trimed_tag.empty())
        {
          _tag_.clear();
          return;
        }
      char first_char = trimed_tag[0];
      bool validate = false;
      if (isalnum(first_char)) 
        {
          validate = true;
        }
      if (!validate)
        {
          std::ostringstream message;
          message << "datatools::utils::version_id::set_tag: "
                  << "Version ID's tag '" << trimed_tag << "' is not allowed !";
          throw std::logic_error(message.str());       
        }
      validate = true;
      for (int i = 0; i < trimed_tag.length(); i++)
        {
          char c = trimed_tag[i];
          if (ispunct(c) && c != '-' && c !=  '_')
            {
              validate = false;
            }
        }
      if (!validate)
        {
          std::ostringstream message;
          message << "datatools::utils::version_id::set_tag: "
                  << "Version ID's tag '" << trimed_tag << "' is not allowed !";
          throw std::logic_error(message.str());       
        } 
      _tag_ = trimed_tag;
      if (isdigit(_tag_[0]))
        {
          // check numerical tags :
          std::istringstream num_tag_iss(_tag_);
          int num_tag;
          num_tag_iss >> num_tag;
          if (num_tag_iss && num_tag > 0)
            {
              _tag_number_ = num_tag;
            }
        }
      else 
        {
          // check special orderable tags :
          if (_tag_ == ALPHA_TAG) 
            {
              _tag_number_ = ALPHA_NUM;
            }
          else if (_tag_ == BETA_TAG) 
            {
              _tag_number_ = BETA_NUM;
            }
          else if (_tag_.length() > 2 && _tag_.substr(0,2) == RELEASE_CANDIDATE_PREFIX_TAG)
            {
              std::string rc_tag_tail = _tag_.substr(2);
              if (! rc_tag_tail.empty()) 
                {
                  std::istringstream rc_tag_iss(rc_tag_tail);
                  int rc_num;
                  rc_tag_iss >> rc_num;
                  if (rc_tag_iss)
                    { 
                      if (rc_num >= 0)
                        {
                          if (rc_num > MAX_RC_NUM)
                            {
                              std::ostringstream message;
                              message << "datatools::utils::version_id::set_tag: "
                                      << "Version ID's special RC tag '" << trimed_tag << "' is not allowed (RC number is too large) !";
                              throw std::logic_error(message.str());       
                            }
                          _tag_number_ = RC_BASE_NUM + rc_num;
                        }
                    } 
                 }
            } // RC
        }
      return;
    }

    void version_id::dump(std::ostream& out_ , 
                          const std::string& title_,
                          const std::string& indent_) const
    {
      const std::string tag = "|-- ";
      const std::string last_tag = "`-- ";
      if (!title_.empty())
        {
          out_ << indent_ << title_ << std::endl;
        }
      out_ << indent_ << tag << "Has major    : " << this->has_major();
      if (this->has_major())
        {
          out_ << " => Major = " << _major_;
        }
      out_ << std::endl;
      out_ << indent_ << tag << "Has minor    : " << this->has_minor();
      if (this->has_minor())
        {
          out_ << " => Minor = " << _minor_;
        }
      out_ << std::endl;
      out_ << indent_ << tag << "Has revision : " << this->has_revision();
      if (this->has_revision())
        {
          out_ << " => Revision = " << _revision_;
        }
      out_ << std::endl;
      out_ << indent_ << tag << "Has tag      : " << this->has_tag();
      if (this->has_tag())
        {
          out_ << " => Tag = '" << _tag_ << "'";
        }
      if (this->has_tag_number())
        {
          out_ << " with number='" << _tag_number_ << "'";
          if (this->has_special_tag())
            {
              out_ << " (special)";
            }
        }
      out_ << std::endl;
      out_ << indent_ << tag      << "Valid        : " << this->is_valid() << std::endl;
      //out_ << indent_ << tag      << "Orderable    : " << is_orderable () << std::endl;
      out_ << indent_ << last_tag << "Format       : '" << *this << "'" << std::endl;
    }

    void version_id::reset()
    {
      _major_ = INVALID_NUMBER;
      _minor_ = INVALID_NUMBER;
      _revision_ = INVALID_NUMBER;
      _tag_number_ = INVALID_NUMBER;
      _tag_.clear ();
      return;
    }

    bool version_id::equals(const version_id& vid_, bool ignore_tag_) const
    {
      bool res = false;
      if (vid_._major_ == _major_
          && vid_._minor_ == _minor_ 
          && vid_._revision_ == _revision_)
        {
          res = true;
        }
      if (res && ! ignore_tag_)
        {
          res = (vid_._tag_ == _tag_);
        }
      return res;
    }

    bool version_id::operator==(const version_id& vid_) const
    {
      return this->equals(vid_, false);
    }

    int version_id::compare(const version_id& vid_) const
    {
      return compare(*this, vid_);
    }

    // static 
    bool version_id::are_orderable(const version_id& vid0_, const version_id& vid1_)
    {
      const version_id& v1 = vid0_;
      const version_id& v2 = vid1_;
      if (!v1.is_valid()) return false;
      if (!v2.is_valid()) return false;
      if (v1.has_tag_only()) return false;
      if (v2.has_tag_only()) return false;
      if (v1.equals(v2, true))
        {
          if (v1.has_tag() && ! v1.has_tag_number()) return false;
          if (v2.has_tag() && ! v2.has_tag_number()) return false;
        }
      return true;
    }

    // static 
    int version_id::compare(const version_id& vid0_, const version_id& vid1_)
    {
      bool devel = false;
      if (vid0_ == vid1_) return 0;
      const version_id& v1 = vid0_;
      const version_id& v2 = vid1_;
      if (!v1.is_valid())
        {
          std::ostringstream message;
          message << "datatools::utils::version_id::compare: "
                  << "Version ID '" << v1 << "' is not valid !";
          throw std::logic_error(message.str());
        }
      if (!v2.is_valid())
        {
          std::ostringstream message;
          message << "datatools::utils::version_id::compare: "
                  << "Version ID '" << v2 << "' is not valid !";
          throw std::logic_error(message.str());
        }
      int maj_num_1 = v1._major_;
      int maj_num_2 = v2._major_;
      int min_num_1 = v1._minor_;
      int min_num_2 = v2._minor_;
      int rev_num_1 = v1._revision_;
      int rev_num_2 = v2._revision_;
      int tag_num_1 = v1._tag_number_;
      int tag_num_2 = v2._tag_number_;

      if (maj_num_1 == INVALID_NUMBER) 
        {
          std::ostringstream message;
          message << "datatools::utils::version_id::compare: "
                  << "Version ID '" << v1 << "' is not orderable !";
          throw std::logic_error(message.str());
        }
      if (maj_num_2 == INVALID_NUMBER)
        {
          std::ostringstream message;
          message << "datatools::utils::version_id::compare: "
                  << "Version ID '" << v2 << "' is not orderable !";
          throw std::logic_error(message.str());
        }
      if (maj_num_1 < maj_num_2)
        {
          return -1;
        }
      else if (maj_num_1 > maj_num_2)
        {
          return +1;
        }
      else 
        {
          // maj_num_1 == maj_num_2
          if ((min_num_1 == INVALID_NUMBER) && (min_num_2 != INVALID_NUMBER))
            {
              return -1; 
            }
          else if ((min_num_1 != INVALID_NUMBER) && (min_num_2 == INVALID_NUMBER))
            {
              return +1; 
            }
          else if ((min_num_1 != INVALID_NUMBER) && (min_num_2 != INVALID_NUMBER))
            {
              if (min_num_1 < min_num_2)
                {
                  return -1;
                }
              else if (min_num_1 > min_num_2)
                {
                  return +1;
                }
              else 
                {
                  // min_num_1 == min_num_2
                  if ((rev_num_1 == INVALID_NUMBER) && (rev_num_2 != INVALID_NUMBER))
                    {
                      return -1; 
                    }
                  else if ((rev_num_1 != INVALID_NUMBER) && (rev_num_2 == INVALID_NUMBER))
                    {
                      return +1; 
                    }
                  else if ((rev_num_1 != INVALID_NUMBER) && (rev_num_2 != INVALID_NUMBER))
                    {
                      if (rev_num_1 < rev_num_2)
                        {
                          return -1;
                        }
                      else if (rev_num_1 > rev_num_2)
                        {
                          return +1;
                        }
                      else 
                        {
                          // rev_num_1 == rev_num_2
                          if (v1.has_tag() && ! v1.has_tag_number())
                            {
                              std::ostringstream message;
                              message << "datatools::utils::version_id::compare: "
                                      << "Version ID '" << v1 << "' tag is not orderable !";
                              throw std::logic_error(message.str());
                            }
                          if (v2.has_tag() && ! v2.has_tag_number())
                            {
                              std::ostringstream message;
                              message << "datatools::utils::version_id::compare: "
                                      << "Version ID '" << v2 << "' tag is not orderable !";
                              throw std::logic_error(message.str());
                            }
                          
                          if ((tag_num_1 == INVALID_NUMBER) && (tag_num_2 != INVALID_NUMBER))
                            {
                              if (devel) std::cerr << "DEVEL: version_id::compare: TEST 1" << std::endl;
                              if (tag_num_2 < INVALID_NUMBER)
                                {
                                  if (devel) std::cerr << "DEVEL: version_id::compare: +1" << std::endl;
                                  return +1; 
                                }
                              else
                                {
                                  if (devel) std::cerr << "DEVEL: version_id::compare: -1" << std::endl;
                                  return -1;
                                }
                            }
                          else if ((tag_num_1 != INVALID_NUMBER) && (tag_num_2 == INVALID_NUMBER))
                            {
                              if (devel) std::cerr << "DEVEL: version_id::compare: TEST 2" << std::endl;
                              if (tag_num_1 < INVALID_NUMBER)
                                {
                                  return -1; 
                                }
                              else
                                {
                                  return +1;
                                }
                            }
                          else if ((tag_num_1 != INVALID_NUMBER) && (tag_num_2 != INVALID_NUMBER))
                            {
                              if (devel) std::cerr << "DEVEL: version_id::compare: TEST 3" << std::endl;
                              if (devel) std::cerr << "DEVEL: version_id::compare:   tag_num_1 = " << tag_num_1 << std::endl;
                              if (devel) std::cerr << "DEVEL: version_id::compare:   tag_num_2 = " << tag_num_2 << std::endl;
                              if (tag_num_1 < tag_num_2)
                                {
                                  return -1;
                                }
                              else if (tag_num_1 > tag_num_2)
                                {
                                  return +1;
                                }             
                            }
                        }
                    }
                }
            }
        }
      return 0;
    }

    void version_id::to_string(std::string& format_) const
    {
      std::ostringstream out;
      out << *this;
      format_ = out.str ();
      return;
    }

    std::ostream & operator<< (std::ostream& out_,
                               const version_id& vid_)
    {
      if (! vid_.is_valid())
        {
          out_ << vid_.INVALID_SYMBOL;
        }
      else
        {
          if (vid_.has_major())
            {
              out_ << vid_.get_major();
              if (vid_.has_minor())
                {
                  out_ << version_id::NUMERIC_SEPARATOR << vid_.get_minor();
                  if (vid_.has_revision())
                    {
                      out_ << version_id::NUMERIC_SEPARATOR << vid_.get_revision();
                    }
                }
              if (vid_.has_tag())
                {
                  out_ << version_id::TAG_SEPARATOR;
                  out_ << vid_.get_tag();
                }
            }
          else
            {
              if (vid_.has_tag())
                {
                  out_ << vid_.get_tag();
                }
            }
        }
      return out_;
    }
    
    bool version_id::from_string(const std::string& s_)
    {
      bool devel = false;
      //devel = true;
      this->reset();
      std::istringstream iss(s_);
      std::string token;
      iss >> std::ws >> token >> std::ws;
      if (token.empty())
        {
          // empty input :
          return false;
        }
      if (token.size() == 1 && token[0] == INVALID_SYMBOL)
        {
          // the invalid value is allowed.
          return true;
        }
      if (devel) std::cerr << std::endl << "DEVEL: version_id::from_string: string = '" << s_ << "'" << std::endl;

      // Check there is only a tag without leading "major[.minor[.revision[-tag]]]" :
      {
        std::istringstream iss(token);
        int val = INVALID_NUMBER;
        iss >> val;
        if (!iss)
          {
            // cannot parse an integer :
            try 
              {
                // pure "tag" format :
                this->set_tag(token);                
              }
            catch (std::exception& x)
              {
                this->reset();
                return false;           
              }     
            if (!this->is_valid())
              {
                this->reset();
                return false;
              }
            return true;
          }
        else
          {
            if (val < 0)
              {
                this->reset();
                return false;
              }
          }
      }

      // Now we are in a "major[.minor[.revision[-tag]]]" format :
      {
        std::istringstream iss(token);
        bool search_tag = false;
        for (int i = 0; i < 3; i++)
          {
            int val = INVALID_NUMBER;
            iss >> val;
            if (!iss)
              {
                this->reset();
                return false;                   
              }
            else if (val < 0)
              {
                this->reset();
                return false;
              }
            try 
              {
                if (i == 0) this->set_major(val);
                if (i == 1) this->set_minor(val);
                if (i == 2) this->set_revision(val);
              }
            catch (std::exception& x)
              {
                this->reset();
                return false;
              }
            // get the separator :
            char sep = 0;
            iss.get(sep);
            if (devel) std::cerr << "DEVEL: version_id::from_string:   separator = '" << sep << "'" << std::endl;
            if (!iss)
              {
                return true;
              }
            if (sep == TAG_SEPARATOR)
              {
                if (i < 2)
                  {
                    this->reset();
                    return false;
                  }
                search_tag = true;
                break;
              }
            if ((i < 2) && (sep == NUMERIC_SEPARATOR))
              {
                continue;
              }
            this->reset();
            return false;
          } 
        if (search_tag)
          {
            // extract a tag :
            std::string tag;
            iss >> tag;
            if (devel) std::cerr << "DEVEL: version_id::from_string:   tag = '" << tag << "'" << std::endl;
            if (tag.empty())
              {
                this->reset();
                return false;           
              }
            try 
              {
                this->set_tag(tag);
              }
            catch (std::exception& x)
              {
                if (devel) std::cerr << "DEVEL: version_id::from_string:   set_tag error = '" << x.what () << "'" << std::endl;
                this->reset();
                return false;           
              }
            if (!this->is_valid())
              {
                this->reset();
                if (devel) std::cerr << "DEVEL: version_id::from_string:   not valid!" << std::endl;
                return false;           
              }
          }
      }

      return true;
    }

    std::istream & operator>>(std::istream& in_, 
                              version_id& vid_)
    {
      std::string word;
      in_ >> word;
      if (!vid_.from_string(word))
        {
          vid_.reset();
          in_.setstate(std::ios::failbit);
        }
      return in_;
    }

    bool version_id::matches(const std::string& version_rule_) const
    {
      std::istringstream vr_iss(version_rule_);
      std::string word;
      vr_iss >> word;
      if (word.empty())
        {
          std::ostringstream message;
          message << "datatools::utils::version_id::matches: "
                  << "Missing valid version rule !";
          throw std::logic_error(message.str());
        }
      const int INVALID          = -1000; 
      const int NOT_EQUAL        = -3; 
      const int LESS_OR_EQUAL    = -2; 
      const int LESS             = -1; 
      const int EQUAL            =  0; 
      const int GREATER          =  1; 
      const int GREATER_OR_EQUAL =  2; 
      int match_mode = INVALID;
      std::string vid_word;
      if (word.length() >= 2)
        {
          std::string op = word.substr(0,2);
          if (op == "==")
            {
              match_mode = EQUAL;
            }
          if (op == "!=")
            {
              match_mode = NOT_EQUAL;
            }
          if (op == "<=")
            {
              match_mode = LESS_OR_EQUAL;
            }
          if (op == ">=")
            {
              match_mode = GREATER_OR_EQUAL;
            }
          vid_word = word.substr (2); 
        }
      else if (word.length() >= 1)
        {
          std::string op = word.substr(0,1);
          if (op == "=")
            {
              match_mode = EQUAL;
            }
          if (op == "!")
            {
              match_mode = NOT_EQUAL;
            }
          if (op == "<")
            {
              match_mode = LESS;
            }
          if (op == ">")
            {
              match_mode = GREATER;
            }
          vid_word = word.substr(1); 
        }
      if (vid_word.empty())
        {
          vr_iss >> vid_word;
        }
      if (vid_word.empty())
        {
          std::ostringstream message;
          message << "datatools::utils::version_id::matches: "
                  << "Missing version ID !";
          throw std::logic_error(message.str());
        }
      version_id matcher_vid;
      std::istringstream vid_iss(vid_word);
      vid_iss >> matcher_vid;
      if (!vid_iss)
        {
          std::ostringstream message;
          message << "datatools::utils::version_id::matches: "
                  << "Invalid version ID format !";
          throw std::logic_error(message.str());
        }
      if (match_mode == EQUAL)
        {
          if (*this == matcher_vid) return true;
          else return false;
        }
      else if (match_mode == NOT_EQUAL)
        {
          if (*this == matcher_vid) return false;
          else return true;
        }
      else 
        {
          if (!are_orderable(*this, matcher_vid))
            {
              std::ostringstream message;
              message << "datatools::utils::version_id::matches: "
                      << "Version IDs " << *this << " and " << matcher_vid << " are not orderable !";
              throw std::logic_error(message.str());
            }
          int res = compare(*this, matcher_vid);
          if (match_mode == LESS)
            {
              if (res == -1) return true;
              else return false;
            }
          else if (match_mode == GREATER)
            {
              if (res == +1) return true;
              else return false;
            }
          else if (match_mode == GREATER_OR_EQUAL)
            {
              if (res != -1 ) return true;
              else return false;
            }
          else if (match_mode == LESS_OR_EQUAL)
            {
              if (res != +1 ) return true;
              else return false;
            }
          else
            {
              std::ostringstream message;
              message << "datatools::utils::version_id::matches: "
                      << "Invalid rule !";
              throw std::logic_error(message.str());
            }
        }
      return false;
    }

  } // namespace utils

} // namespace datatools

// end of version_id.cc

