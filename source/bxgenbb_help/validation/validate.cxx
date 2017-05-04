#include <string>
#include <iostream>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>

#include <TFile.h>
#include <TH1D.h>
#include <TClass.h>
#include <TKey.h>
#include <TKey.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TApplication.h>

#include <datatools/exception.h>

struct config {
  bool devel;
  bool verbose;
  bool interactive;
  std::string path;
  std::string pg_name;
  config() {
    devel = false;
    interactive = false;
    verbose = false;
  }
};

void run(const config & config_);


int main(int argc_, char ** argv_) {
  TApplication theApp("Test", &argc_, argv_);

  config CFG;

  int iarg=1;
  while (iarg < argc_) {
    std::string token = argv_[iarg];
    if (token[0] == '-') {
      std::string option = token;
      if (option == "-d" || option == "--devel") {
        CFG.devel = true;
      } else if (option == "-i" || option == "--interactive") {
        CFG.interactive = true;
      }else if (option == "-v" || option == "--verbose") {
        CFG.verbose = true;
      }
    } else {
      std::string argument = token;
      if (CFG.pg_name.empty()) {
        CFG.pg_name = argument;
      } else if (CFG.path.empty()) {
        CFG.path = argument;
      }
    }
    iarg++;
  }

  if (CFG.pg_name.empty()) CFG.pg_name = "Am241";
  if (CFG.path.empty()) CFG.path = "/transcend/work/tmp";

  run(CFG);

  return 0;
}


void run(const config & config_) {
  std::string fn0 = config_.path + '/' + "histos_" + config_.pg_name + "-cpp.root";
  std::string fn1 = config_.path + '/' + "histos_" + config_.pg_name + "-fortran.root";
  DT_THROW_IF(! boost::filesystem::exists(fn0), std::runtime_error,
              "File '" << fn0 << "' does not exist !");
  DT_THROW_IF(! boost::filesystem::exists(fn1), std::runtime_error,
              "File '" << fn1 << "' does not exist !");
  if (config_.verbose) {
    std::clog << "decay0 C++ port output file     : " << fn0 << '\n';
    std::clog << "decay0 Fortran port output file : " << fn1 << '\n';
  }
  TFile *f0 = new TFile(fn0.c_str(), "r");
  TFile *f1 = new TFile(fn1.c_str(), "r");
  TIter next0(f0->GetListOfKeys());
  TIter next1(f1->GetListOfKeys());
  TKey *key0 = 0;
  TKey *key1 = 0;
  TCanvas canvas0;
  canvas0.Divide(1,2);
  while ((key0 = (TKey*)next0()) && (key1 =(TKey*)next1())) {
    TClass *cl0 = gROOT->GetClass(key0->GetClassName());
    //if (! cl0->InheritsFrom("TH1")) continue;
    std::string kcn0 = key0->GetClassName();
    if (config_.verbose) {
      std::clog << "New loop :\n";
      std::clog << "\t From C++ port     : Key '" << key0->GetName()
                << " of class " << kcn0
                << '\n';
    }
    std::string fn0 = config_.path + '/' + "histos_" + config_.pg_name + "-cpp.root";
    TClass *cl1 = gROOT->GetClass(key1->GetClassName());
    std::string kcn1 = key1->GetClassName();
    if (config_.verbose) {
      std::clog << "\t From Fortran port : Key '" << key1->GetName()
                << " of class " << kcn1
                << '\n';
    }
    if (key1 == 0) {
      if (config_.verbose) std::cerr << "\tNo key1. Skip.\n";
      continue;
    }
    if (kcn0 != "TH1D" || kcn1 != "TH1D") {
      if (config_.verbose) std::cerr << "\tNot TH1D. Skip.\n";
      continue;
    }
    TH1D *h0 = (TH1D*) key0->ReadObj();
    TH1D *h1 = (TH1D*) key1->ReadObj();
    double hn0 = h0->GetEntries();
    double hn1 = h1->GetEntries();
    if (hn0 < 1 || hn1 < 1) {
      if (config_.verbose) std::cerr << "\tNo entries. Skip.\n";
      continue;
    }
    std::string ht0 = h0->GetName();
    std::string ht1 = h1->GetName();
    if (config_.verbose)
      std::clog << "\nCompare histograms :\n"
                << "\t\"" << ht0 << '"' << '\n'
                << "\t\"" << ht1 << '"' << '\n';
    if (config_.interactive) {
      canvas0.cd(1);
      h0->Draw();
      canvas0.cd(2);
      h1->Draw();
      canvas0.Update();
    }
    if (config_.verbose) std::clog << " --- Kolmogorov test --- \n";
    double probK01 = h0->KolmogorovTest(h1, "ND");
    if (config_.verbose) std::clog << " Kolmogorov prob = : " << probK01 << '\n';
    // Fit results:
    std::string title = key0->GetName();
    boost::algorithm::replace_last(title, ".cpp", "");
    std::cout << '#' << " " << config_.pg_name << " " << title << " " << probK01 << '\n';

    if (config_.interactive) {
      std::clog << "Hit '[Enter]' to continue of 'q [Enter]' to quit : ";
      std::string dummy;
      std::getline(std::cin, dummy);
      if (dummy == "q") break;
    }
  }
  return;
}
