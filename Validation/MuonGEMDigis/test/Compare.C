#include "TCanvas.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TList.h"
#include "TGaxis.h"
#include "TMath.h"
#include <stdio.h>
#include <TROOT.h>
#include "TSystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>


void savePlot(TCanvas*, TString);
void prepareHisto (TH1F* hist, Int_t rebin, Double_t weight, Color_t color);

Double_t myfunction(Double_t *x, Double_t *par)
{
  Float_t xx =x[0];
  Double_t f = par[0]+par[1]*xx+par[2]*pow(xx,2)+par[3]*pow(xx,3)+par[4]*pow(xx,4)+par[5]*pow(xx,5)+par[6]*pow(xx,6);
  return f;
}

// My Branch :: Neutral Bkg
// neuBkg.push_back(899644.0); neuBkg.push_back(-30841.0); neuBkg.push_back(441.28);
// neuBkg.push_back(-3.3405); neuBkg.push_back(0.0140588); neuBkg.push_back(-3.11473e-05); neuBkg.push_back(2.83736e-08);
// My Branch :: Electron Bkg
// eleBkg.push_back(4.68590e+05); eleBkg.push_back(-1.63834e+04); eleBkg.push_back(2.35700e+02);
// eleBkg.push_back(-1.77706e+00); eleBkg.push_back(7.39960e-03); eleBkg.push_back(-1.61448e-05); eleBkg.push_back(1.44368e-08);

// 76X
// ME0ModNeuBkgParam0 = 5.69e+06;
// ME0ModNeuBkgParam1 = -293334;
// ME0ModNeuBkgParam2 = 6279.6;
// ME0ModNeuBkgParam3 = -71.2928;
// ME0ModNeuBkgParam4 = 0.452244;
// ME0ModNeuBkgParam5 = -0.0015191;
// ME0ModNeuBkgParam6 = 2.1106e-06;
// ME0ModElecBkgParam0 = 3.77712e+06;
// ME0ModElecBkgParam1 = -199280;
// ME0ModElecBkgParam2 = 4340.69;
// ME0ModElecBkgParam3 = -49.922;
// ME0ModElecBkgParam4 = 0.319699;
// ME0ModElecBkgParam5 = -0.00108113;
// ME0ModElecBkgParam6 = 1.50889e-06;

