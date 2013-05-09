#include <string>
#include <iostream>

#include <TFile.h>
#include <TH1D.h>
#include <TClass.h>
#include <TKey.h>
#include <TKey.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TApplication.h>

void run(const std::string & path_, const std::string & pg_name_) {
  std::string fn0 = path_ + '/' + "histos_" + pg_name_ + "-cpp.root";
  std::string fn1 = path_ + '/' + "histos_" + pg_name_ + "-fortran.root";
  std::clog << "decay0 C++ port output file     : " << fn0 << '\n';
  std::clog << "decay0 Fortran port output file : " << fn1 << '\n';
  TFile *f0 = new TFile(fn0.c_str(), "r");
  TFile *f1 = new TFile(fn1.c_str(), "r");
  TIter next0(f0->GetListOfKeys());
  TIter next1(f1->GetListOfKeys());
  TKey *key0 = 0;
  TKey *key1 = 0;
  while ((key0 = (TKey*)next0()) && (key1 =(TKey*)next1())) {
    TClass *cl0 = gROOT->GetClass(key0->GetClassName());
    //if (! cl0->InheritsFrom("TH1")) continue;
    std::string kcn0 = key0->GetClassName();
    std::clog << "New loop :\n";
    std::clog << "\t From C++ port     : Key '" << key0->GetName()
              << " of class " << kcn0
              << '\n';
    std::string fn0 = path_ + '/' + "histos_" + pg_name_ + "-cpp.root";
    ;
    TClass *cl1 = gROOT->GetClass(key1->GetClassName());
    std::string kcn1 = key1->GetClassName();
    std::clog << "\t From Fortran port : Key '" << key1->GetName()
              << " of class " << kcn1
              << '\n';
    if (key1 == 0) {
      std::cout << "\tNo key1. Skip.\n";
      continue;
    }
    if (kcn0 != "TH1D" || kcn1 != "TH1D") {
      std::cout << "\tNot TH1D. Skip.\n";
      continue;
    }
    TH1D *h0 = (TH1D*) key0->ReadObj();
    TH1D *h1 = (TH1D*) key1->ReadObj();
    double hn0 = h0->GetEntries();
    double hn1 = h1->GetEntries();
    if (hn0 < 1 || hn1 < 1) {
      std::cout << "\tNo entries. Skip.\n";
      continue;
    }
    std::string ht0 = h0->GetName();
    std::string ht1 = h1->GetName();
    std::cout << "Compare histograms :\n"
              << "\t\"" << ht0 << '"' << '\n'
              << "\t\"" << ht1 << '"' << '\n';
    TCanvas canvas0;
    canvas0.Divide(1,2);
    canvas0.cd(1);
    h0->Draw();
    canvas0.cd(2);
    h1->Draw();
    canvas0.Update();
    std::cout << " --- Kolmogorov test --- \n";
    double probK01 = h0->KolmogorovTest(h1, "ND");
    std::cout << " Kolmogorov prob = : " << probK01 << '\n';
    {
      std::clog << "Hit '[Enter]' to continue of 'q[Enter]' to quit : ";
      std::string dummy;
      std::getline(std::cin, dummy);
      if (dummy == "q") break;
    }
  }
  return;
}

int main(int argc_, char ** argv_) {
  TApplication theApp("Test", &argc_, argv_);
  std::string path;
  std::string pg;
  if (argc_ > 1) pg = argv_[1];
  if (argc_ > 2) path = argv_[2];
  if (pg.empty()) pg = "Am241";
  if (path.empty())path = "/transcend/work/tmp";
  run(path, pg);
  return 0;
}

