/// \file datatools/path_utils.h
/// \brief utilities for expanding and manipulating paths

#ifndef DATATOOLS_PATH_UTILS_H
#define DATATOOLS_PATH_UTILS_H

// Standard Library
#include <string>

namespace datatools {
  /** Check if datatools has its global path activated */
  bool has_global_path();

  /** Reset the datatools global path */
  void reset_global_path();

  /** Set/activate the datatools global path */
  void set_global_path(const std::string & gpath_);

  /** Get the datatools global path */
  const std::string & get_global_path();

  /* Functions to expand string as path */

  /// Extract the keys used by library information registration mechanism
  bool resolve_library_info_path_keys(const std::string & library_topic_,
                                      std::string & install_path_key_,
                                      std::string & environ_path_key_);

  /// Fetch the expanded path computed from a string.
  /// @arg path_str the string representing the path to be processed
  /// @return the processed string with resolved environment variable and registered library path
  std::string fetch_path(const std::string & path_str_);

  //! Extract the expanded path computed from the 'word' string.
  bool fetch_path(std::string & word_, std::string & errmsg_);

  //! Extract the expanded path computed from the 'word' string.
  //! Internally uses the getenv function and the datatools::library_info
  //! register.
  /// @arg word the string path to be processed
  /// @return true if the processing was successful (successfully resolved environ variables and
  ///         registered library path)
  ///
  /// Supported formats:
  ///
  /// * Plain path:
  ///
  ///  "/tmp/foo/bar/baz.txt"
  ///  "./foo/bar/baz.txt"
  ///
  /// * Path with special tokens and/or environment variables::
  ///
  ///  "~/foo/bar/baz.txt"
  ///  "~foo/bar/baz.txt"
  ///  "${HOME}/foo/bar/baz.txt"
  ///  "${ENV1}/foo/bar/baz.txt"
  ///  "${ENV1}/foo/${ENV2}/baz.txt"
  ///
  /// * Path from registered/mount point:
  ///
  ///  "@foo:bar/baz.txt"
  ///  "@foo.resources:config/simulation/1.0/manager.conf"
  ///  "@foo.data:dataset/march2017/run_1.data"
  ///
  /// * URN resolved as path:
  ///
  ///   - plain URN:
  ///
  ///     "urn:snemo:analysis:setup:1.0"
  ///
  ///   - URN with explicit category:
  ///
  ///     "urn:snemo:analysis:setup:1.0@configuration"
  ///     "urn:snemo:analysis:logo:small@image"
  ///     "urn:snemo:analysis:run:1234@data"
  ///
  bool fetch_path_with_env(std::string& word_);

  //! Extract the expanded path computed from the 'word' string.
  bool fetch_path_with_env(std::string& word_, std::string & errmsg_);

  /** Extract the expanded path computed from a string.
   * Internally uses the datatools::library_info
   * register.
   * @arg word the string path to be processed
   * @return true if the processing was successful (successfully resolved
   *         registered library path)
   */
  bool fetch_path_without_env(std::string& word_);

  /** Extract the expanded path computed from a string.
   *  Internally uses the getenv function and the datatools::library_info
   *  register.
   */
  bool fetch_path_with_env_p(std::string& word_,
                             const std::string & parent_path_);

  /** Extract the expanded path computed from a string.
   * Internally uses the getenv function and the datatools::library_info
   * register.
   */
  bool fetch_path_with_env_g(std::string& word_);

  /** Extract the expanded path computed from a string.
   * Internally uses the getenv function and the datatools::library_info
   * register.
   */
  bool fetch_path_with_env_pg(std::string& word_,
                              const std::string & parent_path_);

  //! Extract the expanded path computed from a string.
  //! Internally uses the system's shell.
  //!
  //! This method is not very elegant. It uses
  //! brute force, asking a shell to interpret the string via a 'system' call
  //! and using a temporary file to save the result. Urrkkk!
  //! That enables the expansion of environment variables embeded
  //! in the 'path_str' string:
  //!
  //! Example: '$HOME/foo.dat' is expanded to '/home/&gt;login&lt;/foo.dat'
  //! @deprecated Use the fetch_path or fetch_path_with_env functions.
  std::string expand_path(const std::string& path_str_);

} // namespace datatools

#endif // DATATOOLS_PATH_UTILS_H



