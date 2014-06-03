/* test_utils.cxx */
// Ourselves
#include <datatools/utils.h>

// Standard Library
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>
#include <wordexp.h>
#include <bitset>

// Third Party
// - A

// This Project
#include <datatools/datatools_config.h>
#include <datatools/ioutils.h>
#include <datatools/kernel.h>
#include <datatools/library_info.h>
#include <datatools/properties.h>
#include <datatools/version.h>

void test_quotes_manipulation()
{
  std::clog << std::endl;
  std::clog << "Quotes manipulation: " << std::endl;
  std::string text;
  std::string text2;

  text = "\"a text\"";

  if (datatools::is_quoted(text, '"')) {
    std::clog << "Text <" << text << "> is quoted by <\">." << std::endl;
  }

  datatools::remove_quotes(text, text2, '"');
  std::clog << "Text2 = <" << text2 << ">." << std::endl;

  datatools::add_quotes(text2, text, '\'');
  std::clog << "Text  = <" << text << ">." << std::endl;

  datatools::add_quotes(text, text2, '"');
  std::clog << "Text2 = <" << text2 << ">." << std::endl;

  datatools::add_quotes(text2, text, '\'');
  std::clog << "Text  = <" << text << ">." << std::endl;

  if (datatools::is_quoted(text, '\'')) {
    std::clog << "Text <" << text << "> is quoted by <'>." << std::endl;
  }

  datatools::remove_quotes(text, text2, '\'');
  std::clog << "Text2 = <" << text2 << ">." << std::endl;

  datatools::remove_quotes(text2, text, '\'');
  std::clog << "Text = <" << text << ">." << std::endl;

  datatools::remove_quotes(text, '"');
  std::clog << "Text = <" << text << ">." << std::endl;

  datatools::remove_quotes(text, '\'');
  std::clog << "Text = <" << text << ">." << std::endl;

  datatools::add_quotes(text, '"');
  std::clog << "Text = <" << text << ">." << std::endl;


  return;
}

void test_bit_manipulation()
{
  std::clog << std::endl;
  std::clog << "Bit manipulation: " << std::endl;
  uint16_t s = 1;
  std::clog << "s = " << datatools::io::to_binary (s) << std::endl;
  datatools::set_bit (s, 3);
  datatools::set_bit (s, 7);
  datatools::set_bit (s, 8);
  std::clog << "s = " << datatools::io::to_binary (s) << std::endl;
  datatools::unset_bit (s, 0);
  std::clog << "s = " << datatools::io::to_binary (s) << std::endl;
  datatools::toggle_bit (s, 7);
  std::clog << "s = " << datatools::io::to_binary (s) << std::endl;
  datatools::toggle_bit (s, 15);
  std::clog << "s = " << datatools::io::to_binary (s) << std::endl;
  datatools::toggle_bit (s, 15);
  std::clog << "s = " << datatools::io::to_binary (s) << std::endl;
  std::clog << "s = " << datatools::io::to_binary (s, true) << std::endl;
  return;
}

