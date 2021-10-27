void Ie_vs_Ib_w_ratios(int n_modules, const char *input_file_name, const char *beam_energy, const char *bb_angle, const char *target, const char *output_file_name){
  
  double I_beam;
  double I_ex[n_modules]; //Excess current

  TString module[] = {"UVA_UV_Layer0", "UVA_UV_Layer1", "INFN_J0_Top", "INFN_J0_Middle", "INFN_J0_Bottom", "INFN_J2_Middle", "INFN_J2_Bottom", "UVA_XY_Back_Trcker_G0", "UVA_XY_Back_Tracker_G1","UVA_XY_Back_Tracker_G2", "UVA_XY_Back_Tracker_G3"};// This order has to match with the column order in the data file. //Will have to change here if no of modules changes.

  
  fstream file;

  file.open(input_file_name, ios::in); //Open the file with Excess Current (I_e) vs Beam current (I_b)

  TGraph *g[n_modules], *gratios[n_modules]; // Grpah for each modue for both type of plots.
  for (int i=0; i < n_modules; i++) {g[i] = new TGraph(); gratios[i] = new TGraph();};

  double x1[n_modules], x2[n_modules], y1[n_modules], y2[n_modules]; // To rember the first two data points with beam so that later we can make a straight line connecting those two points.
    
  int n = 0;
 
  //Putting data points in to the TGraphs
  while(1)
    {
      file >> I_beam >> I_ex[0] >> I_ex[1] >> I_ex[2] >> I_ex[3] >> I_ex[4] >> I_ex[5] >>  I_ex[6] >> I_ex[7] >> I_ex[8] >> I_ex[9] >> I_ex[10];// Will have to change here if no of modules changes.
      
      n = g[0]->GetN();// Get the number of data points so far
      
      for(int i=0; i < n_modules; i++){

	g[i]->SetPoint(n, I_beam, I_ex[i]);

       	if(n==1){x1[i] = I_beam; y1[i] = I_ex[i];}
	
	if(n==2){x2[i] = I_beam; y2[i] = I_ex[i];}
	  
      }

      if(file.eof()) break;
      
    }



  //Making the lines which connects the first two data points with beam on : to help get an idea about how the GEM "gain" vary.
   
  TF1 *f[n_modules];
  double gradient[n_modules]; // Gradient
  double intercept[n_modules]; // Intercept

  for(int i=0; i < n_modules; i++){

    gradient[i] = (y2[i]-y1[i])/(x2[i]-x1[i]);
    intercept[i] = y2[i] - gradient[i]*x2[i];
    
    f[i] = new TF1("f","[0]*x + [1]",0,g[0]->GetPointX(n));
    f[i]->SetParameter(0,gradient[i]);
    f[i]->SetParameter(1,intercept[i]);
    
  }
  

  //Also making the plots (I_observed - I_from_line)/I_frrom_line vs I_beam. These will give an idea how the gain of the chamber decreases as fraction compared to the initial gain during low luminosity.

  //TGraph *gratios[n_modules];
  double I_from_line;//Current calculatef from the line
  double I_excess_observed; //Observed excess current
  
  for(int i=0; i < n_modules; i++){
    
    for(int j=1; j < n+1; j++){
      
      I_beam = g[i]->GetPointX(j);
      
      I_from_line = gradient[i]*I_beam + intercept[i];

      I_excess_observed = g[i]->GetPointY(j);
      
      double ratio = (I_from_line - I_excess_observed)/I_from_line;

      gratios[i]->SetPoint(j-1, I_beam , ratio);

    }
    
  }



  //Making the plots

  TCanvas *c[n_modules];

   TString pdffilename = output_file_name;
  //pdffilename.ReplaceAll( ".root", ".pdf");
  //pdffilename.Prepend("plots/");

  TString openfilename = pdffilename+"(";
  TString closefilename = pdffilename+")";
  

  double lmargin=0.15;
  double rmargin=0.15;
  double bmargin=0.15;
  double tmargin=0.09;

  for(int i=0; i < n_modules; i++){
    
    c[i] = new TCanvas(module[i],module[i],1400,1050);
    
    c[i]->Divide(2,1);
    
    //I_excess vs I_beam plots
     
    c[i]->cd(1);//Put 1
    //c->cd(1);
    
    gPad->SetLeftMargin(lmargin);
    gPad->SetRightMargin(rmargin);
    gPad->SetBottomMargin(bmargin);
    gPad->SetTopMargin(tmargin);
    gPad->SetGrid();
    
    g[i]->SetTitle(module[i]+" || "+beam_energy+" GeV || BB at "+bb_angle+" degrees || "+target+" target");
    g[i]->GetXaxis()->SetTitle("Beam Current/ micro amphs");
    g[i]->GetYaxis()->SetTitle("Excess Current/ micro amphs");

    g[i]->SetMarkerColor(46);
    g[i]->SetMarkerSize(1.5);
    g[i]->SetMarkerStyle(20);
    g[i]->Draw("AP");
    f[i]->Draw("Same");
     
    //Excess Fraction vs I_beam plots
     
    c[i]->cd(2);
    //c->cd(2);
    
    gPad->SetLeftMargin(lmargin);
    gPad->SetRightMargin(rmargin);
    gPad->SetBottomMargin(bmargin);
    gPad->SetTopMargin(tmargin);
    gPad->SetGrid();
    
    
    gratios[i]->SetTitle(module[i]+" || "+beam_energy+" GeV || BB at "+bb_angle+" degrees || "+target+" target");
    gratios[i]->GetXaxis()->SetTitle("Beam Current/ micro amphs");
    gratios[i]->GetYaxis()->SetTitle("(Current From Line - Observed Excess Current)/Current From Line");
    
    gratios[i]->SetMarkerColor(4);
    gratios[i]->SetMarkerSize(1.5);
    gratios[i]->SetMarkerStyle(20);
    gratios[i]->Draw("AP");
    
    if(i==0)
      {
	//c[i]->Print("plots.pdf(","Title:"+module[i]);
	c[i]->Print(openfilename,"Title:"+module[i]);
	//c[i]->Print(openfilename,"Title:First");
      }
     
    if(i== n_modules-1)
      {
	//c[i]->Print("plots.pdf)","Title:"+module[i]);
	c[i]->Print(closefilename,"Title:"+module[i]);
	//c[i]->Print("plots.pdf]","Title:Middle");
      }
    
    if(i != 0 && i != n_modules-1 )
      {
	//c[i]->Print("plots.pdf","Title:"+module[i]);
	c[i]->Print(pdffilename,"Title:"+module[i]);
	//c[i]->Print(pdffilename,"Title:Last");
      }
    //Save as .png image
    //c[i]->SaveAs(module[i]+"_"+beam_energy+"GeV_"+bb_angle+"degrees_"+target+".png");
  }
  
}
