// Include standard c++ stuff
#include <iostream>
#include <string>

// Include ROOT headers
#include "TCanvas.h"    // For drawing plots
#include "TFile.h"      // ROOT file
#include "TH1D.h"       // 1D (double type) histogram
#include "TH2D.h"       // 2D (double type) histogram
#include "TGraph.h"	// Graph
#include "TTree.h"      // Data tree

// Include our TTree header
#include "nemenixevent.h"

int main(int argc, char * argv[]) {
    
	
    if (argc < 2) {
        return 1;
    }

	//em hists
	//1d hists
	TH1D * h_em_ratio[argc - 1][4][8];
	TH1D * h_em_ratio_tot = new TH1D("h_em_ratio", "Ratio of integral to amplitude for em signals: All inputs", 100, 0, 10);
	h_em_ratio_tot->GetXaxis()->SetTitle("Ratio of integral to amplitude");
	h_em_ratio_tot->GetYaxis()->SetTitle("Number of counts");
	
	//2d hist
	TH2D * h_em_ratio_xy[argc - 1];
	TH2D * h_em_ratio_xy_tot = new TH2D ("h_em_ratio_xy", "Ratio of integral to amplitude for em signals comparison of x and y channels", 100, 0 , 10, 100, 0 ,10);
	h_em_ratio_xy_tot->GetXaxis()->SetTitle("Ratio of integral to amplitude from x axis");
	h_em_ratio_xy_tot->GetYaxis()->SetTitle("Number of counts from y axis");
	
	
	//neutron hists
	//1d hists
	TH1D * h_n_ratio[argc - 1][4][8];
	TH1D * h_n_ratio_tot = new TH1D("h_n_ratio", "Ratio of integral to amplitude for neutrons: All inputs", 100, 0, 100);
	h_n_ratio_tot->GetXaxis()->SetTitle("Ratio of integral to amplitude");
	h_n_ratio_tot->GetYaxis()->SetTitle("Number of counts");
	
	//2d hist
	TH2D * h_n_ratio_xy[argc - 1];
	TH2D * h_n_ratio_xy_tot = new TH2D ("h_n_ratio_xy", "Ratio of integral to amplitude for neutrons comparison of x and y channels", 100, 0 , 100, 100, 0 ,100);
	h_n_ratio_xy_tot->GetXaxis()->SetTitle("Ratio of integral to amplitude from x axis");
	h_n_ratio_xy_tot->GetYaxis()->SetTitle("Number of counts from y axis");
	
	
	//comparison hist
	TH2D * h_n_em_ratio_xy[argc - 1];
	TH2D * h_n_em_ratio_xy_tot = new TH2D ("h_n_em_ratio_xy", "Ratio of integral to amplitude for both neutron and em signals: comparison of x and y channels", 100, 0 , 100, 100, 0 ,100);
	h_n_em_ratio_xy_tot->GetXaxis()->SetTitle("Ratio of integral to amplitude from x axis");
	h_n_em_ratio_xy_tot->GetYaxis()->SetTitle("Number of counts from y axis");
	
	//Setup canvas
    TCanvas * canv = new TCanvas("canv", "canv");
    canv->SetLogz();
    

	//Declare event array for each input file
	TFile * inp[argc - 1];
	TTree * tree[argc - 1];
	long nevt[argc - 1];
	nemenixevent * event[argc - 1];
	
	for (int iii = 0; iii < argc - 1; iii++) {
	
		//Define events for each input file 
		inp[iii] = new TFile(argv[iii + 1], "READ");
		tree[iii] = (TTree *) inp[iii]->Get("events");
		nevt[iii] = tree[iii]->GetEntries();
		event[iii] =  new nemenixevent(tree[iii]);
		
		TFile * outp = new TFile("test.root", "RECREATE");
		outp->cd();
		
		for (int board = 0; board < 4; board++) {
			for (int chan = 0; chan < 8; chan++) {
				
				//em hists
				h_em_ratio[iii][board][chan] = new TH1D (Form("h_em_ratio %d %d", board, chan), Form("Ratio of integral to amplitude for ems: board%d chan%d", board, chan), 100, 0 , 10);
				h_em_ratio[iii][board][chan]->GetXaxis()->SetTitle("Ratio of integral to amplitude");
				h_em_ratio[iii][board][chan]->GetYaxis()->SetTitle("Number of counts");
				
				
				//neutron hists
				h_n_ratio[iii][board][chan] = new TH1D (Form("h_n_ratio %d %d", board, chan), Form("Ratio of integral to amplitude for neutrons: board%d chan%d", board, chan), 100, 0 , 100);
				h_n_ratio[iii][board][chan]->GetXaxis()->SetTitle("Ratio of integral to amplitude");
				h_n_ratio[iii][board][chan]->GetYaxis()->SetTitle("Number of counts");
			}
		}
		
		//define 2d histograms
		h_em_ratio_xy[iii] = new TH2D ("h_em_ratio_xy", "Ratio of integral to amplitude for ems comparison of x and y channels", 100, 0 , 100, 100, 0 ,100);
				h_em_ratio_xy[iii]->GetXaxis()->SetTitle("Ratio of integral to amplitude from x axis");
				h_em_ratio_xy[iii]->GetYaxis()->SetTitle("Number of counts from y axis");
				
		
		//define 2d histograms
		h_n_ratio_xy[iii] = new TH2D ("h_n_ratio_xy", "Ratio of integral to amplitude for neutrons comparison of x and y channels", 100, 0 , 100, 100, 0 ,100);
				h_n_ratio_xy[iii]->GetXaxis()->SetTitle("Ratio of integral to amplitude from x axis");
				h_n_ratio_xy[iii]->GetYaxis()->SetTitle("Number of counts from y axis");
				
		//comparison hist
		h_n_em_ratio_xy[iii] = new TH2D ("h_n_em_ratio_xy", "Ratio of integral to amplitude for both neutron and em signals: comparison of x and y channels", 100, 0 , 10, 100, 0 ,10);
		h_n_em_ratio_xy[iii]->GetXaxis()->SetTitle("Ratio of integral to amplitude from x axis");
		h_n_em_ratio_xy[iii]->GetYaxis()->SetTitle("Number of counts from y axis");
		
		//Loop over events
		for (int ievt = 0; ievt < nevt[iii]; ievt++) {
			
			event[iii]->GetEntry(ievt);
			int nem = event[iii]->peak_n;
			int nn = event[iii]->neutron_n;
			
			//plot if em number = 2 and events do not include board 1 chan 7
			if (nem == 2) {
				int count_em = 0;
				for (int iem = 0; iem < nem; iem++) {
					if (event[iii]->peak_board->at(iem) == 1 && 
						event[iii]->peak_channel->at(iem) == 7) {
						count_em++;
					}
					if (event[iii]->peak_amplitude->at(iem) > 60) {
					count_em++;
					}
				}
				if (count_em == 0) {
					double r_xchan, r_ychan;
					for (int iem = 0; iem < nem; iem++) {
						
						//calculate value
						double n_int = event[iii]->peak_integral->at(iem);
						double n_amp = event[iii]->peak_amplitude->at(iem);
						double r = n_int / n_amp;
						
						if (event[iii]->peak_channel->at(iem) < 4) {
							r_xchan = n_int / n_amp;
						}
						else {
							r_ychan = n_int / n_amp;
						}
						
						//fill corecct hist+ total hist
						h_em_ratio[iii][event[iii]->peak_board->at(iem)][event[iii]->peak_channel->at(iem)]->Fill(r);
						h_em_ratio_tot->Fill(r);
					}
					
					//fill 2d hist
					h_em_ratio_xy[iii]->Fill(r_xchan, r_ychan);
					h_em_ratio_xy_tot->Fill(r_xchan, r_ychan);
					h_n_em_ratio_xy[iii]->Fill(r_xchan, r_ychan);
					h_n_em_ratio_xy_tot->Fill(r_xchan, r_ychan);
				}
			}
			
			if (nn == 2) {
				int count = 0;
				for (int in = 0; in < nn; in++) {
					if (event[iii]->neutron_board->at(in) == 1 && 
						event[iii]->neutron_channel->at(in) == 7) {
						count++;
					}
					if (event[iii]->neutron_amplitude->at(in) > 60) {
					count++;
					}
				}
				if (count == 0) {
					double r_xchan, r_ychan;
					for (int in = 0; in < nn; in++) {
						
						//calculate value
						double n_int = event[iii]->neutron_integral->at(in);
						double n_amp = event[iii]->neutron_amplitude->at(in);
						double r = n_int / n_amp;
						
						if (event[iii]->neutron_channel->at(in) < 4) {
							r_xchan = n_int / n_amp;
						}
						else {
							r_ychan = n_int / n_amp;
						}
						
						//fill corecct hist+ total hist
						h_n_ratio[iii][event[iii]->neutron_board->at(in)][event[iii]->neutron_channel->at(in)]->Fill(r);
						h_n_ratio_tot->Fill(r);
					}
					
					//fill 2d hist
					h_n_ratio_xy[iii]->Fill(r_xchan, r_ychan);
					h_n_ratio_xy_tot->Fill(r_xchan, r_ychan);
					h_n_em_ratio_xy[iii]->Fill(r_xchan, r_ychan);
					h_n_em_ratio_xy_tot->Fill(r_xchan, r_ychan);
				}
			}
		}
		
		//draw histograms
		for (int board = 0; board < 4; board++) {
			for (int chan = 0; chan < 8; chan++) {
	    		
	    		char name[100] = {0};
				string str_i = to_string(iii);
				const char * cstr_i = str_i.c_str();
				string str_c = to_string(chan);
				const char * cstr_c = str_c.c_str();
				string str_b = to_string(board);
				const char * cstr_b = str_b.c_str();
				strcat(name, "imgs/em/ratio/h_em_ratio_inp");
				strcat(name, cstr_i);
				strcat(name, "_board");
				strcat(name, cstr_b);
				strcat(name, "_chan");
				strcat(name, cstr_c);
				strcat(name, ".png");
	    		
	    		h_em_ratio[iii][board][chan]->Draw();
				canv->Update();
				canv->SaveAs(name); 
    			h_em_ratio[iii][board][chan]->Write();
    			
    			strcpy(name, "");
    			strcat(name, "imgs/neutron/ratio/h_n_ratio_inp");
				strcat(name, cstr_i);
				strcat(name, "_board");
				strcat(name, cstr_b);
				strcat(name, "_chan");
				strcat(name, cstr_c);
				strcat(name, ".png");
	    		
	    		h_n_ratio[iii][board][chan]->Draw();
				canv->Update();
				canv->SaveAs(name); 
    			h_n_ratio[iii][board][chan]->Write();
    		}
    	}
    	
    	char name[100] = {0};
		string str_i = to_string(iii);
		const char * cstr_i = str_i.c_str();
    	strcat(name, "imgs/em/ratio/h_em_ratio_xy_inp");
		strcat(name, cstr_i);
		strcat(name, ".png");
		
		h_em_ratio_xy[iii]->Draw("colz");
		canv->Update();
		canv->SaveAs(name); 
    	h_em_ratio_xy[iii]->Write();
    	
    	strcpy(name, "");
    	strcat(name, "imgs/neutron/ratio/h_n_ratio_xy_inp");
		strcat(name, cstr_i);
		strcat(name, ".png");
		
		h_n_ratio_xy[iii]->Draw("colz");
		canv->Update();
		canv->SaveAs(name); 
    	h_n_ratio_xy[iii]->Write();
    	
    	strcpy(name, "");
    	strcat(name, "imgs/h_n_em_ratio_xy_inp");
		strcat(name, cstr_i);
		strcat(name, ".png");
		
		h_n_em_ratio_xy[iii]->Draw("colz");
		canv->Update();
		canv->SaveAs(name); 
    	h_n_em_ratio_xy[iii]->Write();
		
    	inp[iii]->Close();
    	outp->Close();
	}
	
	TFile * outp = new TFile("test.root", "RECREATE");
	outp->cd();
	
	//Draw summed histograms
	h_em_ratio_tot->Draw();
	canv->Update();
	canv->SaveAs("imgs/em/ratio/h_em_ratio_tot.png"); 
	h_em_ratio_tot->Write();
	
	h_em_ratio_xy_tot->Draw("colz");
	canv->Update();
	canv->SaveAs("imgs/em/ratio/h_em_ratio_xy_tot.png"); 
	h_em_ratio_xy_tot->Write();
	
	//Draw summed histograms
	h_n_ratio_tot->Draw();
	canv->Update();
	canv->SaveAs("imgs/neutron/ratio/h_n_ratio_tot.png"); 
	h_n_ratio_tot->Write();
	
	h_n_ratio_xy_tot->Draw("colz");
	canv->Update();
	canv->SaveAs("imgs/neutron/ratio/h_n_ratio_xy_tot.png"); 
	h_n_ratio_xy_tot->Write();
	
	h_n_em_ratio_xy_tot->Draw("colz");
	canv->Update();
	canv->SaveAs("imgs/h_n_em_ratio_xy_tot.png"); 
	h_n_em_ratio_xy_tot->Write();
	
	outp->Close();
	
	return 0;
}