void TestME0Background()
{

  TH1::SetDefaultSumw2();
  cout<<"ciao"<<endl;
  TFile* file = TFile::Open("ME0Digis_ME_151217.root");
  TString tDir = "DQMData/MuonME0DigisV/ME0DigiTask";
  TString total     = tDir+"/NoPrompt/strip_dg_R_NoPrompt";
  TString electrons = tDir+"/NoPrompt/strip_dg_R_NoPrompt_e";
  TString neutrons  = tDir+"/NoPrompt/strip_dg_R_NoPrompt_n";
  TString gammas    = tDir+"/NoPrompt/strip_dg_R_NoPrompt_g";

  cout<<"ciao"<<endl;
  //TH1F* totalBkg     = (TH1F*) file->Get(total);
  TH1F* electronsBkg = (TH1F*) file->Get(electrons);
  TH1F* neutronsBkg  = (TH1F*) file->Get(neutrons);
  TH1F* gammasBkg    = (TH1F*) file->Get(gammas);
  //TH1F* neutralBkg   = neutronsBkg->Add(gammasBkg);

  cout<<"ciao  "<<electronsBkg<<"   "<<neutronsBkg <<"   "<< gammasBkg << endl;
  //Double_t NumEvents = totalBkg->Integral();
  Double_t weight    = 200*9*25e-09;

  prepareHisto(electronsBkg,5,weight,kRed+1);
  prepareHisto(neutronsBkg,5,weight,kOrange+1);
  prepareHisto(gammasBkg,5,weight,kGreen+2);

  cout<<"ciao"<<endl;
  TH1F* neutralBkg = (TH1F*) gammasBkg->Clone();
  neutralBkg->Add(neutronsBkg);
  neutralBkg->SetMarkerColor(kBlack);
  neutralBkg->SetLineColor(kBlack);

  cout<<"ciao"<<endl;
  TLegend *leg_hits = new TLegend(0.60,0.60,0.85,0.85);
  leg_hits->SetFillColor(0);
  leg_hits->SetHeader("ME0 Neutron Bkg Hits");
  leg_hits->AddEntry(neutralBkg,"n/#gamma Bkg","l");
  leg_hits->AddEntry(gammasBkg,"#gamma Bkg","l");
  leg_hits->AddEntry(electronsBkg,"e^{#pm} Bkg ","l");
  leg_hits->AddEntry(neutronsBkg,"n Bkg","l");

  TCanvas *c_noPrompt = new TCanvas("NoPrompt_Rate","NoPrompt_Rate",600,600);
  c_noPrompt->cd();
  c_noPrompt->SetTitle("ME0 Neutron Background Hits");
  neutralBkg->Draw("9p0e1");
  gammasBkg->Draw("9samep0e1");
  electronsBkg->Draw("9p0e1same");
  neutronsBkg->Draw("9p0e1same");
  leg_hits->Draw();

  c_noPrompt->Update();
  //savePlot(c_noPrompt,"NoPromptHitsRate");

  TCanvas *c = new TCanvas("BkgHitRate", "BkgHitRate", 600, 600);
  c->cd();
  c->SetTicks(1,1);
  c->SetGrid(1,1);
  c->SetLogy();
  c->SetTitle("ME0 Neutron Background Parameterization");
  // c->SetTitle("");

  TF1 *f1 = new TF1("myfunc1",myfunction,60,150,7);
  f1->SetParameters(899644.0, -30841.0, 441.28, -3.3405, 0.0140588, -3.11473e-05, 2.83736e-08);
  // f1->SetParNames("constant","coefficient");
  f1->Draw();

  TF1 *f2 = new TF1("myfunc2",myfunction,60,150,7);
  f2->SetParameters(4.68590e+05, -1.63834e+04, 2.35700e+02, -1.77706e+00, 7.39960e-03, -1.61448e-05, 1.44368e-08);
  f2->SetLineColor(kBlue);
  f2->Draw("same");

  TF1 *f3 = new TF1("myfunc3",myfunction,60,150,7);
  f3->SetParameters(5.69e+06, -293334, 6279.6, -71.2928, 0.452244, -0.0015191, 2.1106e-06);
  f3->SetLineColor(kRed);
  f3->SetLineStyle(2);
  f3->Draw("same");

  TF1 *f4 = new TF1("myfunc4",myfunction,60,150,7);
  f4->SetParameters(3.77712e+06, -199280, 4340.69, -49.922, 0.319699, -0.00108113, 1.50889e-06);
  f4->SetLineColor(kBlue);
  f4->SetLineStyle(2);
  f4->Draw("same");

  TLegend *leg = new TLegend(0.60,0.60,0.85,0.85);
  leg->SetFillColor(0);
  leg->SetHeader("ME0 Neutron Bkg Param");
  leg->AddEntry("myfunc1","n/#gamma Bkg SLHC","l");
  leg->AddEntry("myfunc2","e^{#pm} Bkg SLHC","l");
  leg->AddEntry("myfunc3","n/#gamma Bkg 76X","l");
  leg->AddEntry("myfunc4","e^{#pm} Bkg 76X","l");
  leg->Draw();


  f1->GetYaxis()->SetRangeUser(300.01,1.2E5);
  f1->GetYaxis()->SetTitleOffset(1.35);
  f1->GetYaxis()->SetTitle("Rate (Hz/cm^{2})");
  f1->GetXaxis()->SetTitle("R (cm)");
  c->Update();
  //savePlot(c,"BkgParamSLHC26p3");

  TCanvas* c_all = new TCanvas("Superposition","Superposition",600,600);
  c_all->cd();
  c_all->SetTicks(1,1);
  c_all->SetGrid(1,1);
  c_all->SetLogy();
  c_all->SetTitle("ME0 Neutron Background Parameterization");
  f1->Draw("FC"); f2->Draw("FCsame");
  neutralBkg->Draw("9p1e0same");
  gammasBkg->Draw("9p1e0same");
  electronsBkg->Draw("9p1e0same");
  neutronsBkg->Draw("9p1e0same");

  TLegend *leg2 = new TLegend(0.60,0.60,0.85,0.85);
  leg2->SetFillColor(0);
  leg2->SetHeader("ME0 Neutron Bkg Param and Hits");
  leg2->AddEntry("myfunc1","n/#gamma Bkg SLHC Model","l");
  leg2->AddEntry("myfunc2","e^{#pm} Bkg SLHC Model","l");
  leg2->AddEntry(neutralBkg,"n/#gamma Bkg","pl");
  leg2->AddEntry(gammasBkg,"#gamma Bkg","pl");
  leg2->AddEntry(electronsBkg,"e^{#pm} Bkg","pl");
  leg2->AddEntry(neutronsBkg,"n Bkg","pl");
  leg2->Draw();
  c_all->Update();


}

