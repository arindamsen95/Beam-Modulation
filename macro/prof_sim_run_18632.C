void prof_sim_run_18632() { 

  gStyle->SetOptStat(0);

  TFile *f = new TFile("$QW_ROOTFILES/sbs_CntHouse_18632.root","READ");

  if (!f || f->IsZombie()) {
    cout << "Error opening file !" << endl;
    return 1;
  }


  TTree *t = (TTree*)f->Get("evt");
  if (!t) {
    std::cout << "Error: TTree not found!" << std::endl;
    return;
  }

  TString magnet[]={"mhf1c08h","mhf1c08v","mhf1c10h","mhf1c10v","ramp_function","mhf1c02h","mhf1c03v","sl20_rf_vernier"};
  TString bpm[6]={"bpm11X","bpm12X","bpm16X","bpm1X","bpm4aX","bpm4eX"};
  //TString bpm[6]={"bpm11Y","bpm12Y","bpm16Y","bpm1Y","bpm4aY","bpm4eY"};


  TCut rampcut[8] = {"ch_cleandata==1 && ch_scandata1==0 && bcm_dg_ds>4 && Entry$>6.5e3 && Entry$<8.1e3","ch_cleandata==1 && ch_scandata1==1 && bcm_dg_ds>4 && Entry$>17.3e3 && Entry$<18.8e3","ch_cleandata==1 && ch_scandata1==2 && bcm_dg_ds>4 && Entry$>27e3 && Entry$<28.5e3","ch_cleandata==1 && ch_scandata1==3 && bcm_dg_ds>4 && Entry$>36.5e3 && Entry$<38.1e3","ch_cleandata==1 && ch_scandata1==4 && bcm_dg_ds>4 && Entry$>46.2e3 && Entry$<47.8e3","ch_cleandata==1 && ch_scandata1==5 && bcm_dg_ds>4 && Entry$>55.1e3 && Entry$<56.6e3","ch_cleandata==1 && ch_scandata1==6 && bcm_dg_ds>4 && Entry$>66.5e3 && Entry$<68.1e3","ch_cleandata==1 && ch_scandata1==7 && bcm_dg_ds>4 && Entry$>77.1e3 && Entry$<77.6e3"};

  int num_magnet = (sizeof(magnet)/sizeof(*magnet));
  int num_bpm = (sizeof(bpm)/sizeof(*bpm));
  
  ofstream myfile("slopes_run_18632_bpmx.txt");
  if(!myfile.is_open()){
    cerr << "Error opening file!" << endl;
    return;
  }

  myfile << "run, bpm, magnet, slope \n";

  TH2 *hist[num_bpm];
  TFitResultPtr fitres;

  for (int j=0; j<num_magnet; j++) {

    TCanvas *c = new TCanvas(Form("%s",magnet[j].Data()),Form("%s",magnet[j].Data()));
    c->Divide(2,3);

    for (int i=0; i<num_bpm; i++) {
      c->cd(i+1);
      t->Draw(Form("%s:%s >> h_%s_%s",bpm[i].Data(),magnet[j].Data(),bpm[i].Data(),magnet[j].Data()),rampcut[j],"prof");
      hist[i] = (TH2*)gDirectory->Get(Form("h_%s_%s",bpm[i].Data(),magnet[j].Data()));

      hist[i]->GetYaxis()->SetTitle(Form("%s",bpm[i].Data()));
      hist[i]->GetYaxis()->SetTitleOffset(1.52);
      hist[i]->GetXaxis()->SetTitle(Form("%s",magnet[j].Data()));

      TFitResultPtr tmpptr = hist[i]->Fit("pol1","S");
      int status = (int)tmpptr;
      if (status == 0) {
      Double_t p0 = tmpptr->Value(0);
      Double_t p0_err = tmpptr->Error(0);
      Double_t p1 = tmpptr->Value(1);
      Double_t p1_err = tmpptr->Error(1);

      TPaveText *pt = new TPaveText(0.13,0.7,0.3,0.9,"NDC BL");
      pt->AddText(Form("%s vs %s",bpm[i].Data(),magnet[j].Data()));
      pt->AddText(Form("p1 = %lf #pm %.5lf",p1,p1_err));
      pt->AddText(Form("p0 = %lf #pm %.5lf",p0,p0_err));
      pt->Draw();

      cout << bpm[i].Data() << "/" << magnet[j].Data() << setprecision(3) << fixed << "\t" << "====   " << p1*pow(10,6) << endl;

      myfile << (int)18632 << "  " << bpm[i].Data() << "  " << magnet[j].Data() << "  " << p1*pow(10,6) << endl;

      }
    }

   c->cd(6);
   TLatex *   tex = new TLatex(0.3970272,0.5028607,Form("%s",magnet[j].Data()));
   tex->SetTextSize(0.1676202);
   tex->SetLineWidth(2);
   tex->SetTextColor(kRed);
   tex->Draw();
   c->Modified();
    c->SaveAs(Form("/adaqfs/home/apar/arindam/20250618_bmod_study/temp/%s.pdf",magnet[j].Data()));
    c->Update();
  }

  gSystem->Exec("pdfunite $(ls -rt /adaqfs/home/apar/arindam/20250618_bmod_study/temp/*.pdf) /adaqfs/home/apar/arindam/20250618_bmod_study/magnet_prof_run_18632x.pdf");
  myfile.close();
}
