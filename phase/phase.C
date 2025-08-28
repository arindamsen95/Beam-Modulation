  /////////////////////////////////////////////////////////////
  // Script for getting the frequency and
  // phase difference between the magnet and bpms
  // author : Arindam Sen
  // email : asen@jlab.org
  /////////////////////////////////////////////////////////////

  void phase(int run_num=18632) {
  TFile *f = new TFile(Form("/chafs2/work1/parity/japanOutput/sbs_CntHouse_%d.root",run_num),"READ");

  if (!f || f->IsZombie()) {
    cout << "Error opening file !" << endl;
    return 1;
  }

  TTree *t = (TTree*)f->Get("evt");
  if (!t) {
    std::cout << "Error: TTree not found!" << std::endl;
    return;
  }

  gSystem->mkdir("plots");

  TString magnet[]={"mhf1c08h","mhf1c08v","mhf1c10h","mhf1c10v","ramp_function","mhf1c02h","mhf1c03v","sl20_rf_vernier"};
  TString bpm[]={"bpm11X","bpm12X","bpm16X","bpm1X","bpm4aX","bpm4eX"};
  //TString bpm[6]={"bpm11Y","bpm12Y","bpm16Y","bpm1Y","bpm4aY","bpm4eY"};

  TCut rampcut[8] = {"ch_cleandata==1 && ch_scandata1==0 && bcm_dg_ds>4 && Entry$>7e3 && Entry$<7.1e3","ch_cleandata==1 && ch_scandata1==1 && bcm_dg_ds>4 && Entry$>18e3 && Entry$<18.1e3","ch_cleandata==1 && ch_scandata1==2 && bcm_dg_ds>4 && Entry$>28e3 && Entry$<28.1e3","ch_cleandata==1 && ch_scandata1==3 && bcm_dg_ds>4 && Entry$>38e3 && Entry$<38.1e3","ch_cleandata==1 && ch_scandata1==4 && bcm_dg_ds>4 && Entry$>47e3 && Entry$<47.1e3","ch_cleandata==1 && ch_scandata1==5 && bcm_dg_ds>4 && Entry$>56e3 && Entry$<56.1e3","ch_cleandata==1 && ch_scandata1==6 && bcm_dg_ds>4 && Entry$>67e3 && Entry$<67.1e3","ch_cleandata==1 && ch_scandata1==7 && bcm_dg_ds>4 && Entry$>78e3 && Entry$<78.04e3"};

  int num_magnet = (sizeof(magnet)/sizeof(*magnet));
  int num_bpm = (sizeof(bpm)/sizeof(*bpm));

  for (int j=0; j<num_bpm; j++) {
    TCanvas *c1 = new TCanvas(Form("%s",bpm[j].Data()),Form("%s",bpm[j].Data()));
    c1->Divide(2,4);
    for (int i=0; i<num_magnet; i++) {
      c1->cd(i+1);
      //for (int j=0; j<1; j++) {
      //  c1->cd(j+1);
      //  for (int i=0; i<1; i++) {
      // TCanvas *c = new TCanvas(Form("%s-%s",bpm[j].Data(),magnet[i].Data()),Form("%s-%s",bpm[j].Data(),magnet[i].Data()));
      TPad *c = (TPad*)gPad;
      c->Divide(2,1);
      c->cd(1);
      gPad->SetLeftMargin(0.1627346);
      t->Draw(Form("%s:Entry$",magnet[i].Data()),rampcut[i],"l*");

      TGraph *g = (TGraph*)gPad->GetPrimitive("Graph");
      TAxis *xaxis = g->GetHistogram()->GetXaxis();
      TF1 *fitFunc = new TF1("fitFunc", "[0]*sin([1]*x + [2]) + [3]", xaxis->GetXmin(), xaxis->GetXmax());

      if (magnet[i]=="sl20_rf_vernier"){
        fitFunc->SetParameter(0, 3);  // Amplitude guess
        fitFunc->SetParameter(1, 1.55);    // Angular frequency guess (2π*f)
        fitFunc->SetParameter(2, 0.0);    // Phase guess
        fitFunc->SetParameter(3, -256);    // Offset guess
      }
      else {
        fitFunc->SetParameter(0, 10000);  // Amplitude guess
        fitFunc->SetParameter(1, 0.399);    // Angular frequency guess (2π*f)
        fitFunc->SetParameter(2, 0.0);    // Phase guess
        fitFunc->SetParameter(3, 150);    // Offset guess
      }
      g->Fit("fitFunc", "R");
      double phaseReduced = fmod(fitFunc->GetParameter(2), 2*TMath::Pi());
      if (phaseReduced < 0) phaseReduced += 2*TMath::Pi();


      TPaveText* paveText = new TPaveText(0.18, 0.84, 0.32, 0.89, "NDC");
      paveText->SetBorderSize(1);
      paveText->SetFillColor(0);
      paveText->SetTextAlign(12);
      paveText->AddText(Form("Freq: %.4f", fitFunc->GetParameter(1)));
      paveText->AddText(Form("Phase: %.3f", phaseReduced));
      paveText->Draw();

      c->cd(2);
      gPad->SetLeftMargin(0.1336314);
      t->Draw(Form("%s:Entry$",bpm[j].Data()),rampcut[i],"l*");
      TGraph *g2 = (TGraph*)gPad->GetPrimitive("Graph");
      TAxis *xaxis2 = g2->GetHistogram()->GetXaxis();
      TF1 *fitFunc2 = new TF1("fitFunc2", "[0]*sin([1]*x + [2]) + [3]", xaxis2->GetXmin(), xaxis2->GetXmax());
      fitFunc2->SetParameter(0, 0.1);  // Amplitude guess
      fitFunc2->FixParameter(1, fitFunc->GetParameter(1));    // Angular frequency guess (2π*f)
      fitFunc2->SetParameter(2, 0.0);    // Phase guess
      fitFunc2->SetParameter(3, 0.15);    // Offset guess
      g2->Fit("fitFunc2", "R");

      double phaseReduced2 = fmod(fitFunc2->GetParameter(2), 2*TMath::Pi());
      if (phaseReduced2 < 0) phaseReduced2 += 2*TMath::Pi();


      TPaveText* paveText2 = new TPaveText(0.14, 0.84, .28, 0.89, "NDC");
      paveText2->SetBorderSize(1);
      paveText2->SetFillColor(0);
      paveText2->SetTextAlign(12);
      paveText2->AddText(Form("Freq: %.4f", fitFunc2->GetParameter(1)));
      paveText2->AddText(Form("Phase: %.3f", phaseReduced2));
      paveText2->Draw();

      c->cd();
      TPad *overlay = new TPad("overlay","overlay", 0,0, 1,1);
      overlay->SetFillStyle(4000);       // transparent
      overlay->SetFrameFillStyle(4000);  // transparent frame
      overlay->SetMargin(0,0,0,0);
      overlay->Draw();
      overlay->cd();

      TLatex lat;
      lat.SetNDC();                      // NDC coords of the overlay pad
      lat.SetTextAlign(22);              // centered
      lat.SetTextSize(0.035);            // adjust to taste
      lat.SetTextColor(kBlue);
      lat.DrawLatex(0.5, 0.92, Form("Phase difference %0.3f rad / %0.2f#circ", abs(phaseReduced-phaseReduced2), abs(phaseReduced-phaseReduced2)*180/TMath::Pi()));

      c->Update();
    }

     const char* currentDir = gSystem->pwd();
     std::string fullPath = std::string(currentDir) + "/" + "plots";
     std::string filePath = fullPath + Form("/%s.png",bpm[j].Data());
     c1->SaveAs(filePath.c_str()); 
     c1->Update();
    }
  }