void savePlot(TCanvas * c, TString name) {
  // c->SaveAs(name+".root");
  c->SaveAs(name+".png");
  c->SaveAs(name+".eps");
  gSystem->Exec("epstopdf "+name+".eps");
  std::cout<<"Plots saved"<<std::endl;
}



void prepareHisto (TH1F* hist, Int_t rebin, Double_t weight, Color_t color){
  //std::cout<<" BinWidth : "<<hist->GetBinWidth(2)<<std::endl;
  //Double_t bottomLenght = 21.9762;
  //Double_t topLenght = 52.7218;
  //Double_t minBase = 0.;
  //Double_t maxBase = 0.;


  hist->Rebin(rebin);
  //std::cout<<" BinWidth dopo Rebin: "<<hist->GetBinContent(30)<<std::endl;
  //hist->Scale(1/weight);
  hist->SetStats(kFALSE);
  hist->SetLineColor(color);
  hist->SetMarkerColor(color);
  hist->SetMarkerStyle(kFullTriangleDown);
  hist->SetMarkerSize(0.7);
  hist->GetXaxis()->SetRangeUser(65,150);
  std::cout << " Histogram :  "<<hist->GetName() << " -------------  ";
  for (Int_t bin = 1; bin<=hist->GetNbinsX();++bin){
    //  if (bin == 1) minBase = bottomLenght;
    //  else minBase = maxBase;
    //  if (bin==hist->GetNbinsX()) maxBase = topLenght;

      Double_t R_min = hist->GetBinCenter(bin) - 0.5 * hist->GetBinWidth(bin);
      Double_t R_max = hist->GetBinCenter(bin) + 0.5 * hist->GetBinWidth(bin);
    //  Double_t DeltaR = R_max - R_min;
    //  maxBase = 2*DeltaR*0.1763269 + minBase ;

    //  Double_t trapArea = (0.5*DeltaR*(minBase + maxBase))*18*6*2; //18*6*2 fattori geometrici per tenere conto dei 6 layers, 18 camere e le 2 regioni

      Double_t Area     = (TMath::Pi() * ( TMath::Power(R_max,2) - TMath::Power(R_min,2) ))*12;
      std::cout << "\n #bin = "<<bin
              //  <<"\n\tArea = "<<Area<<"  Area Trap= "<<trapArea<<"  Weight = "<< Area*weight
                <<"\n\tBinContent = "<<hist->GetBinContent(bin)<<"  SetBinContent = "<<(hist->GetBinContent(bin))/(Area*weight)<<std::endl;
      hist->SetBinContent(bin,(hist->GetBinContent(bin))/(Area*weight));
  }

}
