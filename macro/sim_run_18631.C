void sim_run_18631() { 

  TFile *f = new TFile("$QW_ROOTFILES/sbs_CntHouse_18631.root","READ");

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

  TCut rampcut[8] = {"ch_cleandata==1 && ch_scandata1==0 && bcm_dg_ds>4 && Entry$>39.8e3 && Entry$<41.4e3","ch_cleandata==1 && ch_scandata1==1 && bcm_dg_ds>4 && Entry$>83e3 && Entry$<84.5e3","ch_cleandata==1 && ch_scandata1==2 && bcm_dg_ds>4","ch_cleandata==1 && ch_scandata1==3 && bcm_dg_ds>4","ch_cleandata==1 && ch_scandata1==4 && bcm_dg_ds>4 && Entry$>153.2e3 && Entry$<154.7e3","ch_cleandata==1 && ch_scandata1==5 && bcm_dg_ds>4 && Entry$>169.4e3 && Entry$<170.9e3","ch_cleandata==1 && ch_scandata1==6 && bcm_dg_ds>4 && Entry$>181.8e3 && Entry$<183.4e3","ch_cleandata==1 && ch_scandata1==7 && bcm_dg_ds>4 && Entry$>194.6e3 && Entry$<196.1e3"};

  int num_magnet = (sizeof(magnet)/sizeof(*magnet));
  int num_bpm = (sizeof(bpm)/sizeof(*bpm));


  for (int j=0; j<num_magnet; j++) {

    TCanvas *c = new TCanvas(Form("%s",magnet[j].Data()),Form("%s",magnet[j].Data()));
    c->Divide(2,3);

    for (int i=0; i<num_bpm; i++) {
      c->cd(i+1);
      t->Draw(Form("%s:%s",bpm[i].Data(),magnet[j].Data()),rampcut[j]);
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

  gSystem->Exec("pdfunite $(ls -rt /adaqfs/home/apar/arindam/20250618_bmod_study/temp/*.pdf) /adaqfs/home/apar/arindam/20250618_bmod_study/magnet_run_18631x.pdf");
}