void test_path_manipulation()
{
  {
    std::clog << "Testing wordexp : " << std::endl;
    wordexp_t p;
    char **w;
    int i;
    wordexp("${HOME}/testing/test_[a-c]*.cxx", &p, 0);
    w = p.we_wordv;
    for (i = 0; i < (int) p.we_wordc; i++)
      std::clog << w[i] << '\n';
    wordfree(&p);
  }

  {
    std::clog << std::endl;
    std::string path = "~francois/*.txt";
    std::clog << "Original path : '" << path << "'" << std::endl;
    try {
      datatools::fetch_path_with_env (path);
      std::clog << "Fetched path  : '" << path << "'" << std::endl;
    }
    catch(std::exception & xxx) {
      std::cerr << "WARNING: As expected, " << xxx.what() << std::endl;
    }
  }

  {
    std::clog << std::endl;
    std::string path = "~francois/file_?.data";
    std::clog << "Original path : '" << path << "'" << std::endl;
    try {
      datatools::fetch_path_with_env (path);
      std::clog << "Fetched path  : '" << path << "'" << std::endl;
    }
    catch(std::exception & xxx) {
      std::cerr << "WARNING: As expected, " << xxx.what() << std::endl;
    }
  }

  {
    std::clog << std::endl;
    std::string path = "${USER/README.txt";
    std::clog << "Original path : '" << path << "'" << std::endl;
    try {
      datatools::fetch_path_with_env (path);
      std::clog << "Fetched path  : '" << path << "'" << std::endl;
    }
    catch(std::exception & xxx) {
      std::cerr << "WARNING: As expected, " << xxx.what() << std::endl;
    }
  }

  {
    std::clog << std::endl;
    std::string path = "$HOME";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded = datatools::expand_path (path);
    std::clog << "Expanded path : '" << expanded << "'" << std::endl;
    datatools::fetch_path_with_env (path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    std::string path = "~francois/setup.${USER}";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded = datatools::expand_path (path);
    std::clog << "Expanded path : '" << expanded << "'" << std::endl;
    datatools::fetch_path_with_env (path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    std::string path = "${HOME}/.ssh/config.${USER}";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded = datatools::expand_path (path);
    std::clog << "Expanded path : '" << expanded << "'" << std::endl;
    datatools::fetch_path_with_env (path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    std::string path = "file://${HOME}/.ssh/config.${USER}";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded = datatools::expand_path (path);
    std::clog << "Expanded path : '" << expanded << "'" << std::endl;
    datatools::fetch_path_with_env (path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    std::string path = "file://~/.ssh/config.${USER}";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded = datatools::expand_path (path);
    std::clog << "Expanded path : '" << expanded << "'" << std::endl;
    datatools::fetch_path_with_env (path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    std::string path = "https://nemo.lpc-caen.in2p3.fr/wiki/datatools/branches/dev_${USER}/README.txt";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded = datatools::expand_path (path);
    std::clog << "Expanded path : '" << expanded << "'" << std::endl;
    datatools::fetch_path_with_env (path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    std::string parent_path = "/usr/share/doc";
    std::clog << "Parent path   : '" << parent_path << "'" << std::endl;
    std::string path = "datatools-5.0.0/README.txt";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded_path = path;
    datatools::fetch_path_with_env_p(path, parent_path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    std::string parent_path = "/usr/share/doc";
    std::clog << "Parent path   : '" << parent_path << "'" << std::endl;
    std::string path = "./datatools-5.0.0/README.txt";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded_path = path;
    datatools::fetch_path_with_env_p(path, parent_path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    std::string parent_path = "/usr/share/doc";
    std::clog << "Parent path   : '" << parent_path << "'" << std::endl;
    std::string path = "/datatools-5.0.1/README.txt";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded_path = path;
    datatools::fetch_path_with_env_p(path, parent_path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    std::string parent_path = "/usr/share/doc";
    std::clog << "Parent path   : '" << parent_path << "'" << std::endl;
    std::string path = "~";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded_path = path;
    datatools::fetch_path_with_env_p(path, parent_path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    std::string parent_path = "/usr/share/doc";
    std::clog << "Parent path   : '" << parent_path << "'" << std::endl;
    std::string path = "file://test.lis";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded_path = path;
    datatools::fetch_path_with_env_p(path, parent_path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    std::string parent_path = "/usr/share/doc";
    std::clog << "Parent path   : '" << parent_path << "'" << std::endl;
    std::string path = "${HOME}/test.lis";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded_path = path;
    datatools::fetch_path_with_env_p(path, parent_path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    datatools::set_global_path("/tmp/${USER}/config.d");
    std::clog << "Global path   : '" << datatools::get_global_path() << "'" << std::endl;
    std::string path = "test.lis";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded_path = path;
    datatools::fetch_path_with_env_g(path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
    datatools::reset_global_path();
  }

  {
    std::clog << std::endl;
    datatools::set_global_path("/tmp/${USER}/config.d");
    std::clog << "Global path   : '" << datatools::get_global_path() << "'" << std::endl;
    std::string path = "test.lis";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded_path = path;
    datatools::fetch_path_with_env_g(path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
    datatools::reset_global_path();
  }

  // 2013-04-28 FM : Not sure setenv is portable :
#ifndef DATATOOLS_NO_SETENV
  {
    std::clog << std::endl;
    ::setenv("DATATOOLS_GLOBAL_PATH", "/var/tmp/config.d", 1);
    std::clog << "Global path   : '" << datatools::get_global_path() << "'" << std::endl;
    std::string path = "test.lis";
    std::clog << "Original path : '" << path << "'" << std::endl;
    std::string expanded_path = path;
    datatools::fetch_path_with_env_g(path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
    datatools::reset_global_path();
  }
#endif

  {
    std::clog << std::endl;
    std::string path = "@foo:data/values.data";
    std::clog << "Original path : '" << path << "'" << std::endl;
    if (datatools::fetch_path_with_env(path)) {
      std::clog << "Fetched path  : '" << path << "'" << std::endl;
    } else {
      std::cerr << "Cannot fetch path  : '" << path << "'" << std::endl;
    }
  }

  {
    std::clog << std::endl;
    std::string path = "@foo:/data/values.data";
    std::clog << "Original path : '" << path << "'" << std::endl;
    if (datatools::fetch_path_with_env(path)) {
      std::clog << "Fetched path  : '" << path << "'" << std::endl;
    } else {
      std::cerr << "Cannot fetch path  : '" << path << "'" << std::endl;
    }
  }

  {
    std::clog << std::endl;
    std::string path = "@foo:/data/${USER}/values.data";
    std::clog << "Original path : '" << path << "'" << std::endl;
    if (datatools::fetch_path_with_env(path)) {
      std::clog << "Fetched path  : '" << path << "'" << std::endl;
    } else {
      std::cerr << "Cannot fetch path  : '" << path << "'" << std::endl;
    }
  }

  std::clog << "\n***** From here: kernel registration of the 'foo' library *****" << std::endl;
  if (! datatools::kernel::is_instantiated()) {
    datatools::kernel::instantiate();
  }
  datatools::kernel & krnl = datatools::kernel::instance();
  if (! krnl.is_initialized()) {
    krnl.initialize(0, 0);
  }
  if (krnl.has_library_info_register()) {
    datatools::library_info & lib_info_reg
      = krnl.grab_library_info_register();
    datatools::properties & datatools_lib_infos
      = lib_info_reg.registration("foo",
                                  "Foo registration test",
                                  datatools::version::get_version()
                                  );

    // Register some paths related to foo in the datatools' kernel:
    datatools_lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
                                     "/tmp/${USER}/foo/resources"
                                     );
    datatools_lib_infos.store_string(datatools::library_info::keys::env_resource_dir(),
                                     "FOO_RESOURCE_DIR"
                                     );
    datatools_lib_infos.store_string(datatools::library_info::keys::install_plugin_lib_dir(),
                                     "/tmp/${USER}/foo/plugins"
                                     );
    datatools_lib_infos.store_string(datatools::library_info::keys::env_plugin_lib_dir(),
                                     "FOO_PLUGIN_LIB_DIR"
                                     );

  }

  {
    std::clog << std::endl;
    std::string path = "@foo:data/${USER}/values.data";
    std::clog << "Original path : '" << path << "'" << std::endl;
    if (datatools::fetch_path_with_env(path)) {
      std::clog << "Fetched path  : '" << path << "'" << std::endl;
    } else {
      std::cerr << "Cannot fetch path  : '" << path << "'" << std::endl;
    }
  }

  {
    std::clog << std::endl;
    std::string path = "@foo:/data/${USER}/values.data";
    std::clog << "Original path : '" << path << "'" << std::endl;
    datatools::fetch_path_with_env(path);
    std::clog << "Fetched path  : '" << path << "'" << std::endl;
  }

  {
    std::clog << std::endl;
    std::string path = "@foo.resources:data/${USER}/values.data";
    std::clog << "Original path : '" << path << "'" << std::endl;
    if (datatools::fetch_path_with_env(path)) {
      std::clog << "Fetched path  : '" << path << "'" << std::endl;
    } else {
      std::cerr << "Cannot fetch path  : '" << path << "'" << std::endl;
    }
  }

  {
    std::clog << std::endl;
    std::string path = "@foo.plugins:releases/1.0";
    std::clog << "Original path : '" << path << "'" << std::endl;
    if (datatools::fetch_path_with_env(path)) {
      std::clog << "Fetched path  : '" << path << "'" << std::endl;
    } else {
      std::cerr << "Cannot fetch path  : '" << path << "'" << std::endl;
    }
  }

  {
    std::clog << std::endl;
    setenv("FOO_PLUGIN_LIB_DIR", "${HOME}/foo.d/plugins", 1);
    std::string path = "@foo.plugins:releases/1.0";
    std::clog << "Original path : '" << path << "'" << std::endl;
    if (datatools::fetch_path_with_env(path)) {
      std::clog << "Fetched path  : '" << path << "'" << std::endl;
    } else {
      std::cerr << "Cannot fetch path  : '" << path << "'" << std::endl;
    }
  }

  return;
}


int main(int /*argc*/, const char** /*argv*/)
{
  try {

    test_bit_manipulation();

    test_path_manipulation();

    test_quotes_manipulation();

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

// end of test_utils.cxx
